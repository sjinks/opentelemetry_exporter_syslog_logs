add_executable(test_logger)
target_sources(test_logger PRIVATE test_logger.cpp)

## To test packag installation
# find_package(opentelemetry_exporter_syslog_logs CONFIG REQUIRED)
# target_link_libraries(test_logger PRIVATE GTest::gtest GTest::gmock GTest::gtest_main opentelemetry_exporter_syslog_logs::opentelemetry_exporter_syslog_logs)

## Normal case
target_link_libraries(test_logger PRIVATE GTest::gtest GTest::gmock GTest::gtest_main ${PROJECT_NAME})

set_target_properties(
    test_logger
    PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN ON
)

include(GoogleTest)
gtest_discover_tests(test_logger)
