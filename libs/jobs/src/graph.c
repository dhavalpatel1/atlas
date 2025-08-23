#include "core_alloc.h"
#include "core_bits.h"
#include "core_diag.h"
#include "core_math.h"
#include "core_sentinel.h"

#include "graph_internal.h"

#include "jobs_graph.h"

static JobTaskLink* jobs_graph_task_link(const JobGraph* graph, JobTaskLinkId id) {
    return dynarray_at_t(&graph->childLinks, id, JobTaskLink);
}

static JobTaskLinkId jobs_graph_add_task_child_link(JobGraph* graph, const JobTaskId childTask, JobTaskLinkId linkHead) {
    JobTaskLinkId lastLink = sentinel_u32;
    while (!sentinel_check(linkHead)) {
        lastLink = linkHead;
        const JobTaskLink* link = jobs_graph_task_link(graph, linkHead);
        diag_assert_msg(link->task != childTask, "Duplicate dependancy for task '{}' is not supported", fmt_int(childTask));
        linkHead = link->next;
    }

    const JobTaskLinkId newLinkIdx = (JobTaskLinkId)graph->childLinks.size;
    *dynarray_push_t(&graph->childLinks, JobTaskLink) = (JobTaskLink) {
        .task = childTask,
        .next = sentinel_u32
    };

    if (!sentinel_check(lastLink)) {
        jobs_graph_task_link(graph, lastLink)->next = newLinkIdx;
    }

    return newLinkIdx;
}

static bool jobs_graph_has_task_cycle(const JobGraph* graph, const JobTaskId task, BitSet processed, BitSet processing) {
    if (bitset_test(processed, task)) {
        return false;
    }

    if (bitset_test(processing, task)) {
        return true;
    }
    bitset_set(processing, task);

    jobs_graph_for_task_child(graph, task, child, {
        if (jobs_graph_has_task_cycle(graph, child.task, processed, processing)) {
            return true;
        }
    });

    bitset_clear(processing, task);
    bitset_set(processed, task);

    return false;
}

static bool jobs_graph_has_cycle(const JobGraph* graph) {
    BitSet processed = alloc_alloc(g_alloc_scratch, bits_to_bytes(graph->tasks.size) + 1, 1);
    BitSet processing = alloc_alloc(g_alloc_scratch, bits_to_bytes(graph->tasks.size) + 1, 1);

    mem_set(processed, 0);
    mem_set(processing, 0);

    jobs_graph_for_task(graph, taskId, {
        if (bitset_test(processed, taskId)) {
            continue;
        }

        if (jobs_graph_has_task_cycle(graph, taskId, processed, processing)) {
            return true;
        }
    });

    return false;
}

static void jobs_graph_topologically_insert(const JobGraph* graph, const JobTaskId task, BitSet processed, DynArray* sortedIndices) {
    bitset_set(processed, task);

    jobs_graph_for_task_child(graph, task, child, {
        if (bitset_test(processed, child.task)) {
            continue;
        }

        jobs_graph_topologically_insert(graph, child.task, processed, sortedIndices);
    });

    *dynarray_push_t(sortedIndices, JobTaskId) = task;
}

static usize jobs_graph_longestpath(const JobGraph* graph) {
    BitSet processed = alloc_alloc(g_alloc_scratch, bits_to_bytes(graph->tasks.size) + 1, 1);
    mem_set(processed, 0);

    DynArray sortedTasks = dynarray_create_t(g_alloc_heap, JobTaskId, graph->tasks.size);

    jobs_graph_for_task(graph, taskId, {
        if (bitset_test(processed, taskId)) {
            continue;
        }

        jobs_graph_topologically_insert(graph, taskId, processed, &sortedTasks);
    });

    DynArray distances = dynarray_create_t(g_alloc_heap, usize, graph->tasks.size);
    dynarray_resize(&distances, graph->tasks.size);
    dynarray_for_t(&distances, usize, itr, {
        const JobTaskId taskId = (JobTaskId)itr_i;
        *itr = jobs_graph_task_has_parent(graph, taskId) ? sentinel_usize : 1;
    });

    usize maxDist = 1;
    for (usize i = sortedTasks.size; i-- != 0;) {
        const JobTaskId taskId = *dynarray_at_t(&sortedTasks, i, JobTaskId);
        const usize currentDist = *dynarray_at_t(&distances, taskId, usize);

        if (!sentinel_check(currentDist)) {
            jobs_graph_for_task_child(graph, taskId, child, {
                usize* childDist = dynarray_at_t(&distances, child.task, usize);
                if (sentinel_check(*childDist) || *childDist < (currentDist + 1)) {
                    *childDist = currentDist + 1;
                }

                maxDist = math_max(maxDist, *childDist);
            });
        }
    }

    dynarray_destroy(&sortedTasks);
    dynarray_destroy(&distances);

    return maxDist;
}

