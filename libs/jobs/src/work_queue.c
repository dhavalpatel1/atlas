
#include "work_queue.h"

#include "core_alignof.h"
#include "core_diag.h"
#include "core_thread.h"

#define item_wrap(_IDX_) ((_IDX_) & (workqueue_max_items - 1))

WorkQueue workqueue_create(Allocator* alloc) {
    return (WorkQueue) {
        .bottom = 0,
        .top = 0,
        .items = alloc_alloc(alloc, sizeof(WorkItem) * workqueue_max_items, alignof(WorkItem)).ptr
    };
}

void workqueue_destroy(Allocator* alloc, WorkQueue* wq) {
    alloc_free(alloc, mem_create(wq->items, sizeof(WorkItem) * workqueue_max_items));
}

usize workqueue_size(const WorkQueue* wq) {
    const i64 bottom = wq->bottom;
    const i64 top = wq->top;

    return (usize)(bottom >= top ? bottom - top : 0);
}

void workqueue_push(WorkQueue* wq, Job* job, JobTaskId task) {
    diag_assert_msg(workqueue_size(wq) != workqueue_max_items, "Maximum number of work-queue items ({}) has been exceeded", fmt_int(workqueue_max_items));

    const i64 idx = wq->bottom;
    wq->items[item_wrap(idx)] = (WorkItem) {
        .job = job,
        .task = task
    };

    thread_atomic_store_i64(&wq->bottom, idx + 1);
}

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