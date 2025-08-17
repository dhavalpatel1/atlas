/**
 * @file scheduler.c
 * @brief Job scheduling system implementation
 *
 * This file implements the job scheduler which manages the lifecycle of job
 * executions. It handles job ID generation, tracks running jobs, and provides
 * synchronization primitives for waiting on job completion.
 *
 * The scheduler coordinates between the high-level job submission API and
 * the low-level executor that manages worker threads and task execution.
 */

#include "core_diag.h"
#include "core_dynarray.h"
#include "core_thread.h"

#include "executor_internal.h"
#include "init_internal.h"

#include "job.h"

#include "jobs_scheduler.h"

static i64 g_jobIdCounter;                  /**< Global counter for generating unique job IDs */
static ThreadMutex g_jobMutex;              /**< Mutex protecting job scheduler data */
static ThreadCondition g_jobCondition;     /**< Condition variable for job completion notifications */
static DynArray g_runningJobs;              /**< Array of currently running job pointers */

/**
 * @brief Checks if a job is finished (internal helper, assumes mutex is locked)
 *
 * @param job Job ID to check
 * @return true if the job is not found in the running jobs list (finished), false otherwise
 */
static bool jobs_scheduler_is_finished_locked(const JobId job) {
    dynarray_for_t(&g_runningJobs, Job*, jobData, {
        if ((*jobData)->id == job) {
            return false;
        }
    });

    return true;
}

/**
 * @brief Initializes the job scheduler subsystem
 *
 * Sets up synchronization primitives and data structures needed for
 * tracking running jobs and coordinating job completion.
 */
void scheduler_init() {
    g_jobIdCounter = 0;
    g_jobMutex = thread_mutex_create(g_alloc_heap);
    g_jobCondition = thread_cond_create(g_alloc_heap);
    g_runningJobs = dynarray_create_t(g_alloc_heap, Job*, 32);
}

/**
 * @brief Shuts down the job scheduler subsystem
 *
 * Cleans up all synchronization primitives and data structures.
 * Should be called after all jobs have completed.
 */
void scheduler_teardown() {
    thread_mutex_destroy(g_jobMutex);
    thread_cond_destroy(g_jobCondition);
    dynarray_destroy(&g_runningJobs);
}

/**
 * @brief Submits a job graph for execution and returns a job ID
 *
 * Creates a job instance from the graph, adds it to the running jobs list,
 * and submits it to the executor for task scheduling. Returns immediately
 * with a unique job ID that can be used to track completion.
 *
 * @param graph Job graph to execute (must be validated)
 * @return Unique job ID for tracking this execution
 */
JobId jobs_scheduler_run(JobGraph* graph) {
    diag_assert_msg(jobs_graph_validate(graph), "Given job graph is invalid");
    diag_assert_msg(g_jobsIsWorker, "Only job-workers can run jobs");

    JobId id = (JobId)thread_atomic_add_i64(&g_jobIdCounter, 1);
    if (UNLIKELY(jobs_graph_task_root_count(graph) == 0)) {
        return id;
    }

    Job* job = job_create(g_alloc_heap, id, graph);

    thread_mutex_lock(g_jobMutex);

    *dynarray_push_t(&g_runningJobs, Job*) = job;

    thread_mutex_unlock(g_jobMutex);

    executor_run(job);

    return id;
}

/**
 * @brief Checks if a specific job has completed execution
 *
 * Thread-safe query to determine if a job has finished executing.
 * Can be called from any thread.
 *
 * @param job Job ID to check
 * @return true if the job has completed, false if still running
 */
bool jobs_scheduler_is_finished(const JobId job) {
    bool finished = true;
    
    thread_mutex_lock(g_jobMutex);
    
    finished = jobs_scheduler_is_finished_locked(job);

    thread_mutex_unlock(g_jobMutex);

    return finished;
}

/**
 * @brief Waits for a specific job to complete execution
 *
 * Blocks the calling thread until the specified job finishes.
 * Cannot be called from within a task execution context.
 *
 * @param job Job ID to wait for
 */
void jobs_scheduler_wait(const JobId job) {
    diag_assert_msg(!g_jobsIsWorking, "Waiting for job to finish is not allowed inside a task");

    thread_mutex_lock(g_jobMutex);

    while (!jobs_scheduler_is_finished_locked(job)) {
        thread_cond_wait(g_jobCondition, g_jobMutex);
    }

    thread_mutex_unlock(g_jobMutex);
}

/**
 * @brief Waits for a job while helping execute other tasks
 *
 * Similar to jobs_scheduler_wait but allows the calling thread to help
 * execute pending tasks while waiting. Can only be called from worker threads.
 *
 * @param job Job ID to wait for
 */
void jobs_scheduler_wait_help(const JobId job) {
    diag_assert_msg(g_jobsIsWorker, "Only job-workers can help out");

    while (true) {
        while (executor_help());

        if (jobs_scheduler_is_finished(job)) {
            return;
        }

        thread_yield();
    }
}

/**
 * @brief Marks a job as finished and cleans up its resources
 *
 * Called by the executor when all tasks in a job have completed.
 * Removes the job from the running list and notifies waiting threads.
 *
 * @param job Job instance that has completed
 */
void jobs_scheduler_finish(Job* job) {
    thread_mutex_lock(g_jobMutex);
    {
        job_destroy(g_alloc_heap, job);

        dynarray_for_t(&g_runningJobs, Job*, other, {
            if (*other == job) {
                dynarray_remove(&g_runningJobs, other_i, 1);
                break;
            }
        });
    }

    thread_mutex_unlock(g_jobMutex);
    thread_cond_broadcast(g_jobCondition);
}