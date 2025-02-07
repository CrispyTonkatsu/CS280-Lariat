cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(na-engine LANGUAGES C CXX VERSION 0.1)

set(CMAKE_C_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_C_IMPLICIT_INCLUDE_DIRECTORIES})
set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})

# Compile Options
add_compile_options(-Wall -Werror -Wextra -std=c++17 -pedantic -Wconversion -O2 -Wno-unused-result)
add_compile_options(-fdiagnostics-color=always)

# files to compile
add_executable(driver_c ./src/driver.cpp)
add_executable(driver_custom ./src/custom.cpp)
