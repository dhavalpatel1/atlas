/**
 * @file jobs_graph.h
 * @brief Job dependency graph management and task execution framework.
 *
 * This file provides the core functionality for creating and managing job dependency
 * graphs. It defines the structures and interfaces for building directed acyclic graphs
 * (DAGs) of job tasks, where edges represent dependencies between tasks. The system
 * supports parallel execution of independent tasks while respecting dependency constraints.
 */
#pragma once

#include "core_string.h"
#include "core_types.h"

typedef struct sAllocator Allocator;

/**
 * @brief Identifier for job tasks within a job graph.
 *
 * Each task in a job graph is assigned a unique identifier that can be used
 * to reference the task for dependency management and execution tracking.
 */
typedef u32 JobTaskId;

/**
 * @brief Iterator for traversing child tasks of a job task.
 *
 * This structure is used to iterate over the children (dependent tasks) of a
 * given task in the job graph. The iterator maintains the current task and
 * the next position in the iteration sequence.
 */
typedef struct {
    JobTaskId task;  /**< Current child task being iterated */
    u32 next;        /**< Internal state for iteration continuation */
} JobTaskChildItr;

/**
 * @brief Function pointer type for job task execution routines.
 *
 * Job tasks are implemented as functions that accept a context pointer.
 * The context can contain any data needed for task execution and is
 * specified when the task is added to the job graph.
 *
 * @param context Pointer to task-specific execution context
 */
typedef void (*JobTaskRoutine)(void* context);

/**
 * @brief Opaque structure representing a job dependency graph.
 *
 * The JobGraph structure contains all the data necessary to represent
 * a directed acyclic graph of job tasks and their dependencies.
 */
typedef struct sJobGraph JobGraph;

/**
 * @brief Macro for iterating over all tasks in a job graph.
 *
 * This macro provides a convenient way to iterate over all tasks in a job graph.
 * The iteration variable is automatically declared and incremented, and the
 * loop body can access the current task ID.
 *
 * @param _GRAPH_ Pointer to the JobGraph to iterate over
 * @param _VAR_ Name of the JobTaskId variable to use for iteration
 * @param ... Code block to execute for each task
 */
#define jobs_graph_for_task(_GRAPH_, _VAR_, ...)                                      \
    {                                                                                 \
        for (JobTaskId _VAR_ = 0; _VAR_ != jobs_graph_task_count(_GRAPH_); ++_VAR_) { \
            __VA_ARGS__                                                               \
        }                                                                             \
    }

/**
 * @brief Macro for iterating over child tasks of a specific task.
 *
 * This macro provides a convenient way to iterate over all child tasks
 * (dependencies) of a given task in the job graph. The iteration continues
 * until all children have been processed.
 *
 * @param _GRAPH_ Pointer to the JobGraph containing the tasks
 * @param _TASK_ JobTaskId of the parent task whose children to iterate
 * @param _VAR_ Name of the JobTaskChildItr variable to use for iteration
 * @param ... Code block to execute for each child task
 */
#define jobs_graph_for_task_child(_GRAPH_, _TASK_, _VAR_, ...)                                                                                                        \
    {                                                                                                                                                                 \
        for (JobTaskChildItr _VAR_ = jobs_graph_task_child_begin(_GRAPH_, _TASK_); !sentinel_check(_VAR_.task); _VAR_ = jobs_graph_task_child_next(_GRAPH_, _VAR_)) { \
            __VA_ARGS__                                                                                                                                               \
        }                                                                                                                                                             \
    }

/**
 * @brief Create a new job dependency graph.
 *
 * Allocates and initializes a new job graph with the specified name and
 * capacity for tasks. The graph starts empty and tasks can be added using
 * jobs_graph_add_task().
 *
 * @param allocator Memory allocator to use for graph allocation
 * @param name Name identifier for the job graph
 * @param taskCapacity Maximum number of tasks the graph can contain
 * @return Pointer to the newly created job graph
 */
JobGraph* jobs_graph_create(Allocator* allocator, String name, usize taskCapacity);

/**
 * @brief Destroy a job graph and free its resources.
 *
 * Deallocates all memory associated with the job graph, including task
 * data and dependency information. The graph pointer becomes invalid
 * after this call.
 *
 * @param graph Job graph to destroy
 */
void jobs_graph_destroy(JobGraph* graph);

/**
 * @brief Add a new task to the job graph.
 *
 * Creates a new task in the job graph with the specified name, execution
 * routine, and context data. The task starts with no dependencies and
 * can be connected to other tasks using jobs_graph_task_depend().
 *
 * @param graph Job graph to add the task to
 * @param name Name identifier for the task
 * @param routine Function to execute when the task runs
 * @param ctx Memory containing context data for task execution
 * @return Unique identifier for the newly created task
 */
JobTaskId jobs_graph_add_task(JobGraph* graph, String name, JobTaskRoutine routine, Mem ctx);

