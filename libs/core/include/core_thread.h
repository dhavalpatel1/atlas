/**
 * @file core_thread.h
 * @brief Threading primitives and utilities for concurrent programming
 *
 * This header provides cross-platform threading support including thread creation,
 * synchronization primitives (mutexes and condition variables), atomic operations,
 * and thread-local storage. It abstracts platform-specific threading APIs to
 * provide a consistent interface across different operating systems.
 */

#pragma once

#include "core_alloc.h"
#include "core_annotation.h"
#include "core_string.h"
#include "core_time.h"
#include "core_types.h"

/** @brief Process ID of the current process */
extern i64 g_thread_pid;

/** @brief Thread ID of the main thread */
extern i64 g_thread_main_tid;

/** @brief Thread-local variable containing the current thread ID */
extern THREAD_LOCAL i64 g_thread_tid;

/** @brief Thread-local variable containing the current thread name */
extern THREAD_LOCAL String g_thread_name;

/** @brief Number of CPU cores available on the system */
extern u16 g_thread_core_count;

/**
 * @brief Function pointer type for thread entry points
 * @param data User data passed to the thread routine
 */
typedef void (*ThreadRoutine)(void* data);

/** @brief Opaque handle type for thread objects */
typedef uptr ThreadHandle;

/** @brief Opaque handle type for mutex objects */
typedef uptr ThreadMutex;

/** @brief Opaque handle type for condition variable objects */
typedef uptr ThreadCondition;

/**
 * @brief Atomically load a 64-bit integer value
 * @param ptr Pointer to the integer to load from
 * @return The loaded value
 */
i64 thread_atomic_load_i64(i64* ptr);

/**
 * @brief Atomically store a 64-bit integer value
 * @param ptr Pointer to the integer to store to
 * @param value Value to store
 */
void thread_atomic_store_i64(i64* ptr, i64 value);

/**
 * @brief Atomically exchange a 64-bit integer value
 * @param ptr Pointer to the integer to exchange
 * @param value New value to store
 * @return The previous value
 */
i64 thread_atomic_exchange_i64(i64* ptr, i64 value);

/**
 * @brief Atomically compare and exchange a 64-bit integer value
 * @param ptr Pointer to the integer to compare and exchange
 * @param expected Pointer to the expected value (updated with actual value on failure)
 * @param value New value to store if comparison succeeds
 * @return true if exchange occurred, false otherwise
 */
bool thread_atomic_compare_exchange_i64(i64* ptr, i64* expected, i64 value);

/**
 * @brief Atomically add to a 64-bit integer value
 * @param ptr Pointer to the integer to add to
 * @param value Value to add
 * @return The previous value before addition
 */
i64 thread_atomic_add_i64(i64* ptr, i64 value);

/**
 * @brief Atomically subtract from a 64-bit integer value
 * @param ptr Pointer to the integer to subtract from
 * @param value Value to subtract
 * @return The previous value before subtraction
 */
i64 thread_atomic_sub_i64(i64* ptr, i64 value);

/**
 * @brief Start a new thread with the specified routine and data
 * @param routine Function to execute in the new thread
 * @param data User data to pass to the thread routine
 * @param threadName Name for the new thread (for debugging)
 * @return Handle to the created thread
 */
ThreadHandle thread_start(ThreadRoutine routine, void* data, String threadName);

/**
 * @brief Wait for a thread to complete execution
 * @param handle Handle to the thread to wait for
 */
void thread_join(ThreadHandle handle);

/**
 * @brief Yield execution to allow other threads to run
 */
void thread_yield();

/**
 * @brief Sleep for the specified duration
 * @param duration Time duration to sleep
 */
void thread_sleep(TimeDuration duration);

/**
 * @brief Create a new mutex for thread synchronization
 * @param allocator Allocator to use for mutex creation
 * @return Handle to the created mutex
 */
ThreadMutex thread_mutex_create(Allocator* allocator);

/**
 * @brief Destroy a mutex and free its resources
 * @param mutex Mutex handle to destroy
 */
void thread_mutex_destroy(ThreadMutex mutex);

/**
 * @brief Lock a mutex, blocking until available
 * @param mutex Mutex handle to lock
 */
void thread_mutex_lock(ThreadMutex mutex);

/**
 * @brief Try to lock a mutex without blocking
 * @param mutex Mutex handle to try locking
 * @return true if lock was acquired, false if already locked
 */
bool thread_mutex_trylock(ThreadMutex mutex);

/**
 * @brief Unlock a previously locked mutex
 * @param mutex Mutex handle to unlock
 */
void thread_mutex_unlock(ThreadMutex mutex);

/**
 * @brief Create a new condition variable for thread coordination
 * @param allocator Allocator to use for condition variable creation
 * @return Handle to the created condition variable
 */
ThreadCondition thread_cond_create(Allocator* allocator);

/**
 * @brief Destroy a condition variable and free its resources
 * @param cond Condition variable handle to destroy
 */
void thread_cond_destroy(ThreadCondition cond);

/**
 * @brief Wait on a condition variable with mutex protection
 * @param cond Condition variable to wait on
 * @param mutex Mutex to release while waiting and reacquire after signaling
 */
void thread_cond_wait(ThreadCondition cond, ThreadMutex mutex);

/**
 * @brief Signal one thread waiting on a condition variable
 * @param cond Condition variable to signal
 */
void thread_cond_signal(ThreadCondition cond);

/**
 * @brief Signal all threads waiting on a condition variable
 * @param cond Condition variable to broadcast to
 */
void thread_cond_broadcast(ThreadCondition cond);