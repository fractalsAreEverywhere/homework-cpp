cmake_minimum_required(VERSION 3.20)
project(cpp_homework CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    # Исходники в UTF-8, чтобы русские строки в коде не ломались.
    add_compile_options(/utf-8)
    # /RTC1 (проверки в Debug) несовместим с /fsanitize=address, убираем его.
    string(REPLACE "/RTC1" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
endif()

add_subdirectory(hw1)