/**
 * @brief Establish a dependency relationship between two tasks.
 *
 * Creates a dependency edge from the parent task to the child task,
 * ensuring that the parent task completes before the child task can
 * begin execution. This maintains the directed acyclic graph property.
 *
 * @param graph Job graph containing both tasks
 * @param parent Task that must complete first
 * @param child Task that depends on the parent
 */
void jobs_graph_task_depend(JobGraph* graph, JobTaskId parent, JobTaskId child);

/**
 * @brief Validate the job graph for correctness.
 *
 * Performs validation checks on the job graph to ensure it forms a
 * valid directed acyclic graph (DAG) without cycles. This should be
 * called before attempting to execute the graph.
 *
 * @param graph Job graph to validate
 * @return true if the graph is valid, false if cycles or other issues are detected
 */
bool jobs_graph_validate(const JobGraph* graph);

/**
 * @brief Get the total number of tasks in the job graph.
 *
 * @param graph Job graph to query
 * @return Number of tasks currently in the graph
 */
usize jobs_graph_task_count(const JobGraph* graph);

/**
 * @brief Get the number of root tasks (tasks with no dependencies).
 *
 * Root tasks are tasks that have no parent dependencies and can be
 * executed immediately when the graph execution begins.
 *
 * @param graph Job graph to query
 * @return Number of root tasks in the graph
 */
usize jobs_graph_task_root_count(const JobGraph* graph);

/**
 * @brief Get the number of leaf tasks (tasks with no dependents).
 *
 * Leaf tasks are tasks that no other tasks depend on. They represent
 * the final stages of computation in the job graph.
 *
 * @param graph Job graph to query
 * @return Number of leaf tasks in the graph
 */
usize jobs_graph_task_leaf_count(const JobGraph* graph);

/**
 * @brief Get the name of the job graph.
 *
 * @param graph Job graph to query
 * @return String containing the graph's name
 */
String jobs_graph_name(const JobGraph* graph);

/**
 * @brief Get the name of a specific task.
 *
 * @param graph Job graph containing the task
 * @param taskId Identifier of the task to query
 * @return String containing the task's name
 */
String jobs_graph_task_name(const JobGraph* graph, JobTaskId taskId);

/**
 * @brief Check if a task has any parent dependencies.
 *
 * @param graph Job graph containing the task
 * @param taskId Identifier of the task to check
 * @return true if the task has parent dependencies, false if it's a root task
 */
bool jobs_graph_task_has_parent(const JobGraph* graph, JobTaskId taskId);

/**
 * @brief Check if a task has any child dependencies.
 *
 * @param graph Job graph containing the task
 * @param taskId Identifier of the task to check
 * @return true if the task has child dependencies, false if it's a leaf task
 */
bool jobs_graph_task_has_child(const JobGraph* graph, JobTaskId taskId);

/**
 * @brief Get the number of parent dependencies for a task.
 *
 * @param graph Job graph containing the task
 * @param taskId Identifier of the task to query
 * @return Number of tasks that this task depends on
 */
usize jobs_graph_task_parent_count(const JobGraph* graph, JobTaskId taskId);

/**
 * @brief Begin iteration over child tasks of a given task.
 *
 * Initializes an iterator for traversing all child tasks (dependents)
 * of the specified task. Use with jobs_graph_task_child_next() to
 * iterate through all children.
 *
 * @param graph Job graph containing the task
 * @param taskId Identifier of the parent task
 * @return Iterator positioned at the first child task
 */
JobTaskChildItr jobs_graph_task_child_begin(const JobGraph* graph, JobTaskId taskId);

/**
 * @brief Advance the child task iterator to the next child.
 *
 * Moves the iterator to the next child task in the iteration sequence.
 * When all children have been visited, the iterator's task field will
 * be set to a sentinel value that can be checked with sentinel_check().
 *
 * @param graph Job graph containing the tasks
 * @param itr Current iterator position
 * @return Updated iterator positioned at the next child task
 */
JobTaskChildItr jobs_graph_task_child_next(const JobGraph* graph, JobTaskChildItr itr);

/**
 * @brief Calculate the critical path length (span) of the job graph.
 *
 * The span represents the longest path through the dependency graph,
 * which determines the minimum execution time assuming unlimited
 * parallelism. This is useful for performance analysis.
 *
 * @param graph Job graph to analyze
 * @return Length of the critical path in task units
 */
usize jobs_graph_task_span(const JobGraph* graph);

/**
 * @brief Calculate the theoretical parallelism of the job graph.
 *
 * Computes the ratio of total work to critical path length, which
 * represents the theoretical maximum speedup achievable with perfect
 * parallelization. Higher values indicate better parallelization potential.
 *
 * @param graph Job graph to analyze
 * @return Parallelism factor (total tasks / critical path length)
 */
f32 jobs_graph_task_parallelism(const JobGraph* graph);