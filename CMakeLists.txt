cmake_minimum_required(VERSION 2.6)
project(effects_viewer)

#set(CMAKE_LIBRARY_PATH "E:\\libs\\glew-2.0.0\\")

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

if (WIN32) 
	find_package(OpenGL REQUIRED) 
	set(CMAKE_PREFIX_PATH "e:/libs/glew-2.0.0")
	set(CMAKE_LIBRARY_PATH "e:/libs/glew-2.0.0/lib/Release/Win32/")
	find_package(GLEW REQUIRED)
	set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "e:/libs/SDL2-2.0.5")
	set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "e:/libs/SDL2-2.0.5/lib/x86")
	find_package(SDL2 REQUIRED)
	set(GLM_INCLUDE_DIRS "e:/libs/glm-0.9.8.3")
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
					src/graphics/window.cpp
					src/graphics/shader.cpp
					src/graphics/mesh.cpp 
					src/graphics/mesh_gen.cpp 
					)

add_executable(fxview ${SOURCE_FILES})

target_link_libraries(fxview	${SDL2_LIBRARY} 
								${OPENGL_LIBRARIES}
								${GLEW_LIBRARIES}
								)

#set_target_properties(fxview PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
