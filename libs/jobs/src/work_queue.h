
#pragma once

#include "core_alloc.h"

#include "job.h"

#define workqueue_max_items 8192

typedef struct {
    Job* job;
    JobTaskId task;
} WorkItem;

typedef struct {
    i64 top;
    i64 bottom;
    WorkItem* items;
} WorkQueue;

#define workitem_valid(_WORKITEM_) ((_WORKITEM_).job != null)

WorkQueue workqueue_create(Allocator*);

void workqueue_destroy(Allocator*, WorkQueue*);

usize workqueue_size(const WorkQueue*);

void workqueue_push(WorkQueue*, Job*, JobTaskId);

WorkItem workqueue_pop(WorkQueue*);

WorkItem workqueue_steal(WorkQueue*);