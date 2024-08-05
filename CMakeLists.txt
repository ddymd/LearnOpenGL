# source: https://github.com/JoeyDeVries/learnopengl
cmake_minimum_required(VERSION 3.20)
if(POLICY CMP0074)
    cmake_policy(SET CMP0074 NEW)
endif()

project(LearnOpenGL VERSION 0.0.1)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(TOP_SRC_DIR "${CMAKE_SOURCE_DIR}/")

configure_file(config.h.in config.h)
include_directories(${PROJECT_BINARY_DIR})

# OPENGL_LIBRARIES
find_package(OpenGL REQUIRED)
message(STATUS "OPENGL_LIBRARIES:${OPENGL_LIBRARIES}")
# Threads::Threads
find_package(Threads REQUIRED)

# glfw
# find_package(glfw3 REQUIRED)
add_subdirectory(3rdparty/glfw)

# spdlog
add_subdirectory(3rdparty/spdlog)

# glm
add_subdirectory(3rdparty/glm)

set(3RD_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/glad/glad.c
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/glad/glad.h
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/stb_image.h
)

set(SHADER_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/Shaders/shader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Shaders/shader.h
)

set(CAMERA_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/Camera/camera.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Camera/camera.h
)

add_library(commonlib ${3RD_SRC} ${SHADER_SRC} ${CAMERA_SRC})
target_include_directories(commonlib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/3rdparty>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Shaders>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Camera>
)
target_link_libraries(commonlib PUBLIC spdlog::spdlog glfw glm)

# add_subdirectory(HelloTriangle)
# add_subdirectory(Shaders)
# add_subdirectory(Textures)
# add_subdirectory(Transformations)
# add_subdirectory(CoordinateSystems)
# add_subdirectory(Camera)

add_subdirectory(Lighting/Colors)
add_subdirectory(Lighting/Materials)
