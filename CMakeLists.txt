cmake_minimum_required(VERSION 2.6)
project(effects_viewer)

find_package(OpenGL)
#find_package(GLEW REQUIRED)


set(SDL2_INCLUDE_DIRS /usr/include/SDL2)
set(SDL2_LIBRARY /usr/lib/x86_64-linux-gnu/libSDL2.so) 

find_path(GLEW_INCLUDE_DIR GL/glew.h)
#find_library(GLEW_LIBRARY NAMES GLEW PATH_SUFFIXES x86_64-linux-gnu)
#set(GLEW_INCLUDE_DIR /usr/include/SDL2)
set(GLEW_LIBRARY /usr/lib/x86_64-linux-gnu/libGLEW.so) 

add_compile_options(-std=c++11 -Wall -g)

include_directories(${SDL2_INCLUDE_DIRS} 
					${OPENGL_INCLUDE_DIRS} 
					${GLEW_INCLUDE_DIR}
					)

set(SOURCE_FILES	src/main.cpp 
					src/graphics/window.cpp
					src/graphics/shader.cpp
					src/graphics/mesh.cpp
					)

add_executable(fxview ${SOURCE_FILES})

target_link_libraries(fxview	${SDL2_LIBRARY} 
								${OPENGL_LIBRARIES}
								${GLEW_LIBRARY}
								)

#set_target_properties(fxview PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
