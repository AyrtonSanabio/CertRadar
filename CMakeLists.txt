cmake_minimum_required(VERSION 3.24)

project(
    CertRadar
    VERSION 0.1.0
    DESCRIPTION "Assistente local de diagnostico para suporte a certificados digitais"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

FetchContent_Declare(
    doctest
    GIT_REPOSITORY https://github.com/doctest/doctest.git
    GIT_TAG d44d4f6e66232d716af82f00a063759e9d0e50d6
    GIT_SHALLOW FALSE
)
set(DOCTEST_WITH_MAIN_IN_STATIC_LIB OFF CACHE BOOL "" FORCE)
set(DOCTEST_NO_INSTALL ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(doctest)

function(certradar_enable_strict_warnings target_name)
    if(MSVC)
        target_compile_options(
            ${target_name}
            PRIVATE /W4 /WX /permissive- /sdl /guard:cf /utf-8
        )
    else()
        target_compile_options(
            ${target_name}
            PRIVATE -Wall -Wextra -Wpedantic -Werror
        )
    endif()
endfunction()

add_library(certradar_core STATIC
    src/product_info.cpp
)
target_include_directories(certradar_core PUBLIC "${PROJECT_SOURCE_DIR}/include")
certradar_enable_strict_warnings(certradar_core)

enable_testing()

add_executable(certradar_tests
    tests/test_main.cpp
    tests/product_info_tests.cpp
)
target_link_libraries(certradar_tests PRIVATE certradar_core doctest::doctest)
certradar_enable_strict_warnings(certradar_tests)

include("${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake")
doctest_discover_tests(certradar_tests)
