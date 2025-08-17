/**
 * @file work_queue.h
 * @brief Work queue implementation for job system
 *
 * This header provides a lock-free work queue implementation for the job system.
 * The work queue supports both push/pop operations for the owning thread and
 * work stealing operations for other threads.
 */

#pragma once

#include "core_alloc.h"

#include "job.h"

/** @brief Maximum number of items that can be stored in a work queue */
#define workqueue_max_items 8192

/**
 * @brief Represents a single work item in the queue
 *
 * A work item consists of a job reference and a specific task ID
 * within that job to be executed.
 */
typedef struct {
    /** @brief Pointer to the job containing the task */
    Job* job;
    /** @brief ID of the specific task within the job */
    JobTaskId task;
} WorkItem;

/**
 * @brief Lock-free work queue structure
 *
 * Implements a work-stealing deque with top and bottom pointers
 * for efficient lock-free operations.
 */
typedef struct {
    /** @brief Top of the queue (for stealing) */
    i64 top;
    /** @brief Bottom of the queue (for local operations) */
    i64 bottom;
    /** @brief Array of work items */
    WorkItem* items;
} WorkQueue;

/**
 * @brief Check if a work item is valid
 * @param _WORKITEM_ The work item to check
 * @return true if the work item has a valid job pointer
 */
#define workitem_valid(_WORKITEM_) ((_WORKITEM_).job != null)

/**
 * @brief Create a new work queue
 * @param alloc Allocator to use for memory management
 * @return The newly created work queue
 */
WorkQueue workqueue_create(Allocator*);

/**
 * @brief Destroy a work queue and free its resources
 * @param alloc Allocator used to create the work queue
 * @param queue Pointer to the work queue to destroy
 */
void workqueue_destroy(Allocator*, WorkQueue*);

/**
 * @brief Get the current size of the work queue
 * @param queue The work queue to query
 * @return Number of items currently in the queue
 */
usize workqueue_size(const WorkQueue*);

/**
 * @brief Push a work item to the bottom of the queue
 * @param queue The work queue to push to
 * @param job The job containing the task
 * @param task The task ID within the job
 */
void workqueue_push(WorkQueue*, Job*, JobTaskId);

/**
 * @brief Pop a work item from the bottom of the queue (local operation)
 * @param queue The work queue to pop from
 * @return The popped work item, or invalid work item if queue is empty
 */
WorkItem workqueue_pop(WorkQueue*);

/**
 * @brief Steal a work item from the top of the queue (remote operation)
 * @param queue The work queue to steal from
 * @return The stolen work item, or invalid work item if queue is empty
 */
WorkItem workqueue_steal(WorkQueue*);