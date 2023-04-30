cmake_minimum_required(VERSION 3.20)
project(chess_project VERSION 1.0)

include(CTest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(include)

if (MSVC)
    # warning level 4 and all warnings as errors
    add_compile_options(/W4 /WX)
else()
    # lots of warnings and all warnings as errors
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

add_executable(chess_project
        src/controller/controller.cpp
        src/logic/chessboard.cpp
        src/player/tui.cpp
        src/view/tui.cpp
        src/main.cpp)

add_test(NAME test_1_chess_project COMMAND sh -c "cd ${PROJECT_SOURCE_DIR} && ./test-level.sh 1 ${CMAKE_CURRENT_BINARY_DIR}/chess_project")
add_test(NAME test_2_chess_project COMMAND sh -c "cd ${PROJECT_SOURCE_DIR} && ./test-level.sh 2 ${CMAKE_CURRENT_BINARY_DIR}/chess_project")
add_test(NAME test_3_chess_project COMMAND sh -c "cd ${PROJECT_SOURCE_DIR} && ./test-level.sh 3 ${CMAKE_CURRENT_BINARY_DIR}/chess_project")
add_test(NAME test_4_chess_project COMMAND sh -c "cd ${PROJECT_SOURCE_DIR} && ./test-level.sh 4 ${CMAKE_CURRENT_BINARY_DIR}/chess_project")

