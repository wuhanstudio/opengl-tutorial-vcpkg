cmake_minimum_required(VERSION 3.10)

set( CMAKE_C_COMPILER "gcc" )
set( CMAKE_CXX_COMPILER "g++" )

project(HelloOpenGL)

find_package(fmt CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

add_executable(hello-opengl main.cpp)

target_link_libraries(hello-opengl PRIVATE fmt::fmt glfw glad::glad)
