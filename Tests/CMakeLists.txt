project(UnitTestRunner VERSION 0.1)

set_property(GLOBAL PROPERTY CTEST_TARGETS_ADDED 1)
include(CTest)


juce_add_console_app(UnitTestRunner PRODUCT_NAME "Unit Test Runner")

target_sources(UnitTestRunner PRIVATE Tests.cpp)

target_compile_definitions(UnitTestRunner PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0)

target_link_libraries(UnitTestRunner PRIVATE
        doctest
        juce_recommended_config_flags
        juce_recommended_lto_flags
        juce_recommended_warning_flags
        juce_core)

add_test(NAME my-tests COMMAND UnitTestRunner)
