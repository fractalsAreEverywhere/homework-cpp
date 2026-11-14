cmake_minimum_required(VERSION 3.20)
project(hw3 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(HW3_ENABLE_ASAN "Enable AddressSanitizer for Debug tests" ON)
if(MSVC)
    add_compile_options(/utf-8)
    if(HW3_ENABLE_ASAN)
        # /RTC1 несовместим с AddressSanitizer
        string(REPLACE "/RTC1" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
    endif()
endif()

# Общие реализации матриц; у демонстрации, тестов и бенчмарка разные main
add_library(hw3_core STATIC src/matrix.cpp src/dense_matrix.cpp src/sparse_matrix.cpp)
target_include_directories(hw3_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(hw3_demo src/main.cpp)
target_link_libraries(hw3_demo PRIVATE hw3_core)

add_executable(hw3_tests tests/test_sparse.cpp)
target_link_libraries(hw3_tests PRIVATE hw3_core)
if(HW3_ENABLE_ASAN)
    if(MSVC)
        target_compile_options(hw3_core PUBLIC $<$<CONFIG:Debug>:/fsanitize=address>)
        target_link_options(hw3_core PUBLIC $<$<CONFIG:Debug>:/INCREMENTAL:NO>)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(hw3_core PUBLIC $<$<CONFIG:Debug>:-fsanitize=address>)
        target_link_options(hw3_core PUBLIC $<$<CONFIG:Debug>:-fsanitize=address>)
    endif()
endif()

# Собираем бенчмарк явно в Release; там флаги санитайзера отсутствуют
add_executable(hw3_bench EXCLUDE_FROM_ALL bench/bench_sparse.cpp)
target_link_libraries(hw3_bench PRIVATE hw3_core)

# При обычном запуске Windows ищет DLL рядом с exe
if(MSVC AND HW3_ENABLE_ASAN)
    get_filename_component(HW3_COMPILER_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)
    file(GLOB HW3_ASAN_DLLS "${HW3_COMPILER_DIR}/clang_rt.asan*_dynamic-*.dll")
    if(NOT HW3_ASAN_DLLS)
        message(FATAL_ERROR "AddressSanitizer DLL not found next to the compiler")
    endif()
    foreach(HW3_TARGET hw3_demo hw3_tests hw3_bench)
        add_custom_command(TARGET ${HW3_TARGET} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${HW3_ASAN_DLLS} "$<TARGET_FILE_DIR:${HW3_TARGET}>"
            VERBATIM
        )
    endforeach()
endif()

enable_testing()
add_test(NAME hw3_sparse_correctness COMMAND hw3_tests)
