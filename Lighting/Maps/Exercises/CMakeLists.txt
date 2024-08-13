cmake_minimum_required(VERSION 3.20)
if(POLICY CMP0074)
    cmake_policy(SET CMP0074 NEW)
endif()

project(LightingMapsExercises VERSION 0.0.1)

add_executable(${PROJECT_NAME}_1 ${CMAKE_CURRENT_SOURCE_DIR}/1.cpp)
target_link_libraries(${PROJECT_NAME}_1 Threads::Threads ${OPENGL_LIBRARIES} commonlib)

add_executable(${PROJECT_NAME}_2 ${CMAKE_CURRENT_SOURCE_DIR}/2.cpp)
target_link_libraries(${PROJECT_NAME}_2 Threads::Threads ${OPENGL_LIBRARIES} commonlib)

add_executable(${PROJECT_NAME}_3 ${CMAKE_CURRENT_SOURCE_DIR}/3.cpp)
target_link_libraries(${PROJECT_NAME}_3 Threads::Threads ${OPENGL_LIBRARIES} commonlib)
