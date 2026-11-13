cmake_minimum_required(VERSION 3.20)
project(hw2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(ENABLE_ASAN "Enable AddressSanitizer for correctness tests" OFF)
if(ENABLE_ASAN AND NOT MSVC)
    message(FATAL_ERROR "ENABLE_ASAN is currently configured for MSVC only")
endif()
if(ENABLE_ASAN AND MSVC)
    # MSVC runtime checks (/RTC1) cannot be combined with AddressSanitizer
    string(REPLACE "/RTC1" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
endif()

add_executable(hw2_demo hw2/hw2.cpp hw2/dense_matrix.cpp)
add_executable(hw2_tests tests/test_dense.cpp hw2/dense_matrix.cpp)
add_executable(hw2_bench bench/bench_dense.cpp hw2/dense_matrix.cpp)

if(ENABLE_ASAN AND MSVC)
    target_compile_options(hw2_tests PRIVATE /fsanitize=address)
endif()

enable_testing()
add_test(NAME dense_correctness COMMAND hw2_tests)
