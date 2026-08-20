cmake_minimum_required(VERSION 3.20)

project(Model-Language-Interpreter-Linux
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(InterpreterLib
    src/Lexeme.cpp
    src/Tables.cpp
    src/Scanner.cpp
    src/Parser.cpp
    src/Executer.cpp
    src/Interpreter.cpp
    src/Terminal.cpp
)

target_include_directories(InterpreterLib PUBLIC include)

if(UNIX)
    target_link_libraries(InterpreterLib
        PUBLIC
            curses
    )
endif()

add_executable(Main
    src/main.cpp
)

target_link_libraries(Main
    PRIVATE
        InterpreterLib
)

enable_testing()

SET(TESTS
    TestWrite
    TestBool
    TestIf
    TestSwitch
    TestWhile
    TestDoWhile
    TestFor
    TestForStep
    TestBreak
    TestContinue
    TestGoto
    TestComment
    TestUnary
)

foreach(TEST ${TESTS})
    add_executable(${TEST} tests/${TEST}.cpp)

    target_link_libraries(${TEST}
        PRIVATE
            InterpreterLib
    )

    add_test(
        NAME ${TEST}
        COMMAND ${TEST}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests/test_cases
    )
endforeach()