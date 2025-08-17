message(STATUS "Configuring CLI Library")

add_library(atlas_cli STATIC
    src/app.c
    src/failure.c
    src/help.c
    src/read.c
    src/parse.c
    src/validate.c
)

target_include_directories(atlas_cli PUBLIC include)
target_link_libraries(atlas_cli PUBLIC atlas_core)

add_executable(atlas_cli_test
    test/main.c
    test/test_app.c
    test/test_failure.c
    test/test_help.c
    test/test_read.c
    test/test_parse.c
    test/test_validate.c
)

target_link_libraries(atlas_cli_test PRIVATE atlas_cli)
target_link_libraries(atlas_cli_test PRIVATE atlas_anvil)

configure_test(atlas_cli_test cli)