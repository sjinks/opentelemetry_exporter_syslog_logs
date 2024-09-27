cmake_minimum_required(VERSION 3.25)

project(ci VERSION 1.0.0 LANGUAGES CXX)
set(CMAKE_VERBOSE_MAKEFILE ON)

find_package(wwa_opentelemetry_exporter_syslog_logs CONFIG REQUIRED)

add_executable(ci main.cpp)
target_link_libraries(ci PRIVATE wwa::opentelemetry::syslog_log_record_exporter)
