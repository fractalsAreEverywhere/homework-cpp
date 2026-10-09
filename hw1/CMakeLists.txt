# Две цели (target): тесты корректности и тесты производительности.

add_executable(hw1_tests tests/tests.cpp)
target_include_directories(hw1_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(hw1_bench bench/bench.cpp)
target_include_directories(hw1_bench PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Санитайзер адресов только для тестов. Бенчмарк собирается без него.
if(MSVC)
    target_compile_options(hw1_tests PRIVATE /fsanitize=address)
else()
    # Для g++/clang (на случай сборки не в Visual Studio).
    target_compile_options(hw1_tests PRIVATE -fsanitize=address)
    target_link_options(hw1_tests PRIVATE -fsanitize=address)
endif()
