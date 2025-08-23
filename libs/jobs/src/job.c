
#include "job.h"

#include "core_alloc.h"

#include "jobs_graph.h"

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

void job_destroy(Allocator* alloc, Job* job) {
    const usize size = sizeof(Job) + sizeof(JobTaskData) * jobs_graph_task_count(job->graph);
    alloc_free(alloc, mem_create(job, size));
}