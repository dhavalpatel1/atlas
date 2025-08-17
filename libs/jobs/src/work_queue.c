/**
 * @file work_queue.c
 * @brief Work queue data structure implementation
 *
 * This file implements a lock-free work-stealing queue data structure used
 * by the job executor. Each worker thread has its own work queue that supports
 * efficient push/pop operations from the owner thread and steal operations
 * from other threads.
 *
 * The implementation uses a circular buffer with atomic operations to provide
 * thread-safe access without locks. The queue supports up to workqueue_max_items
 * work items and uses a power-of-2 size for efficient modulo operations.
 */

#include "work_queue.h"

#include "core_alignof.h"
#include "core_diag.h"
#include "core_thread.h"

/**
 * @brief Wraps an index to fit within the circular buffer bounds
 */
#define item_wrap(_IDX_) ((_IDX_) & (workqueue_max_items - 1))

/**
 * @brief Creates a new work queue
 *
 * Allocates and initializes a work queue with empty state.
 * The queue starts with both top and bottom indices at 0.
 *
 * @param alloc Allocator to use for the queue's item storage
 * @return Initialized work queue structure
 */
WorkQueue workqueue_create(Allocator* alloc) {
    return (WorkQueue) {
        .bottom = 0,
        .top = 0,
        .items = alloc_alloc(alloc, sizeof(WorkItem) * workqueue_max_items, alignof(WorkItem)).ptr
    };
}

/**
 * @brief Destroys a work queue and frees its memory
 *
 * @param alloc Allocator that was used to create the queue
 * @param wq Work queue to destroy
 */
void workqueue_destroy(Allocator* alloc, WorkQueue* wq) {
    alloc_free(alloc, mem_create(wq->items, sizeof(WorkItem) * workqueue_max_items));
}

/**
 * @brief Gets the current number of items in the work queue
 *
 * Calculates the difference between bottom and top indices to determine
 * how many work items are currently queued.
 *
 * @param wq Work queue to query
 * @return Number of items currently in the queue
 */
usize workqueue_size(const WorkQueue* wq) {
    const i64 bottom = wq->bottom;
    const i64 top = wq->top;

    return (usize)(bottom >= top ? bottom - top : 0);
}

/**
 * @brief Pushes a work item onto the queue (owner thread only)
 *
 * Adds a new work item to the bottom of the queue. This operation should
 * only be performed by the thread that owns this queue.
 *
 * @param wq Work queue to push to
 * @param job Job containing the task to execute
 * @param task ID of the task within the job
 */
void workqueue_push(WorkQueue* wq, Job* job, JobTaskId task) {
    diag_assert_msg(workqueue_size(wq) != workqueue_max_items, "Maximum number of work-queue items ({}) has been exceeded", fmt_int(workqueue_max_items));

    const i64 idx = wq->bottom;
    wq->items[item_wrap(idx)] = (WorkItem) {
        .job = job,
        .task = task
    };

    thread_atomic_store_i64(&wq->bottom, idx + 1);
}

/**
 * @brief Pops a work item from the queue (owner thread only)
 *
 * Removes and returns a work item from the bottom of the queue.
 * Uses atomic operations to coordinate with potential stealers.
 * Returns an invalid work item if the queue is empty.
 *
 * @param wq Work queue to pop from
 * @return Work item if available, invalid work item if queue is empty
 */
WorkItem workqueue_pop(WorkQueue* wq) {
    const i64 idx = wq->bottom - 1;
    thread_atomic_store_i64(&wq->bottom, idx);

    i64 topIdx = wq->top;
    if (topIdx > idx) {
        wq->bottom = idx + 1;

        return (WorkItem){0};
    }

    WorkItem item = wq->items[item_wrap(idx)];
    if (idx != topIdx) {
        return item;
    }

    if (!thread_atomic_compare_exchange_i64(&wq->top, &topIdx, topIdx + 1)) {
        item = (WorkItem){0};
    }

    wq->bottom = idx + 1;

    return item;
}

/**
 * @brief Steals a work item from another thread's queue
 *
 * Attempts to steal a work item from the top of another thread's queue.
 * Uses atomic compare-and-swap operations to ensure thread safety.
 * Returns an invalid work item if stealing fails or queue is empty.
 *
 * @param wq Work queue to steal from (owned by another thread)
 * @return Work item if stolen successfully, invalid work item otherwise
 */
WorkItem workqueue_steal(WorkQueue* wq) {
    i64 idx = thread_atomic_load_i64(&wq->top);
    const i64 bottomIdx = thread_atomic_load_i64(&wq->bottom);

    if (idx >= bottomIdx) {
        return (WorkItem){0};
    }

    WorkItem item = wq->items[item_wrap(idx)];

    if (!thread_atomic_compare_exchange_i64(&wq->top, &idx, idx + 1)) {
        return (WorkItem){0};
    }

    return item;
}