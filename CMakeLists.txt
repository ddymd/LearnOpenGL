cmake_minimum_required(VERSION 3.20)
if(POLICY CMP0074)
    cmake_policy(SET CMP0074 NEW)
endif()

project(LearnOpenGL VERSION 0.0.1)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# message("CMAKE_SYSTEM:${CMAKE_SYSTEM}, CMAKE_SYSTEM_NAME:${CMAKE_SYSTEM_NAME}")
# message("CMAKE_SYSTEM_VERSION:${CMAKE_SYSTEM_VERSION}")
# message("CMAKE_SYSTEM_PROCESSOR:${CMAKE_SYSTEM_PROCESSOR}")
# message("CMAKE_HOST_SYSTEM_NAME:${CMAKE_HOST_SYSTEM_NAME}")
if(APPLE)
    add_definitions(-DMacOS)
endif()

# glfw
find_package(glfw3 REQUIRED)
# OPENGL_LIBRARIES
find_package(OpenGL REQUIRED)
# Threads::Threads
find_package(Threads REQUIRED)

# [4]
add_executable(HelloWindow
    ${CMAKE_CURRENT_SOURCE_DIR}/glad.c
    ${CMAKE_CURRENT_SOURCE_DIR}/HelloWindow.cpp)
target_link_libraries(HelloWindow Threads::Threads glfw ${OPENGL_LIBRARIES})

# [5]
add_executable(HelloTriangle
    ${CMAKE_CURRENT_SOURCE_DIR}/glad.c
    ${CMAKE_CURRENT_SOURCE_DIR}/HelloTriangle.cpp)
target_link_libraries(HelloTriangle Threads::Threads glfw ${OPENGL_LIBRARIES})
