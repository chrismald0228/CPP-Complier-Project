cmake_minimum_required(VERSION 3.10)
project(Compiler)

set(CMAKE_CXX_STANDARD 20)

# Find header files
include_directories(Headers/include)

# Compile both .cpp files into one executable
add_executable(Compiler
    Sources/main.cpp
    Sources/src/LexAnalyzer.cpp
    Sources/src/SyntaxAnalyzer.cpp
)

# Copy .txt files to build folder so your program can find them at runtime
file(COPY lexemes.txt source.txt data.txt DESTINATION ${CMAKE_BINARY_DIR})