JobGraph* jobs_graph_create(Allocator* alloc, const String name, const usize taskCapacity) {
    JobGraph* graph = alloc_alloc_t(alloc, JobGraph);

    *graph = (JobGraph) {
        .tasks = dynarray_create(alloc, 64, alignof(JobTask), taskCapacity),
        .parentCounts = dynarray_create_t(alloc, u32, taskCapacity),
        .childSetHeads = dynarray_create_t(alloc, JobTaskLinkId, taskCapacity),
        .childLinks = dynarray_create_t(alloc, JobTaskLink, taskCapacity),
        .name = string_dup(alloc, name),
        .alloc = alloc
    };

    return graph;
}

void jobs_graph_destroy(JobGraph* graph) {
    dynarray_for_t(&graph->tasks, JobTask, t, { string_free(graph->alloc, t->name); });
    dynarray_destroy(&graph->tasks);

    dynarray_destroy(&graph->parentCounts);
    dynarray_destroy(&graph->childSetHeads);
    dynarray_destroy(&graph->childLinks);

    string_free(graph->alloc, graph->name);
    alloc_free_t(graph->alloc, graph);
}

JobTaskId jobs_graph_add_task(JobGraph* graph, const String name, const JobTaskRoutine routine, Mem ctx) {
    const JobTaskId id = (JobTaskId)graph->tasks.size;

    Mem taskStorage = dynarray_push(&graph->tasks, 1);
    *((JobTask*)taskStorage.ptr) = (JobTask) {
        .routine = routine,
        .name = string_dup(graph->alloc, name)
    };
    mem_cpy(mem_consume(taskStorage, sizeof(JobTask)), ctx);

    *dynarray_push_t(&graph->parentCounts, u32) = 0;
    *dynarray_push_t(&graph->childSetHeads, JobTaskLinkId) = sentinel_u32;
    
    return id;
}

void jobs_graph_task_depend(JobGraph* graph, const JobTaskId parent, const JobTaskId child) {
    diag_assert(parent != child);

    ++(*dynarray_at_t(&graph->parentCounts, child, u32));

    JobTaskLinkId* parentChildSetHead = dynarray_at_t(&graph->childSetHeads, parent, JobTaskLinkId);
    if (sentinel_check(*parentChildSetHead)) {
        *parentChildSetHead = jobs_graph_add_task_child_link(graph, child, sentinel_u32);
    } else {
        jobs_graph_add_task_child_link(graph, child, *parentChildSetHead);
    }
}

bool jobs_graph_validate(const JobGraph* graph) {
    return !jobs_graph_has_cycle(graph);
}

usize jobs_graph_task_count(const JobGraph* graph) {
    return graph->tasks.size;
}

usize jobs_graph_task_root_count(const JobGraph* graph) {
    usize count = 0;
    jobs_graph_for_task(graph, taskId, { count += !jobs_graph_task_has_parent(graph, taskId); });

    return count;
}

usize jobs_graph_task_leaf_count(const JobGraph* graph) {
    usize count = 0;
    jobs_graph_for_task(graph, taskId, { count += !jobs_graph_task_has_child(graph, taskId); });
    
    return count;
}

String jobs_graph_name(const JobGraph* graph) {
    return graph->name;
}

String jobs_graph_task_name(const JobGraph* grpah, JobTaskId id) {
    return dynarray_at_t(&grpah->tasks, id, JobTask)->name;
}

bool jobs_graph_task_has_parent(const JobGraph* graph, const JobTaskId task) {
    return jobs_graph_task_parent_count(graph, task) != 0;
}

bool jobs_graph_task_has_child(const JobGraph* graph, const JobTaskId task) {
    const JobTaskLinkId childSetHead = *dynarray_at_t(&graph->childSetHeads, task, JobTaskLinkId);

    return !sentinel_check(childSetHead);
}

usize jobs_graph_task_parent_count(const JobGraph* graph, const JobTaskId task) {
    return *dynarray_at_t(&graph->parentCounts, task, u32);
}

JobTaskChildItr jobs_graph_task_child_begin(const JobGraph* graph, const JobTaskId task) {
    const JobTaskLinkId childSetHead = *dynarray_at_t(&graph->childSetHeads, task, JobTaskLinkId);

    return jobs_graph_task_child_next(graph, (JobTaskChildItr){ .next = childSetHead });
}

JobTaskChildItr jobs_graph_task_child_next(const JobGraph* graph, const JobTaskChildItr itr) {
    if (sentinel_check(itr.next)) {
        return (JobTaskChildItr) { .task = sentinel_u32, .next = sentinel_u32 };
    }

    const JobTaskLink link = *jobs_graph_task_link(graph, itr.next);

    return (JobTaskChildItr){ .task = link.task, .next = link.next };
}

usize jobs_graph_task_span(const JobGraph* graph) {
    return (f32)jobs_graph_longestpath(graph);
}

f32 jobs_graph_task_parallelism(const JobGraph* graph) {
    return (f32)jobs_graph_task_count(graph) / (f32)jobs_graph_task_span(graph);
}