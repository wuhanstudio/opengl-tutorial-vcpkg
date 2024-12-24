cmake_minimum_required(VERSION 3.25)

set( CMAKE_C_COMPILER "gcc" )
set( CMAKE_CXX_COMPILER "g++" )

project(HelloIMGUI)

find_package(fmt CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Stb REQUIRED)
find_package(imgui REQUIRED)

add_executable(hello-imgui main.cpp ShaderProgram.cpp Texture2D.cpp Camera.cpp Mesh.cpp Skybox.cpp imgui_impl_glfw.cpp imgui_impl_opengl3.cpp)

target_include_directories(hello-imgui PRIVATE ${Stb_INCLUDE_DIR})
target_link_libraries(hello-imgui PRIVATE fmt::fmt glfw glad::glad glm::glm imgui::imgui)
