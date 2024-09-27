get_filename_component(WWA_OTEL_SYSLOG_EXPORTER_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)

list(APPEND CMAKE_MODULE_PATH ${WWA_OTEL_SYSLOG_EXPORTER_CMAKE_DIR})

include(CMakeFindDependencyMacro)
find_dependency(opentelemetry-cpp QUIET REQUIRED COMPONENTS logs)
if(opentelemetry-cpp_VERSION VERSION_LESS 1.11.0)
    message(FATAL_ERROR "opentelemetry-cpp version must be at least 1.11.0, ${opentelemetry-cpp_VERSION} found")
endif()

if(NOT TARGET wwa_opentelemetry_exporter_syslog_logs)
    include("${WWA_OTEL_SYSLOG_EXPORTER_CMAKE_DIR}/wwa_opentelemetry_exporter_syslog_logs-target.cmake")
    add_library(wwa::opentelemetry::syslog_log_record_exporter ALIAS wwa_opentelemetry_exporter_syslog_logs)
endif()
