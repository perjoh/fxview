cmake_minimum_required(VERSION 2.6)
project(kvant)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

if (WIN32) 

# Environment variable "LIBS" must be set to path to libraries.

	find_package(OpenGL REQUIRED) 

	# GLEW
	find_path(MY_GLEW_PATH "include/GL/glew.h" PATHS "$ENV{LIBS}/*")
	set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${MY_GLEW_PATH})
	set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "${MY_GLEW_PATH}/lib/Release/Win32/")
	find_package(GLEW REQUIRED)

	# SDL
	find_path(MY_SDL_PATH "include/SDL.h" PATHS "$ENV{LIBS}/*")
	set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${MY_SDL_PATH})
	set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "${MY_SDL_PATH}/lib/x86")
	find_package(SDL2 REQUIRED)

	# GLM
	find_path(MY_GLM_PATH "glm/glm.hpp" PATHS "$ENV{LIBS}/*")
	set(GLM_INCLUDE_DIRS ${MY_GLM_PATH})

	add_compile_options(/W4)

else()

	add_compile_options(-std=c++11 -Wall -g)

#set(SDL2_INCLUDE_DIRS /usr/include/SDL2)
#set(SDL2_LIBRARIES /usr/lib/x86_64-linux-gnu/libSDL2.so) 

#find_path(GLEW_INCLUDE_DIR GL/glew.h)
#find_library(GLEW_LIBRARY NAMES GLEW PATH_SUFFIXES x86_64-linux-gnu)
#set(GLEW_INCLUDE_DIR /usr/include/SDL2)
#set(GLEW_LIBRARY /usr/lib/x86_64-linux-gnu/libGLEW.so) 
endif() 

include_directories(	${SDL2_INCLUDE_DIR} 
						${OPENGL_INCLUDE_DIRS} 
						${GLEW_INCLUDE_DIRS}
						${GLM_INCLUDE_DIRS}
						)

set(SOURCE_FILES	src/main.cpp
					src/base/task_runner.cpp
					src/base/frame_time.cpp
					src/graphics/window.cpp
					src/graphics/shader.cpp
					src/graphics/mesh.cpp
					src/graphics/mesh_gen.cpp
					src/graphics/render.cpp
					src/graphics/shape_gen.cpp
					)

add_custom_target(pre_build_step 
					python bin2cpp.py src/graphics/shaders/basic_fs.glsl graphics::shader::basic > src/graphics/shaders/basic_fs.h)

add_executable(kvant ${SOURCE_FILES})

target_link_libraries(kvant	${SDL2_LIBRARY} 
								${OPENGL_LIBRARIES}
								${GLEW_LIBRARIES}
								)

add_executable(tests 	tests/main.cpp
						tests/quad_tree.cpp
						tests/shapes.cpp)

add_dependencies(${PROJECT_NAME} pre_build_step) 

#set_target_properties(kvant PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
