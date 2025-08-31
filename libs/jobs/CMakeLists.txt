message(STATUS "Configuring Jobs Library")

add_library(atlas_jobs STATIC
    src/dot.c
    src/graph.c
    src/init.c
    src/job.c
    src/executor.c
    src/scheduler.c
    src/work_queue.c
)

target_include_directories(atlas_jobs PUBLIC include)
target_link_libraries(atlas_jobs PUBLIC atlas_core)

add_executable(atlas_jobs_test
    test/main.c
    test/test_dot.c
    test/test_executor.c
    test/test_graph.c
    test/test_scheduler.c
)

target_link_libraries(atlas_jobs_test PRIVATE atlas_anvil)
target_link_libraries(atlas_jobs_test PRIVATE atlas_cli)
target_link_libraries(atlas_jobs_test PRIVATE atlas_jobs)
target_link_libraries(atlas_jobs_test PRIVATE atlas_log)

configure_test(atlas_jobs_test jobs)
