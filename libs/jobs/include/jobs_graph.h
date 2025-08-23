#pragma once

#include "core_string.h"
#include "core_types.h"

typedef struct sAllocator Allocator;

typedef u32 JobTaskId;

typedef struct {
    JobTaskId task;
    u32 next;
} JobTaskChildItr;

typedef void (*JobTaskRoutine)(void* context);

typedef struct sJobGraph JobGraph;

#define jobs_graph_for_task(_GRAPH_, _VAR_, ...)                                      \
    {                                                                                 \
        for (JobTaskId _VAR_ = 0; _VAR_ != jobs_graph_task_count(_GRAPH_); ++_VAR_) { \
            __VA_ARGS__                                                               \
        }                                                                             \
    }

#define jobs_graph_for_task_child(_GRAPH_, _TASK_, _VAR_, ...)                                                                                                        \
    {                                                                                                                                                                 \
        for (JobTaskChildItr _VAR_ = jobs_graph_task_child_begin(_GRAPH_, _TASK_); !sentinel_check(_VAR_.task); _VAR_ = jobs_graph_task_child_next(_GRAPH_, _VAR_)) { \
            __VA_ARGS__                                                                                                                                               \
        }                                                                                                                                                             \
    }

JobGraph* jobs_graph_create(Allocator* allocator, String name, usize taskCapacity);

void jobs_graph_destroy(JobGraph* graph);

JobTaskId jobs_graph_add_task(JobGraph* graph, String name, JobTaskRoutine routine, Mem ctx);

void jobs_graph_task_depend(JobGraph* graph, JobTaskId parent, JobTaskId child);

bool jobs_graph_validate(const JobGraph* graph);

usize jobs_graph_task_count(const JobGraph* graph);

usize jobs_graph_task_root_count(const JobGraph* graph);

usize jobs_graph_task_leaf_count(const JobGraph* graph);

String jobs_graph_name(const JobGraph* graph);

String jobs_graph_task_name(const JobGraph* graph, JobTaskId taskId);

bool jobs_graph_task_has_parent(const JobGraph* graph, JobTaskId taskId);

bool jobs_graph_task_has_child(const JobGraph* graph, JobTaskId taskId);

usize jobs_graph_task_parent_count(const JobGraph* graph, JobTaskId taskId);

JobTaskChildItr jobs_graph_task_child_begin(const JobGraph* graph, JobTaskId taskId);

JobTaskChildItr jobs_graph_task_child_next(const JobGraph* graph, JobTaskChildItr itr);

usize jobs_graph_task_span(const JobGraph* graph);

f32 jobs_graph_task_parallelism(const JobGraph* graph);