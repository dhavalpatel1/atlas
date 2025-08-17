/**
 * @file job.c
 * @brief Individual job/task management implementation
 *
 * This file implements the Job data structure which represents a runtime
 * instance of a JobGraph. It manages the execution state and dependency
 * tracking for all tasks within a single job execution.
 *
 * Each Job contains dependency counters for tasks and tracks completion
 * status to coordinate with the scheduler and executor systems.
 */

#include "job.h"

#include "core_alloc.h"

#include "jobs_graph.h"

/**
 * @brief Creates a new job instance from a job graph
 *
 * Allocates a job structure with dependency tracking data for all tasks.
 * Initializes dependency counters based on the graph structure - each task's
 * dependency count equals its number of parent tasks.
 *
 * @param alloc Allocator to use for job memory
 * @param id Unique identifier for this job instance
 * @param graph Job graph template to create the job from
 * @return Pointer to the newly created job instance
 */
Job* job_create(Allocator* alloc, const JobId id, const JobGraph* graph) {
    const usize size = sizeof(Job) + sizeof(JobTaskData) * jobs_graph_task_count(graph);
    Job* data = alloc_alloc(alloc, size, alignof(Job)).ptr;

    data->id = id;
    data->graph = graph;
    data->dependancies = (i64)jobs_graph_task_leaf_count(graph);

    jobs_graph_for_task(graph, taskId, {
        data->taskData[taskId].dependancies = (i64)jobs_graph_task_parent_count(graph, taskId);
    });

    return data;
}

/**
 * @brief Destroys a job instance and frees its memory
 *
 * @param alloc Allocator that was used to create the job
 * @param job Job instance to destroy
 */
void job_destroy(Allocator* alloc, Job* job) {
    const usize size = sizeof(Job) + sizeof(JobTaskData) * jobs_graph_task_count(job->graph);
    alloc_free(alloc, mem_create(job, size));
}