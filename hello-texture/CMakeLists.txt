cmake_minimum_required(VERSION 3.10)

set( CMAKE_C_COMPILER "gcc" )
set( CMAKE_CXX_COMPILER "g++" )

project(HelloTexture)

find_package(fmt CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Stb REQUIRED)

add_executable(hello-texture main.cpp ShaderProgram.cpp Texture2D.cpp)

target_include_directories(main PRIVATE ${Stb_INCLUDE_DIR})
target_link_libraries(hello-texture PRIVATE fmt::fmt glfw glad::glad glm::glm)
