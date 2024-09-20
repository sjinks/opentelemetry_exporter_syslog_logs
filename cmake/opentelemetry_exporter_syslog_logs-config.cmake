get_filename_component(OTEL_SYSLOG_EXPORTER_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)

list(APPEND CMAKE_MODULE_PATH ${OTEL_SYSLOG_EXPORTER_CMAKE_DIR})

include(CMakeFindDependencyMacro)
find_dependency(opentelemetry-cpp QUIET REQUIRED COMPONENTS logs)
if(opentelemetry-cpp_VERSION VERSION_LESS 1.11.0)
    message(FATAL_ERROR "opentelemetry-cpp version must be at least 1.11.0, ${opentelemetry-cpp_VERSION} found")
endif()

if(NOT TARGET opentelemetry_exporter_syslog_logs::opentelemetry_exporter_syslog_logs)
    include("${OTEL_SYSLOG_EXPORTER_CMAKE_DIR}/opentelemetry_exporter_syslog_logs-target.cmake")
endif()

set(OTEL_SYSLOG_EXPORTER_LIBRARIES opentelemetry_exporter_syslog_logs::opentelemetry_exporter_syslog_logs)
