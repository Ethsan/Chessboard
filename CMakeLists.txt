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
	src/player/player_random.cpp
	src/player/player_remote.cpp
        src/view/tui.cpp
	src/board.cpp
        src/main.cpp)

add_test(NAME test_1_chess_project COMMAND sh -c "cd ${PROJECT_SOURCE_DIR} && ./test-level.sh 1 ${CMAKE_CURRENT_BINARY_DIR}/chess_project")
add_test(NAME test_2_chess_project COMMAND sh -c "cd ${PROJECT_SOURCE_DIR} && ./test-level.sh 2 ${CMAKE_CURRENT_BINARY_DIR}/chess_project")
add_test(NAME test_3_chess_project COMMAND sh -c "cd ${PROJECT_SOURCE_DIR} && ./test-level.sh 3 ${CMAKE_CURRENT_BINARY_DIR}/chess_project")
add_test(NAME test_4_chess_project COMMAND sh -c "cd ${PROJECT_SOURCE_DIR} && ./test-level.sh 4 ${CMAKE_CURRENT_BINARY_DIR}/chess_project")

# first we can indicate the documentation build as an option and set it to ON by default
option(BUILD_DOC "Build documentation" ON)

# check if Doxygen is installed
find_package(Doxygen)
if (DOXYGEN_FOUND)
    # set input and output files
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    # request to configure the file
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    message("Doxygen build started")

    # note the option ALL which allows to build the docs together with the application
    add_custom_target( doc_doxygen ALL
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM )
else (DOXYGEN_FOUND)
  message("Doxygen need to be installed to generate the doxygen documentation")
endif (DOXYGEN_FOUND)
