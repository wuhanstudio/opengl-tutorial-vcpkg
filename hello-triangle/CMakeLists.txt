cmake_minimum_required(VERSION 3.25)

set( CMAKE_C_COMPILER "gcc" )
set( CMAKE_CXX_COMPILER "g++" )

project(HelloTriangle)

find_package(fmt CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

add_executable(hello-triangle main.cpp)

target_link_libraries(hello-triangle PRIVATE fmt::fmt glfw glad::glad)
