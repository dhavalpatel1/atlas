/**
 * @file jobs_scheduler.h
 * @brief Job scheduling and execution coordination system.
 *
 * This file provides the interface for scheduling and executing job dependency graphs.
 * The scheduler manages the execution of job graphs by coordinating worker threads,
 * respecting task dependencies, and providing mechanisms for waiting on completion.
 * It handles the parallel execution of independent tasks while ensuring dependency
 * constraints are satisfied.
 */
#pragma once

#include "jobs_graph.h"

/**
 * @brief Unique identifier for scheduled job graph executions.
 *
 * Each execution of a job graph is assigned a unique JobId that can be used
 * to track execution status, wait for completion, or coordinate with other
 * parts of the system. JobIds remain valid until the execution completes.
 */
typedef u64 JobId;

/**
 * @brief Schedule a job graph for execution.
 *
 * Submits a job graph to the scheduler for parallel execution. The scheduler
 * will coordinate worker threads to execute tasks in dependency order, ensuring
 * that parent tasks complete before their children begin. Tasks with no
 * dependencies can execute immediately in parallel.
 *
 * @param graph Job graph to execute (must be validated)
 * @return Unique identifier for this execution instance
 */
JobId jobs_scheduler_run(JobGraph* graph);

/**
 * @brief Check if a scheduled job graph has finished execution.
 *
 * Tests whether all tasks in the specified job graph execution have completed.
 * This is a non-blocking operation that returns immediately with the current
 * status.
 *
 * @param jobId Identifier of the job execution to check
 * @return true if all tasks have completed, false if execution is still in progress
 */
bool jobs_scheduler_is_finished(JobId jobId);

/**
 * @brief Wait for a scheduled job graph to complete execution.
 *
 * Blocks the calling thread until all tasks in the specified job graph
 * execution have completed. This is a passive wait that does not contribute
 * to task execution.
 *
 * @param jobId Identifier of the job execution to wait for
 */
void jobs_scheduler_wait(JobId jobId);

/**
 * @brief Wait for job completion while helping with task execution.
 *
 * Blocks the calling thread until the specified job graph execution completes,
 * but actively participates in task execution while waiting. The calling thread
 * will help execute ready tasks from the job graph, potentially reducing
 * overall execution time.
 *
 * @param jobId Identifier of the job execution to wait for
 */
void jobs_scheduler_wait_help(JobId jobId);