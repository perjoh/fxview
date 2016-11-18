cmake_minimum_required(VERSION 2.6)
project(effects_viewer)

find_package(OpenGL)

set(SDL2_INCLUDE_DIR /usr/include/SDL2)
set(SDL2_LIBRARY /usr/lib/x86_64-linux-gnu/libSDL2.so) 

add_compile_options(-std=c++0x)

include_directories(${SDL2_INCLUDE_DIR} ${OPENGL_INCLUDE_DIRS})

set(SOURCE_FILES src/main.cpp src/graphics/window.cpp)

add_executable(fxview ${SOURCE_FILES})

target_link_libraries(fxview ${SDL2_LIBRARY} ${OPENGL_LIBRARIES})
