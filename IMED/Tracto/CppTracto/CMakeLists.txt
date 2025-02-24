cmake_minimum_required(VERSION 3.28)
project(Tractographie)

set(CMAKE_CXX_STANDARD 17)

# ---- OpenGL ----
find_package(OpenGL REQUIRED)

# ---- GLFW ----
add_subdirectory(thirdparty/glfw)

# ---- GLM ----
add_subdirectory(thirdparty/glm)

# ---- GLAD ----
add_library(glad "thirdparty/glad/src/glad.c")
target_include_directories(glad PUBLIC thirdparty/glad/include)

add_executable(${PROJECT_NAME} main.cpp
        renderer.cpp
        TensorField.h
        filamentMaker.h)

target_link_libraries(
        ${PROJECT_NAME}
        glfw
        glm::glm
        glad
        OpenGL::GL
)

