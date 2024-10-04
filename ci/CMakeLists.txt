cmake_minimum_required(VERSION 3.25)

project(ci LANGUAGES CXX)
set(CMAKE_VERBOSE_MAKEFILE ON)

if(DEFINED VCPKG_INSTALLED_DIR)
    file(MAKE_DIRECTORY "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include" "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/debug/include" )
endif()

include(FindPkgConfig)

find_package(wwa_opentelemetry_exporter_syslog_logs CONFIG REQUIRED)
pkg_check_modules(wwa_opentelemetry_exporter_syslog_logs REQUIRED IMPORTED_TARGET wwa_opentelemetry_exporter_syslog_logs)

add_executable(ci-cmake main.cpp)
target_link_libraries(ci-cmake PRIVATE wwa::opentelemetry::syslog_log_record_exporter)

add_executable(ci-pkgconfig main.cpp)
target_link_libraries(ci-pkgconfig PRIVATE PkgConfig::wwa_opentelemetry_exporter_syslog_logs)
