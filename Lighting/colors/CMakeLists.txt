cmake_minimum_required(VERSION 3.20)
if(POLICY CMP0074)
    cmake_policy(SET CMP0074 NEW)
endif()

project(LightingColors VERSION 0.0.1)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/ColorsDemo.cpp ${3RD_SRC} ${SHADER_SRC} ${CAMERA_SRC})
target_link_libraries(${PROJECT_NAME} Threads::Threads glfw ${OPENGL_LIBRARIES} spdlog::spdlog glm)

# add_subdirectory(Exercises)
