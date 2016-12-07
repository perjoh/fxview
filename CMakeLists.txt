cmake_minimum_required(VERSION 2.6)
project(effects_viewer)

find_package(OpenGL)
find_package(GLEW)

set(SDL2_INCLUDE_DIR /usr/include/SDL2)
set(SDL2_LIBRARY /usr/lib/x86_64-linux-gnu/libSDL2.so) 

add_compile_options(-std=c++11 -Wall)

include_directories(${SDL2_INCLUDE_DIR} 
					${OPENGL_INCLUDE_DIRS} 
					${GLEW_INCLUDE_DIRS}
					)

set(SOURCE_FILES	src/main.cpp 
					src/graphics/window.cpp
					src/graphics/shader.cpp
					src/graphics/mesh.cpp
					)

add_executable(fxview ${SOURCE_FILES})

target_link_libraries(fxview	${SDL2_LIBRARY} 
								${OPENGL_LIBRARIES}
								${GLEW_LIBRARIES}
								)
