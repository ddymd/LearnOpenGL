cmake_minimum_required(VERSION 3.20)
if(POLICY CMP0074)
    cmake_policy(SET CMP0074 NEW)
endif()

project(LearnOpenGL VERSION 0.0.1)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

if(APPLE)
    add_definitions(-DMacOS)
endif()


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
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty)

set(3RD_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/glad/glad.c
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/glad/glad.h)

# [4]
# add_executable(HelloWindow ${CMAKE_CURRENT_SOURCE_DIR}/HelloWindow.cpp ${3RD_SRC})
# target_link_libraries(HelloWindow Threads::Threads glfw ${OPENGL_LIBRARIES} spdlog::spdlog)

# [5]
add_executable(HelloTriangle ${CMAKE_CURRENT_SOURCE_DIR}/HelloTriangle.cpp ${3RD_SRC})
target_link_libraries(HelloTriangle Threads::Threads glfw ${OPENGL_LIBRARIES} spdlog::spdlog)
