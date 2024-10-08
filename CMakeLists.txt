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

# Logs signal became stable in v1.11.0, see https://github.com/open-telemetry/opentelemetry-cpp/pull/2229, https://github.com/open-telemetry/opentelemetry-cpp/blob/v1.11.0/CHANGELOG.md
find_package(opentelemetry-cpp CONFIG REQUIRED)
if(opentelemetry-cpp_VERSION VERSION_LESS 1.11.0)
    message(FATAL_ERROR "opentelemetry-cpp version must be at least 1.11.0, ${opentelemetry-cpp_VERSION} found")
endif()

add_subdirectory(src)

if(WITH_TESTING)
    include(FindGTest)
    find_package(GTest CONFIG COMPONENTS gtest gmock)
    if(TARGET GTest::gtest AND TARGET GTest::gmock)
        include(CTest)
        enable_testing()
        add_subdirectory(test)
    else()
        message(WARNING "GTest not found, tests will not be built")
    endif()
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
