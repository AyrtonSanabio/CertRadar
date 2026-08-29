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

FetchContent_Declare(
    nlohmann_json
    URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
    URL_HASH SHA256=42f6e95cad6ec532fd372391373363b62a14af6d771056dbfc86160e6dfff7aa
)
set(JSON_BuildTests OFF CACHE INTERNAL "")
FetchContent_MakeAvailable(nlohmann_json)

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
    src/certificates.cpp
    src/facts.cpp
    src/known_folders.cpp
    src/platform.cpp
    src/product_info.cpp
    src/search.cpp
    src/search_plan.cpp
    src/ui_model.cpp
)
target_include_directories(certradar_core PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_link_libraries(certradar_core PUBLIC nlohmann_json::nlohmann_json)
certradar_enable_strict_warnings(certradar_core)

enable_testing()

add_executable(certradar_tests
    tests/test_main.cpp
    tests/facts_tests.cpp
    tests/certificates_tests.cpp
    tests/known_folders_tests.cpp
    tests/platform_tests.cpp
    tests/product_info_tests.cpp
    tests/search_tests.cpp
    tests/search_plan_tests.cpp
    tests/ui_model_tests.cpp
)
if(WIN32)
    target_link_libraries(certradar_core PRIVATE advapi32 crypt32 ole32 shell32)
endif()
target_link_libraries(certradar_tests PRIVATE certradar_core doctest::doctest)
certradar_enable_strict_warnings(certradar_tests)

include("${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake")
doctest_discover_tests(certradar_tests)

add_executable(certradar WIN32 src/app/main.cpp)
target_link_libraries(certradar PRIVATE certradar_core comctl32)
certradar_enable_strict_warnings(certradar)
