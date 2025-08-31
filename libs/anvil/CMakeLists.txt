message(STATUS "Configuring Anvil Library")

add_library(atlas_anvil STATIC
    src/def.c
    src/output.c
    src/output_log.c
    src/output_pretty.c
    src/result.c
    src/runner.c
    src/spec.c
)

target_include_directories(atlas_anvil PUBLIC include)
target_link_libraries(atlas_anvil PUBLIC atlas_core)
target_link_libraries(atlas_anvil PUBLIC atlas_jobs)
target_link_libraries(atlas_anvil PUBLIC atlas_log)

add_executable(atlas_anvil_test
    test/main.c
    test/test_dynarray.c
    test/test_fizzbuzz.c
)

target_link_libraries(atlas_anvil_test PRIVATE atlas_anvil)
target_link_libraries(atlas_anvil_test PRIVATE atlas_cli)
target_link_libraries(atlas_anvil_test PRIVATE atlas_log)

configure_test(atlas_anvil_test anvil)
