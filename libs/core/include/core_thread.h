#pragma once

#include "core_alloc.h"
#include "core_annotation.h"
#include "core_string.h"
#include "core_time.h"
#include "core_types.h"

extern i64 g_thread_pid;

extern i64 g_thread_main_tid;

extern THREAD_LOCAL i64 g_thread_tid;

extern THREAD_LOCAL String g_thread_name;

extern u16 g_thread_core_count;

typedef void (*ThreadRoutine)(void* data);

typedef uptr ThreadHandle;

typedef uptr ThreadMutex;

typedef uptr ThreadCondition;

i64 thread_atomic_load_i64(i64* ptr);

void thread_atomic_store_i64(i64* ptr, i64 value);

i64 thread_atomic_exchange_i64(i64* ptr, i64 value);

bool thread_atomic_compare_exchange_i64(i64* ptr, i64* expected, i64 value);

i64 thread_atomic_add_i64(i64* ptr, i64 value);

i64 thread_atomic_sub_i64(i64* ptr, i64 value);

ThreadHandle thread_start(ThreadRoutine routine, void* data, String threadName);

void thread_join(ThreadHandle handle);

void thread_yield();

void thread_sleep(TimeDuration duration);

ThreadMutex thread_mutex_create(Allocator* allocator);

void thread_mutex_destroy(ThreadMutex mutex);

void thread_mutex_lock(ThreadMutex mutex);

bool thread_mutex_trylock(ThreadMutex mutex);

void thread_mutex_unlock(ThreadMutex mutex);

ThreadCondition thread_cond_create(Allocator* allocator);

void thread_cond_destroy(ThreadCondition cond);

void thread_cond_wait(ThreadCondition cond, ThreadMutex mutex);

void thread_cond_signal(ThreadCondition cond);

void thread_cond_broadcast(ThreadCondition cond);