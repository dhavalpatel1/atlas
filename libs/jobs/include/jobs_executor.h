/**
 * @file jobs_executor.h
 * @brief Job execution management and worker thread state.
 *
 * This file provides the interface for job execution management, including
 * worker thread identification and state tracking. It defines global variables
 * that track the number of worker threads and provides thread-local storage
 * for worker identification and execution state.
 */
#pragma once

#include "core_annotation.h"
#include "core_types.h"

/**
 * @brief Identifier for job worker threads.
 *
 * A unique identifier assigned to each worker thread in the job system.
 * Worker IDs are used to distinguish between different execution contexts
 * and can be used for load balancing or debugging purposes.
 */
typedef u16 JobWorkerId;

/**
 * @brief Global count of active job worker threads.
 *
 * This variable tracks the total number of worker threads available in the
 * job system. It is set during initialization and determines the level of
 * parallelism available for job execution.
 */
extern u16 g_jobsWorkerCount;

/**
 * @brief Thread-local identifier for the current worker thread.
 *
 * Each worker thread has a unique identifier stored in thread-local storage.
 * This allows worker threads to identify themselves and coordinate their
 * work appropriately within the job system.
 */
extern THREAD_LOCAL JobWorkerId g_jobsWorkerId;

/**
 * @brief Thread-local flag indicating if the current thread is a job worker.
 *
 * This flag is set to true for threads that are part of the job worker pool
 * and false for other threads (such as the main thread). It can be used to
 * determine execution context and enable worker-specific optimizations.
 */
extern THREAD_LOCAL bool g_jobsIsWorker;

/**
 * @brief Thread-local flag indicating if the current worker is actively executing a job.
 *
 * This flag tracks whether the current worker thread is actively executing
 * a job task. It is used for state management and can help with debugging
 * or performance monitoring of the job execution system.
 */
extern THREAD_LOCAL bool g_jobsIsWorking;