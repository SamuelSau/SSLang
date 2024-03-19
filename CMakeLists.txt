cmake_minimum_required(VERSION 3.10)

# Project Name
project(SSLang)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include Directories
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/src
    )

# Add Executable
add_executable(SSLang src/main.cpp src/lexer/Lexer.cpp)