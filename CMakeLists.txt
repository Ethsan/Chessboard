cmake_minimum_required(VERSION 3.26)
project(chess_project)

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
        include/controller/controller.hpp
        include/logic/bitboard.hpp
        include/logic/chessboard.hpp
        include/logic/utils.hpp
        include/player/player.hpp
        include/player/tui.hpp
        include/view/multiplexer.hpp
        include/view/noop_view.hpp
        include/view/tui.hpp
        include/view/view.hpp
        include/board.hpp
        src/controller/controller.cpp
        src/logic/chessboard.cpp
        src/player/tui.cpp
        src/view/tui.cpp
        src/main.cpp)
