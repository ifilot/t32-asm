cmake_minimum_required(VERSION 3.10)

project(t32)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(t32lib
    src/parser.cpp
    src/assembler.cpp
    src/vm.cpp
)

target_include_directories(t32lib PUBLIC src)

# ---- CLI11 ----
include(FetchContent)

FetchContent_Declare(
    CLI11
    GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
    GIT_TAG v2.3.2
)

FetchContent_MakeAvailable(CLI11)

add_executable(t32
    src/main.cpp
)

target_link_libraries(t32 PRIVATE t32lib CLI11::CLI11)
set_target_properties(t32 PROPERTIES OUTPUT_NAME t32)

enable_testing()

add_library(t32_e2e_test_harness
    tests/e2e_test_harness.cpp
)
target_link_libraries(t32_e2e_test_harness PRIVATE t32lib)
target_include_directories(t32_e2e_test_harness PRIVATE src tests)

add_executable(t32_e2e_hello_test
    tests/e2e_hello_test.cpp
)
target_link_libraries(t32_e2e_hello_test PRIVATE t32_e2e_test_harness)

add_executable(t32_e2e_fibo_test
    tests/e2e_fibo_test.cpp
)
target_link_libraries(t32_e2e_fibo_test PRIVATE t32_e2e_test_harness)

add_executable(t32_e2e_int2dec_test
    tests/e2e_int2dec_test.cpp
)
target_link_libraries(t32_e2e_int2dec_test PRIVATE t32_e2e_test_harness)

add_executable(t32_e2e_int2hex_test
    tests/e2e_int2hex_test.cpp
)
target_link_libraries(t32_e2e_int2hex_test PRIVATE t32_e2e_test_harness)

add_test(NAME e2e_hello COMMAND t32_e2e_hello_test ${CMAKE_SOURCE_DIR}/tests/cases)
add_test(NAME e2e_fibo COMMAND t32_e2e_fibo_test ${CMAKE_SOURCE_DIR}/tests/cases)
add_test(NAME e2e_int2dec COMMAND t32_e2e_int2dec_test ${CMAKE_SOURCE_DIR}/tests/cases)
add_test(NAME e2e_int2hex COMMAND t32_e2e_int2hex_test ${CMAKE_SOURCE_DIR}/tests/cases)
