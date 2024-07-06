cmake_minimum_required(VERSION 3.10)
project(WaveFunctionCollapseExample)

add_executable(
    ${PROJECT_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Pipes.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Sudoku.cpp
)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../WFC
)
