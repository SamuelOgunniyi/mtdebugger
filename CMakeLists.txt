cmake_minimum_required(VERSION 3.16)
project(ucdbg LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Create header-only library target
add_library(ucdbg INTERFACE)

# 2. Include directories
target_include_directories(ucdbg
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(ucdbg INTERFACE cxx_std_20)

target_compile_options(ucdbg INTERFACE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
)

# 3. Add tests or executables
add_executable(test_basic tests/test_basic.cpp)
target_link_libraries(test_basic PRIVATE ucdbg)

