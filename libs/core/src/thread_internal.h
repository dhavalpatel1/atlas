
#pragma once

#include "core_alloc.h"
#include "core_thread.h"
#include "core_time.h"

#define thread_pal_stacksize (usize_mebibyte * 2)

#if defined(ATLAS_LINUX)
#define thread_pal_rettype void*
#elif defined(ATLAS_WIN32)
#define thread_pal_rettype unsigned long
#else
_Static_assert(false, "Unsupported platform")
#endif

i64 thread_pal_pid();

i64 thread_pal_tid();

u16 thread_pal_core_count();

void thread_pal_set_name(String name);

i64 thread_pal_atomic_load_i64(i64* ptr);

void thread_pal_atomic_store_i64(i64* ptr, i64 value);

i64 thread_pal_atomic_exchange_i64(i64* ptr, i64 value);

bool thread_pal_atomic_compare_exchange_i64(i64* ptr, i64* expected, i64 value);

i64 thread_pal_atomic_add_i64(i64* ptr, i64 value);

i64 thread_pal_atomic_sub_i64(i64* ptr, i64 value);

ThreadHandle thread_pal_start(thread_pal_rettype (*entry_point)(void*), void* arg);

void thread_pal_join(ThreadHandle handle);

void thread_pal_yield();

void thread_pal_sleep(TimeDuration duration);

ThreadMutex thread_pal_mutex_create(Allocator* allocator);

void thread_pal_mutex_destroy(ThreadMutex mutex);

void thread_pal_mutex_lock(ThreadMutex mutex);

bool thread_pal_mutex_trylock(ThreadMutex mutex);

void thread_pal_mutex_unlock(ThreadMutex mutex);

ThreadCondition thread_pal_cond_create(Allocator* allocator);

void thread_pal_cond_destroy(ThreadCondition cond);

void thread_pal_cond_wait(ThreadCondition cond, ThreadMutex mutex);

void thread_pal_cond_signal(ThreadCondition cond);

void thread_pal_cond_broadcast(ThreadCondition cond);