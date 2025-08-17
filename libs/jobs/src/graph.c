/**
 * @file graph.c
 * @brief Job dependency graph implementation
 *
 * This file implements the job dependency graph data structure and operations.
 * It provides functionality for creating graphs, adding tasks with dependencies,
 * validating graph integrity (cycle detection), and analyzing graph properties
 * such as critical path length and parallelism potential.
 *
 * The graph uses adjacency lists to represent task dependencies efficiently,
 * with support for topological sorting and cycle detection algorithms.
 */

#include "core_alloc.h"
#include "core_bits.h"
#include "core_diag.h"
#include "core_math.h"
#include "core_sentinel.h"

#include "graph_internal.h"

#include "jobs_graph.h"

/**
 * @brief Gets a task link from the graph by ID
 *
 * @param graph Job graph containing the link
 * @param id ID of the link to retrieve
 * @return Pointer to the task link structure
 */
static JobTaskLink* jobs_graph_task_link(const JobGraph* graph, JobTaskLinkId id) {
    return dynarray_at_t(&graph->childLinks, id, JobTaskLink);
}

/**
 * @brief Adds a child task link to a parent task's child list
 *
 * Creates a new link in the adjacency list for child tasks. Ensures no
 * duplicate dependencies are added to the same parent task.
 *
 * @param graph Job graph to modify
 * @param childTask ID of the child task to add
 * @param linkHead Head of the existing child link list
 * @return ID of the newly created link
 */
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

/**
 * @brief Recursively checks for cycles starting from a specific task
 *
 * Uses depth-first search with processing/processed state tracking to detect
 * cycles in the task dependency graph.
 *
 * @param graph Job graph to analyze
 * @param task Starting task ID for cycle detection
 * @param processed Bitset of tasks that have been fully processed
 * @param processing Bitset of tasks currently being processed
 * @return true if a cycle is detected, false otherwise
 */
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

/**
 * @brief Checks if the job graph contains any dependency cycles
 *
 * Performs cycle detection on the entire graph using depth-first search.
 * A cycle would make the graph invalid for execution.
 *
 * @param graph Job graph to validate
 * @return true if cycles are found, false if the graph is acyclic
 */
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

/**
 * @brief Recursively performs topological sort insertion for a task
 *
 * Implements the recursive part of topological sorting, ensuring that
 * child tasks are visited before their parents in the sorted order.
 *
 * @param graph Job graph being sorted
 * @param task Current task being processed
 * @param processed Bitset tracking which tasks have been processed
 * @param sortedIndices Output array for topologically sorted task IDs
 */
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

/**
 * @brief Calculates the longest path (critical path) in the task graph
 *
 * Uses topological sorting and dynamic programming to find the length of
 * the longest path from any root task to any leaf task. This represents
 * the minimum possible execution time assuming infinite parallelism.
 *
 * @param graph Job graph to analyze
 * @return Length of the longest path in the graph
 */
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

/**
 * @brief Creates a new job dependency graph
 *
 * Allocates and initializes a new job graph with the specified initial capacity.
 * The graph can grow beyond this capacity as needed.
 *
 * @param alloc Allocator to use for graph memory
 * @param name Name identifier for the graph
 * @param taskCapacity Initial capacity for number of tasks
 * @return Pointer to the newly created job graph
 */
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

/**
 * @brief Destroys a job graph and frees all associated memory
 *
 * @param graph Job graph to destroy
 */
void jobs_graph_destroy(JobGraph* graph) {
    dynarray_for_t(&graph->tasks, JobTask, t, { string_free(graph->alloc, t->name); });
    dynarray_destroy(&graph->tasks);

    dynarray_destroy(&graph->parentCounts);
    dynarray_destroy(&graph->childSetHeads);
    dynarray_destroy(&graph->childLinks);

    string_free(graph->alloc, graph->name);
    alloc_free_t(graph->alloc, graph);
}

/**
 * @brief Adds a new task to the job graph
 *
 * Creates a new task with the specified routine and context data.
 * The task is initially independent with no dependencies.
 *
 * @param graph Job graph to add the task to
 * @param name Name identifier for the task
 * @param routine Function pointer to execute for this task
 * @param ctx Context data to store with the task
 * @return ID of the newly created task
 */
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

/**
 * @brief Creates a dependency relationship between two tasks
 *
 * Establishes that the child task depends on the parent task completing first.
 * The child task will not execute until the parent task finishes.
 *
 * @param graph Job graph to modify
 * @param parent ID of the parent task (dependency)
 * @param child ID of the child task (dependent)
 */
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

/**
 * @brief Validates that a job graph is executable (acyclic)
 *
 * @param graph Job graph to validate
 * @return true if the graph is valid (no cycles), false otherwise
 */
