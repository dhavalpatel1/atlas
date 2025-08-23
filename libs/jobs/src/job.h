
#pragma once

#include "jobs_graph.h"

typedef u64 JobId;

typedef struct {
    i64 dependancies;
} JobTaskData;

typedef struct {
    JobId id;
    const JobGraph* graph;
    i64 dependancies;
    JobTaskData taskData[];
} Job;

Job* job_create(Allocator* alloc, const JobId id, const JobGraph*);

void job_destroy(Allocator*, Job*);