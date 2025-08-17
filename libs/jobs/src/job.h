/**
 * @file job.h
 * @brief Internal job representation and management
 *
 * This header defines the internal job structure and related types for the
 * jobs system. Jobs represent units of work that can be executed with
 * dependencies on other jobs.
 */

#pragma once

#include "jobs_graph.h"

/** @brief Unique identifier for a job */
typedef u64 JobId;

/**
 * @brief Task-specific data for a job
 *
 * Contains runtime information for individual tasks within a job,
 * including dependency tracking.
 */
typedef struct {
    /** @brief Number of dependencies remaining for this task */
    i64 dependancies;
} JobTaskData;

/**
 * @brief Internal job structure
 *
 * Represents a job with its associated graph, dependencies, and task data.
 * Jobs are created from job graphs and contain runtime state for execution.
 */
typedef struct {
    /** @brief Unique identifier for this job */
    JobId id;
    /** @brief The job graph this job belongs to */
    const JobGraph* graph;
    /** @brief Number of dependencies remaining for the job */
    i64 dependancies;
    /** @brief Variable-length array of task-specific data */
    JobTaskData taskData[];
} Job;

/**
 * @brief Create a new job instance
 * @param alloc Allocator to use for memory management
 * @param id Unique identifier for the job
 * @param graph The job graph this job belongs to
 * @return Pointer to the newly created job, or null on failure
 */
Job* job_create(Allocator* alloc, const JobId id, const JobGraph*);

/**
 * @brief Destroy a job and free its resources
 * @param alloc Allocator used to create the job
 * @param job The job to destroy
 */
void job_destroy(Allocator*, Job*);