/**
 * @file jobs_init.h
 * @brief Initialization and teardown functions for the job system.
 *
 * This file provides the essential functions for initializing and shutting down
 * the job execution system. These functions manage the lifecycle of worker threads,
 * internal data structures, and system resources required for job scheduling
 * and execution.
 */
#pragma once

/**
 * @brief Initialize the job execution system.
 *
 * Sets up the job system infrastructure including worker threads, scheduling
 * queues, and synchronization primitives. This function must be called before
 * any other job system functions can be used. The number of worker threads
 * is typically determined automatically based on system capabilities.
 */
void jobs_init();

/**
 * @brief Shutdown and cleanup the job execution system.
 *
 * Gracefully shuts down all worker threads, waits for pending jobs to complete,
 * and frees all resources allocated by the job system. After calling this
 * function, no other job system functions should be used until jobs_init()
 * is called again.
 */
void jobs_teardown();