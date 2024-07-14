cmake_minimum_required(VERSION 3.20)
if(POLICY CMP0074)
    cmake_policy(SET CMP0074 NEW)
endif()

project(HelloTriangle VERSION 0.0.1)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

if(APPLE)
    add_definitions(-DMacOS)
endif()

add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/HelloTriangle.cpp ${3RD_SRC})
target_link_libraries(${PROJECT_NAME}  Threads::Threads glfw ${OPENGL_LIBRARIES} spdlog::spdlog)

add_subdirectory(Exercies)
