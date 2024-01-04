cmake_minimum_required(VERSION 3.20)
if(POLICY CMP0074)
    cmake_policy(SET CMP0074 NEW)
endif()

project(LearnOpenGL VERSION 0.0.1)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# glfw
find_package(glfw3 REQUIRED)
message(STATUS "GLFW: ${glfw3_INCLUDE_DIR} ${GLFW_INCLUDE_DIR} ${GLFW_LIBRARIES} ${glfw3_LIBS}")

find_package(OpenGL REQUIRED)
message(STATUS "OpenGL: ${OPENGL_INCLUDE_DIR}, ${OPENGL_LIBRARIES}")

find_package(Threads REQUIRED)

add_executable(HelloWindow
    ${CMAKE_CURRENT_SOURCE_DIR}/glad.c
    ${CMAKE_CURRENT_SOURCE_DIR}/HelloWindow.cpp)
target_link_libraries(HelloWindow Threads::Threads glfw ${OPENGL_LIBRARIES})
