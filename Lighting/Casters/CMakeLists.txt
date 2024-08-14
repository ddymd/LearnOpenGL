cmake_minimum_required(VERSION 3.20)
if(POLICY CMP0074)
    cmake_policy(SET CMP0074 NEW)
endif()

project(LightingCasters VERSION 0.0.1)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(${PROJECT_NAME}Directional "${CMAKE_CURRENT_SOURCE_DIR}/CastersDirectional.cpp")
target_link_libraries(${PROJECT_NAME}Directional Threads::Threads ${OPENGL_LIBRARIES} commonlib)

add_executable(${PROJECT_NAME}Spotlight "${CMAKE_CURRENT_SOURCE_DIR}/CastersSpotlight.cpp")
target_link_libraries(${PROJECT_NAME}Spotlight Threads::Threads ${OPENGL_LIBRARIES} commonlib)

# add_subdirectory(Exercises)
