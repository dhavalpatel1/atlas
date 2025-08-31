message(STATUS "Configuring Log Library")

add_library(atlas_log STATIC
    src/init.c
    src/logger.c
    src/sink_json.c
    src/sink_pretty.c
)

target_include_directories(atlas_log PUBLIC include)
target_link_libraries(atlas_log PUBLIC atlas_core)
target_link_libraries(atlas_log PUBLIC atlas_json)

add_executable(atlas_log_test
    test/main.c
    test/test_logger.c
    test/test_sink_json.c
    test/test_sink_pretty.c
)

target_link_libraries(atlas_log_test PRIVATE atlas_anvil)
target_link_libraries(atlas_log_test PRIVATE atlas_cli)
target_link_libraries(atlas_log_test PRIVATE atlas_log)

configure_test(atlas_log_test log)