bool jobs_graph_validate(const JobGraph* graph) {
    return !jobs_graph_has_cycle(graph);
}

/**
 * @brief Gets the total number of tasks in the graph
 *
 * @param graph Job graph to query
 * @return Number of tasks in the graph
 */
usize jobs_graph_task_count(const JobGraph* graph) {
    return graph->tasks.size;
}

/**
 * @brief Gets the number of root tasks (tasks with no dependencies)
 *
 * @param graph Job graph to analyze
 * @return Number of root tasks
 */
usize jobs_graph_task_root_count(const JobGraph* graph) {
    usize count = 0;
    jobs_graph_for_task(graph, taskId, { count += !jobs_graph_task_has_parent(graph, taskId); });

    return count;
}

/**
 * @brief Gets the number of leaf tasks (tasks with no dependents)
 *
 * @param graph Job graph to analyze
 * @return Number of leaf tasks
 */
usize jobs_graph_task_leaf_count(const JobGraph* graph) {
    usize count = 0;
    jobs_graph_for_task(graph, taskId, { count += !jobs_graph_task_has_child(graph, taskId); });
    
    return count;
}

/**
 * @brief Gets the name of the job graph
 *
 * @param graph Job graph to query
 * @return Name string of the graph
 */
String jobs_graph_name(const JobGraph* graph) {
    return graph->name;
}

/**
 * @brief Gets the name of a specific task
 *
 * @param grpah Job graph containing the task
 * @param id ID of the task to query
 * @return Name string of the task
 */
String jobs_graph_task_name(const JobGraph* grpah, JobTaskId id) {
    return dynarray_at_t(&grpah->tasks, id, JobTask)->name;
}

/**
 * @brief Checks if a task has any parent dependencies
 *
 * @param graph Job graph to query
 * @param task ID of the task to check
 * @return true if the task has dependencies, false if it's a root task
 */
bool jobs_graph_task_has_parent(const JobGraph* graph, const JobTaskId task) {
    return jobs_graph_task_parent_count(graph, task) != 0;
}

/**
 * @brief Checks if a task has any child dependents
 *
 * @param graph Job graph to query
 * @param task ID of the task to check
 * @return true if the task has dependents, false if it's a leaf task
 */
bool jobs_graph_task_has_child(const JobGraph* graph, const JobTaskId task) {
    const JobTaskLinkId childSetHead = *dynarray_at_t(&graph->childSetHeads, task, JobTaskLinkId);

    return !sentinel_check(childSetHead);
}

/**
 * @brief Gets the number of parent dependencies for a task
 *
 * @param graph Job graph to query
 * @param task ID of the task to check
 * @return Number of parent dependencies
 */
usize jobs_graph_task_parent_count(const JobGraph* graph, const JobTaskId task) {
    return *dynarray_at_t(&graph->parentCounts, task, u32);
}

/**
 * @brief Gets an iterator for the child tasks of a given task
 *
 * @param graph Job graph to query
 * @param task ID of the task whose children to iterate
 * @return Iterator positioned at the first child task
 */
JobTaskChildItr jobs_graph_task_child_begin(const JobGraph* graph, const JobTaskId task) {
    const JobTaskLinkId childSetHead = *dynarray_at_t(&graph->childSetHeads, task, JobTaskLinkId);

    return jobs_graph_task_child_next(graph, (JobTaskChildItr){ .next = childSetHead });
}

/**
 * @brief Advances a child task iterator to the next child
 *
 * @param graph Job graph being iterated
 * @param itr Current iterator position
 * @return Iterator positioned at the next child task, or end iterator if finished
 */
JobTaskChildItr jobs_graph_task_child_next(const JobGraph* graph, const JobTaskChildItr itr) {
    if (sentinel_check(itr.next)) {
        return (JobTaskChildItr) { .task = sentinel_u32, .next = sentinel_u32 };
    }

    const JobTaskLink link = *jobs_graph_task_link(graph, itr.next);

    return (JobTaskChildItr){ .task = link.task, .next = link.next };
}

/**
 * @brief Gets the critical path length (minimum execution time) of the graph
 *
 * @param graph Job graph to analyze
 * @return Length of the longest dependency chain
 */
usize jobs_graph_task_span(const JobGraph* graph) {
    return (f32)jobs_graph_longestpath(graph);
}

/**
 * @brief Calculates the theoretical parallelism potential of the graph
 *
 * Returns the ratio of total work to critical path length, indicating
 * how much parallelism is theoretically possible.
 *
 * @param graph Job graph to analyze
 * @return Parallelism ratio (total tasks / critical path length)
 */
f32 jobs_graph_task_parallelism(const JobGraph* graph) {
    return (f32)jobs_graph_task_count(graph) / (f32)jobs_graph_task_span(graph);
}