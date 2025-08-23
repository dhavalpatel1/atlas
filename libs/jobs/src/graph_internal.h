
#include "core_dynarray.h"

#include "jobs_graph.h"

#define jobtask_max_user_data (usize)(64 - sizeof(JobTask))

typedef u32 JobTaskLinkId;

typedef struct {
    JobTaskRoutine routine;
    String name;
} JobTask;

typedef struct {
    JobTaskId task;
    JobTaskLinkId next;
} JobTaskLink;

struct sJobGraph {
    DynArray tasks;
    DynArray parentCounts;
    DynArray childSetHeads;
    DynArray childLinks;
    String name;
    Allocator* alloc;
};