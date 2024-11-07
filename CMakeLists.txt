cmake_minimum_required(VERSION 3.25)

set(EXPORT_COMPILE_COMMANDS ON)

project(
    wwa_opentelemetry_exporter_syslog_logs
    VERSION 1.0.1
    DESCRIPTION "Syslog logs exporter for OpenTelemetry"
    HOMEPAGE_URL "https://github.com/sjinks/opentelemetry_exporter_syslog_logs"
    LANGUAGES CXX
)
set(CMAKE_VERBOSE_MAKEFILE ON)

option(WITH_TESTING "Whether to enable tests" ON)
option(INSTALL_SYSLOG_EXPORTER "Whether to install the syslog exporter" ON)

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
        set(GTEST_VERSION "v1.15.2")
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
