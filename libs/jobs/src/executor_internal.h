/**
 * @file executor_internal.h
 * @brief Internal job executor interface
 *
 * This header defines the internal interface for the job executor
 * subsystem including job execution and work assistance operations.
 */

#include "job.h"

#include "jobs_executor.h"

void executor_run(Job*);

bool executor_help();