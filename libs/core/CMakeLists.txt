message(STATUS "Configuring Core Library")

add_library(atlas_core STATIC
    src/alloc.c
    src/alloc_bump.c
    src/alloc_heap.c
    src/alloc_page_pal.c
    src/alloc_scratch.c
    src/ascii.c
    src/base64.c
    src/bits.c
    src/bitset.c
    src/compare.c
    src/diag_pal.c
    src/diag.c
    src/dynarray.c
    src/dynbitset.c
    src/dynstring.c
    src/env_pal.c
    src/file.c
    src/file_pal.c
    src/format.c
    src/init.c
    src/math.c
    src/memory.c
    src/path_pal.c
    src/path.c
    src/rng.c
    src/shuffle.c
    src/signal.c
    src/signal_pal.c
    src/sort_bubblesort.c
    src/sort_quicksort.c
    src/string.c
    src/time_pal.c
    src/time.c
    src/thread.c
    src/thread_pal.c
    src/tty.c
    src/tty_pal.c
    src/utf8.c
    src/winutils.c
)

target_include_directories(atlas_core PUBLIC include)

if(${ATLAS_PLATFORM} STREQUAL "linux")
    target_link_libraries(atlas_core PRIVATE m)
endif()

add_executable(atlas_core_test
    test/main.c
    test/test_alloc_bump.c
    test/test_alloc_page.c
    test/test_alloc_scratch.c
    test/test_ascii.c
    test/test_base64.c
    test/test_bits.c
    test/test_bitset.c
    test/test_compare.c
    test/test_dynarray.c
    test/test_dynbitset.c
    test/test_dynstring.c
    test/test_env.c
    test/test_file.c
    test/test_float.c
    test/test_format.c
    test/test_macro.c
    test/test_math.c
    test/test_memory.c
    test/test_path.c
    test/test_rng.c
    test/test_shuffle.c
    test/test_sort.c
    test/test_string.c
    test/test_thread.c
    test/test_time.c
    test/test_utf8.c
    test/test_winutils.c
)

target_link_libraries(atlas_core_test PRIVATE atlas_anvil)
target_link_libraries(atlas_core_test PRIVATE atlas_cli)
target_link_libraries(atlas_core_test PRIVATE atlas_core)
target_link_libraries(atlas_core_test PRIVATE atlas_log)

configure_test(atlas_core_test core)
