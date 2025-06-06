set(sources
    log_record_exporter.cpp
    log_record_exporter_factory.cpp
    default_syslog_implementation_p.cpp
    syslog_interface.cpp
    recordable.cpp
)

set(headers
    ${CMAKE_SOURCE_DIR}/include/opentelemetry/exporters/wwa/syslog/export.h
    ${CMAKE_SOURCE_DIR}/include/opentelemetry/exporters/wwa/syslog/log_record_exporter_factory.h
    ${CMAKE_SOURCE_DIR}/include/opentelemetry/exporters/wwa/syslog/syslog_interface.h
)

add_library(${PROJECT_NAME} ${sources})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_link_libraries(${PROJECT_NAME} PUBLIC opentelemetry-cpp::logs)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
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

if(NOT BUILD_SHARED_LIBS)
    target_compile_definitions(${PROJECT_NAME} PUBLIC OPENTELEMETRY_EXPORTER_SYSLOG_LOGS_STATIC_DEFINE)
endif()

if(ENABLE_MAINTAINER_MODE)
    target_compile_options(${PROJECT_NAME} PRIVATE ${CMAKE_CXX_FLAGS_MM})
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
        PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/opentelemetry/exporters/wwa/syslog"
    )

    install(
        EXPORT "${PROJECT_NAME}-target"
        FILE "${PROJECT_NAME}-target.cmake"
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

    if(BUILD_SHARED_LIBS)
        set(REQUIRES_SUFFIX ".private")
    endif()
    configure_file("${CMAKE_SOURCE_DIR}/cmake/pkg-config.pc.in" "${PROJECT_NAME}.pc" @ONLY)
    install(
        FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig"
    )
endif()
