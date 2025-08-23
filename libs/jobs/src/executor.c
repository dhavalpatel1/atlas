#include "core_diag.h"
#include "core_math.h"
#include "core_rng.h"
#include "core_thread.h"

#include "executor_internal.h"
#include "graph_internal.h"
#include "scheduler_internal.h"

#include "jobs_graph.h"

#include "work_queue.h"

#define worker_reserved_core_count 1

#define worker_min_count 1

#define worker_max_count 64

typedef enum {
    ExecMode_Running,
    ExecMode_Teardown
} ExecMode;

static ExecMode g_mode = ExecMode_Running;
static ThreadHandle g_workerThreads[worker_max_count];
static WorkQueue g_workerQueues[worker_max_count];
static i64 g_sleepingWorkers;
static ThreadMutex g_mutex;
static ThreadCondition g_wakeCondition;

u16 g_jobsWorkerCount;
THREAD_LOCAL JobWorkerId g_jobsWorkerId;
THREAD_LOCAL bool g_jobsIsWorker;
THREAD_LOCAL bool g_jobsIsWorking;

static WorkItem executor_steal() {
    JobWorkerId perfVictim = (JobWorkerId)rng_sample_range(g_rng, 0, g_jobsWorkerCount);
    for (u16 i = 0; i != g_jobsWorkerCount; ++i) {
        JobWorkerId victim = (perfVictim + i) % g_jobsWorkerCount;
        if (victim == g_jobsWorkerId) {
            continue;
        }

        WorkItem stolenItem = workqueue_steal(&g_workerQueues[victim]);
        if (workitem_valid(stolenItem)) {
            return stolenItem;
        }
    }

    return (WorkItem){0};
}

static WorkItem executor_steal_loop() {
    static const usize maxIterators = 1000;
    for (usize itr = 0; itr != maxIterators; ++itr) {
        WorkItem stolenItem = executor_steal();
        if (workitem_valid(stolenItem)) {
            return stolenItem;
        }

        thread_yield();
    }

    return (WorkItem){0};
}

static void executor_perform_work(WorkItem item) {
    JobTask* jobTaskDef = dynarray_at_t(&item.job->graph->tasks, item.task, JobTask);

    g_jobsIsWorking = true;
    jobTaskDef->routine((u8*)jobTaskDef + sizeof(JobTask));
    g_jobsIsWorking = false;

    if (jobs_graph_task_has_child(item.job->graph, item.task)) {
        bool taskPushed = false;
        jobs_graph_for_task_child(item.job->graph, item.task, child, {
            if (thread_atomic_sub_i64(&item.job->taskData[child.task].dependancies, 1) == 1) {
                workqueue_push(&g_workerQueues[g_jobsWorkerId], item.job, child.task);
                taskPushed = true;
            }
        });

        if (taskPushed && g_sleepingWorkers) {
            thread_cond_broadcast(g_wakeCondition);
        }

        return;
    }

    if (thread_atomic_sub_i64(&item.job->dependancies, 1) == 1) {
        jobs_scheduler_finish(item.job);
    }
}

static void executor_worker_thread(void* data) {
    g_jobsWorkerId = (JobWorkerId)(usize)data;
    g_jobsIsWorker = true;

    WorkItem work = (WorkItem){0};
    while (LIKELY(g_mode == ExecMode_Running)) {
        if (workitem_valid(work)) {
            executor_perform_work(work);
        }

        work = workqueue_pop(&g_workerQueues[g_jobsWorkerId]);
        if (!workitem_valid(work)) {
            work = executor_steal_loop();
        }

        if (workitem_valid(work)) {
            continue;
        }

        thread_mutex_lock(g_mutex);

        work = executor_steal();
        if (!workitem_valid(work) && LIKELY(g_mode == ExecMode_Running)) {
            ++g_sleepingWorkers;
            thread_cond_wait(g_wakeCondition, g_mutex);

            --g_sleepingWorkers;
        }

        thread_mutex_unlock(g_mutex);
    }
}

void executor_init() {
    g_jobsWorkerCount = math_min(math_max(worker_min_count, g_thread_core_count - worker_reserved_core_count), worker_max_count);
    g_mutex = thread_mutex_create(g_alloc_heap);
    g_wakeCondition = thread_cond_create(g_alloc_heap);

    for (u16 i = 0; i != g_jobsWorkerCount; ++i) {
        g_workerQueues[i] = workqueue_create(g_alloc_heap);
    }

    g_jobsWorkerId = 0;
    g_jobsIsWorker = true;

    for (u16 i = 1; i != g_jobsWorkerCount; ++i) {
        g_workerThreads[i] = thread_start(executor_worker_thread, (void*)(usize)i, fmt_write_scratch("jobs_exec_{}", fmt_int(i)));
    }
}

void executor_teardown() {
    diag_assert_msg(g_thread_tid == g_thread_main_tid, "Only the main-thread can teardown the executor");
    diag_assert_msg(g_jobsWorkerId == 0, "Unexpected worker-id for the main-thread");

    thread_mutex_lock(g_mutex);

    g_mode = ExecMode_Teardown;
    thread_cond_broadcast(g_wakeCondition);
    thread_mutex_unlock(g_mutex);

    for (u16 i = 1; i != g_jobsWorkerCount; ++i) {
        thread_join(g_workerThreads[i]);
    }

    for (u16 i = 0; i != g_jobsWorkerCount; ++i) {
        if (workqueue_size(&g_workerQueues[i])) {
            diag_print_err("jobs_executor: Worker {} has {} unfinished tasks.\n", fmt_int(i), fmt_int(workqueue_size(&g_workerQueues[i])));
        }

        workqueue_destroy(g_alloc_heap, &g_workerQueues[i]);
    }

    thread_cond_destroy(g_wakeCondition);
    thread_mutex_destroy(g_mutex);
}

void executor_run(Job* job) {
    diag_assert_msg(g_jobsIsWorker, "Only job-workers can run jobs");
    diag_assert_msg(g_jobsWorkerCount, "Job system has to be initialized jobs_init() first.");

    jobs_graph_for_task(job->graph, taskId, {
        if (jobs_graph_task_has_parent(job->graph, taskId)) {
            continue;
        }

        workqueue_push(&g_workerQueues[g_jobsWorkerId], job, taskId);
    });

    if (g_sleepingWorkers) {
        thread_cond_broadcast(g_wakeCondition);
    }
}

bool executor_help() {
    WorkItem work = workqueue_pop(&g_workerQueues[g_jobsWorkerId]);
    if (workitem_valid(work)) {
        executor_perform_work(work);
        
        return true;
    }

    work = executor_steal();
    if (workitem_valid(work)) {
        executor_perform_work(work);

        return true;
    }

    return false;
}