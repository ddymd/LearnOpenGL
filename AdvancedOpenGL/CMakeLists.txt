project(AdvancedOpenGL VERSION 0.0.1 LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(ADVOGL_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/")

configure_file(advconf.hpp.in advconf.hpp)
include_directories(${PROJECT_BINARY_DIR})

add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/demo.cpp)
target_link_libraries(${PROJECT_NAME} PUBLIC commonlib)

add_executable(${PROJECT_NAME}_stencil ${CMAKE_CURRENT_SOURCE_DIR}/stencil.cpp)
target_link_libraries(${PROJECT_NAME}_stencil PUBLIC commonlib)

add_executable(${PROJECT_NAME}_blending_discard ${CMAKE_CURRENT_SOURCE_DIR}/blending_discard.cpp)
target_link_libraries(${PROJECT_NAME}_blending_discard PUBLIC commonlib)

add_executable(${PROJECT_NAME}_faceculling ${CMAKE_CURRENT_SOURCE_DIR}/faceculling.cpp)
target_link_libraries(${PROJECT_NAME}_faceculling PUBLIC commonlib)
