add_executable(test_logger)
target_sources(test_logger PRIVATE test_logger.cpp)
target_link_libraries(test_logger PRIVATE GTest::gmock GTest::gtest_main ${PROJECT_NAME})

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
