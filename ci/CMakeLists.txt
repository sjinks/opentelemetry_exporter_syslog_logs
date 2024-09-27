cmake_minimum_required(VERSION 3.25)

project(ci VERSION 1.0.0.1 LANGUAGES CXX)
set(CMAKE_VERBOSE_MAKEFILE ON)

include(FindPkgConfig)

find_package(wwa_opentelemetry_exporter_syslog_logs CONFIG REQUIRED)
pkg_check_modules(wwa_opentelemetry_exporter_syslog_logs REQUIRED IMPORTED_TARGET wwa_opentelemetry_exporter_syslog_logs)

add_executable(ci-cmake main.cpp)
target_link_libraries(ci-cmake PRIVATE wwa::opentelemetry::syslog_log_record_exporter)

add_executable(ci-pkgconfig main.cpp)
target_link_libraries(ci-pkgconfig PRIVATE PkgConfig::wwa_opentelemetry_exporter_syslog_logs)
