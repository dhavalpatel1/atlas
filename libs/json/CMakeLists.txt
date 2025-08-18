message(STATUS "Configuring Json Library")

add_library(atlas_json STATIC
    src/doc.c
    src/eq.c
    src/lex.c
    src/parse.c
    src/write.c
)

target_include_directories(atlas_json PUBLIC include)
target_link_libraries(atlas_json PUBLIC atlas_core)

add_executable(atlas_json_test
    test/main.c
    test/test_doc.c
    test/test_eq.c
    test/test_parse.c
    test/test_write.c
)

target_link_libraries(atlas_json_test PRIVATE atlas_anvil)
target_link_libraries(atlas_json_test PRIVATE atlas_cli)
target_link_libraries(atlas_json_test PRIVATE atlas_jobs)
target_link_libraries(atlas_json_test PRIVATE atlas_json)

configure_test(atlas_json_test json)