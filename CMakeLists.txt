cmake_minimum_required(VERSION 3.25)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(
    wwa_opentelemetry_exporter_syslog_logs
    VERSION 1.0.3
    DESCRIPTION "Syslog logs exporter for OpenTelemetry"
    HOMEPAGE_URL "https://github.com/sjinks/opentelemetry_exporter_syslog_logs"
    LANGUAGES CXX
)
set(CMAKE_VERBOSE_MAKEFILE ON)

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(WITH_TESTING "Whether to enable tests" ON)
option(INSTALL_SYSLOG_EXPORTER "Whether to install the syslog exporter" ON)
option(ENABLE_MAINTAINER_MODE "Enable maintainer mode" OFF)

if(CMAKE_CONFIGURATION_TYPES)
    list(APPEND CMAKE_CONFIGURATION_TYPES "Coverage" "ASAN" "LSAN" "TSAN" "UBSAN")
endif()

string(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_LOWER)
string(TOLOWER "${CMAKE_CONFIGURATION_TYPES}" CMAKE_CONFIGURATION_TYPES_LOWER)

string(REGEX MATCH "Clang" CMAKE_COMPILER_IS_CLANG "${CMAKE_CXX_COMPILER_ID}")
string(REGEX MATCH "GNU" CMAKE_COMPILER_IS_GNU "${CMAKE_CXX_COMPILER_ID}")

if(CMAKE_COMPILER_IS_GNU OR CMAKE_COMPILER_IS_CLANG)
    set(CMAKE_CXX_FLAGS_ASAN "-O1 -g -fsanitize=address -fno-omit-frame-pointer -fno-optimize-sibling-calls")
    set(CMAKE_CXX_FLAGS_TSAN "-O1 -g -fsanitize=thread -fno-omit-frame-pointer")
    set(CMAKE_CXX_FLAGS_LSAN "-O1 -g -fsanitize=leak -fno-omit-frame-pointer -fno-optimize-sibling-calls")

    if(CMAKE_COMPILER_IS_GNU)
        set(CMAKE_CXX_FLAGS_COVERAGE "-Og -g --coverage -fprofile-abs-path")
        set(CMAKE_CXX_FLAGS_UBSAN "-O1 -g -fsanitize=undefined -fsanitize=float-divide-by-zero -fno-omit-frame-pointer")
    elseif(CMAKE_COMPILER_IS_CLANG)
        set(CMAKE_CXX_FLAGS_COVERAGE "-O1 -g --coverage")
        set(CMAKE_CXX_FLAGS_UBSAN "-O1 -g -fsanitize=undefined -fsanitize=float-divide-by-zero -fsanitize=integer -fsanitize=implicit-conversion -fsanitize=local-bounds -fsanitize=nullability -fno-omit-frame-pointer")
    endif()
endif()

if(ENABLE_MAINTAINER_MODE)
    if(CMAKE_COMPILER_IS_CLANG)
        set(CMAKE_CXX_FLAGS_MM -Weverything -Werror -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-missing-prototypes -Wno-padded -pedantic -Wno-global-constructors -Wno-exit-time-destructors)
    elseif(CMAKE_COMPILER_IS_GNU)
        set(CMAKE_CXX_FLAGS_MM -Wall -Wextra -Werror -pedantic)
    endif()
endif()

include(FetchContent)

# Logs signal became stable in v1.11.0, see https://github.com/open-telemetry/opentelemetry-cpp/pull/2229, https://github.com/open-telemetry/opentelemetry-cpp/blob/v1.11.0/CHANGELOG.md
find_package(opentelemetry-cpp CONFIG REQUIRED)
if(opentelemetry-cpp_VERSION VERSION_LESS 1.11.0)
    message(FATAL_ERROR "opentelemetry-cpp version must be at least 1.11.0, ${opentelemetry-cpp_VERSION} found")
endif()

add_subdirectory(src)

if(WITH_TESTING)
    include(FindGTest)
    find_package(GTest CONFIG COMPONENTS gtest gmock)
    if(NOT TARGET GTest::gtest OR NOT TARGET GTest::gmock)
        message(STATUS "Google Test not found, fetching it from GitHub")
        # renovate: datasource=github-tags depName=google/googletest
        set(GTEST_VERSION "v1.16.0")
        FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest
            GIT_TAG "${GTEST_VERSION}"
            GIT_SHALLOW ON
        )

        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(googletest)
    endif()

    include(CTest)
    enable_testing()
    add_subdirectory(test)
endif()

find_program(CLANG_FORMAT NAMES clang-format)
find_program(CLANG_TIDY NAMES clang-tidy)

if(CLANG_FORMAT)
    add_custom_target(
        format
        COMMAND ${CLANG_FORMAT} --Wno-error=unknown -i -style=file ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endif()

if(CLANG_TIDY)
    add_custom_target(
        tidy
        COMMAND ${CLANG_TIDY} -p ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endif()
