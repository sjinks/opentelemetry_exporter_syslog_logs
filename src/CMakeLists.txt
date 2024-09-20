include(GenerateExportHeader)

set(sources
    syslog_exporter.cpp
    syslog_exporter_factory.cpp
    default_syslog_implementation_p.cpp
)

set(headers
    ${CMAKE_CURRENT_BINARY_DIR}/opentelemetry_exporter_syslog_logs_export.h
    ${CMAKE_SOURCE_DIR}/include/opentelemetry_exporter_syslog_logs/syslog_exporter_factory.h
    ${CMAKE_SOURCE_DIR}/include/opentelemetry_exporter_syslog_logs/syslog_interface.h
)

add_library(${PROJECT_NAME} ${sources})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_link_libraries(${PROJECT_NAME} PUBLIC opentelemetry-cpp::logs)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}
)

set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
        CXX_EXTENSIONS OFF
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN ON
        POSITION_INDEPENDENT_CODE ON
        PUBLIC_HEADER "${headers}"
        INTERFACE_COMPILE_FEATURES cxx_std_17
)

generate_export_header(${PROJECT_NAME} BASE_NAME ${PROJECT_NAME})
if(NOT BUILD_SHARED_LIBS)
    set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS -DOPENTELEMETRY_EXPORTER_SYSLOG_LOGS_STATIC_DEFINE)
endif()

if(INSTALL_SYSLOG_EXPORTER)
    include(GNUInstallDirs)
    include(CMakePackageConfigHelpers)

    install(
        TARGETS ${PROJECT_NAME}
        EXPORT "${PROJECT_NAME}-target"
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}"
    )

    install(EXPORT "${PROJECT_NAME}-target"
        FILE "${PROJECT_NAME}-target.cmake"
        NAMESPACE "${PROJECT_NAME}::"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
    )

    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
    )

    install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake"
        "${CMAKE_SOURCE_DIR}/cmake/${PROJECT_NAME}-config.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
    )
endif()