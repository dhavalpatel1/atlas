/**
 * @file thread.c
 * @brief Cross-platform threading implementation
 *
 * This file implements thread management, synchronization primitives, and
 * atomic operations. It provides a unified threading API that works across
 * different platforms through platform abstraction layers, including thread
 * creation, mutexes, condition variables, and atomic operations.
 */

#include "core_alloc.h"
#include "core_annotation.h"
#include "core_init.h"
#include "core_memory.h"
#include "core_thread.h"
#include "core_time.h"
#include "init_internal.h"
#include "thread_internal.h"

/**
 * @brief Data structure passed to new threads
 *
 * Contains all the information needed to properly initialize and run
 * a new thread, including the user routine and thread identification.
 */
typedef struct {
    String threadName;        /**< Name of the thread for debugging */
    ThreadRoutine userRoutine; /**< User function to execute */
    void* userData;           /**< User data to pass to the routine */
} ThreadRunData;

/**
 * @brief Internal thread entry point that handles initialization and cleanup
 *
 * This function serves as the entry point for all new threads. It performs
 * necessary initialization (core subsystems, thread naming), runs the user's
 * routine, and handles cleanup when the thread exits.
 *
 * @param data Pointer to ThreadRunData containing thread information
 * @return Platform-specific thread return value (typically null)
 */
static thread_pal_rettype thread_runner(void* data) {
    ThreadRunData* runData = (ThreadRunData*)data;

    // Initialize engine subsystems for this thread
    core_init();

    // Set up thread-local identification and naming
    g_thread_name = runData->threadName;
    thread_pal_set_name(g_thread_name);

    // Execute the user's thread routine
    runData->userRoutine(runData->userData);

    // Clean up engine resources
    core_teardown();

    // Free the thread data allocation
    string_free(g_alloc_heap, runData->threadName);
    alloc_free_t(g_alloc_heap, runData);

    return null;
}

i64 g_thread_pid;
i64 g_thread_main_tid;

THREAD_LOCAL i64 g_thread_tid;
THREAD_LOCAL String g_thread_name;

u16 g_thread_core_count;

void thread_init() {
    g_thread_pid = thread_pal_pid();
    g_thread_main_tid = thread_pal_tid();
    g_thread_name = string_lit("atlas_main");
    g_thread_core_count = thread_pal_core_count();

    thread_pal_set_name(g_thread_name);
}

void thread_init_thread() {
    g_thread_tid = thread_pal_tid();
}

FORCE_INLINE i64 thread_atomic_load_i64(i64 *ptr) {
    return thread_pal_atomic_load_i64(ptr);
}

FORCE_INLINE void thread_atomic_store_i64(i64* ptr, i64 value) {
    thread_pal_atomic_store_i64(ptr, value);
}

FORCE_INLINE i64 thread_atomic_exchange_i64(i64 *ptr, i64 value) {
    return thread_pal_atomic_exchange_i64(ptr, value);
}

FORCE_INLINE bool thread_atomic_compare_exchange_i64(i64 *ptr, i64 *expected, i64 value) {
    return thread_pal_atomic_compare_exchange_i64(ptr, expected, value);
}

FORCE_INLINE i64 thread_atomic_add_i64(i64 *ptr, i64 value) {
    return thread_pal_atomic_add_i64(ptr, value);
}

FORCE_INLINE i64 thread_atomic_sub_i64(i64 *ptr, i64 value) {
    return thread_pal_atomic_sub_i64(ptr, value);
}

ThreadHandle thread_start(ThreadRoutine routine, void* data, String threadName) {
    ThreadRunData* threadRunData = alloc_alloc_t(g_alloc_heap, ThreadRunData);
    threadRunData->threadName = string_dup(g_alloc_heap, threadName);
    threadRunData->userRoutine = routine;
    threadRunData->userData = data;

    return thread_pal_start(thread_runner, threadRunData);
}

void thread_join(ThreadHandle thread) {
    thread_pal_join(thread);
}

void thread_yield() {
    thread_pal_yield();
}

void thread_sleep(const TimeDuration duration) {
    thread_pal_sleep(duration);
}

ThreadMutex thread_mutex_create(Allocator *allocator) {
    return thread_pal_mutex_create(allocator);
}

void thread_mutex_destroy(ThreadMutex mutex) {
    thread_pal_mutex_destroy(mutex);
}

void thread_mutex_lock(ThreadMutex mutex) {
    thread_pal_mutex_lock(mutex);
}

bool thread_mutex_trylock(ThreadMutex mutex) {
    return thread_pal_mutex_trylock(mutex);
}

void thread_mutex_unlock(ThreadMutex mutex) {
    thread_pal_mutex_unlock(mutex);
}

ThreadCondition thread_cond_create(Allocator *allocator) {
    return thread_pal_cond_create(allocator);
}

void thread_cond_destroy(ThreadCondition cond) {
    thread_pal_cond_destroy(cond);
}

void thread_cond_wait(ThreadCondition cond, ThreadMutex mutex) {
    thread_pal_cond_wait(cond, mutex);
}

void thread_cond_signal(ThreadCondition cond) {
    thread_pal_cond_signal(cond);
}

void thread_cond_broadcast(ThreadCondition cond) {
    thread_pal_cond_broadcast(cond);
}