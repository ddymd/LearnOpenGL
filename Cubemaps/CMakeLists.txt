project(Cubemaps VERSION 0.0.1 LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CUBEMAPS_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/")

configure_file(cubemapsconf.hpp.in cubemapsconf.hpp)
include_directories(${PROJECT_BINARY_DIR})

add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/demo.cpp)
target_link_libraries(${PROJECT_NAME} PUBLIC commonlib)

add_executable(${PROJECT_NAME}Reflection ${CMAKE_CURRENT_SOURCE_DIR}/reflection.cpp)
target_link_libraries(${PROJECT_NAME}Reflection PUBLIC commonlib)
