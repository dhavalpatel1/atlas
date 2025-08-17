/**
 * @file scheduler_internal.h
 * @brief Internal job scheduler interface
 *
 * This header defines the internal interface for the job scheduler
 * subsystem including job completion handling and scheduling operations.
 */

#include "job.h"

#include "jobs_scheduler.h"

void jobs_scheduler_finish(Job*);