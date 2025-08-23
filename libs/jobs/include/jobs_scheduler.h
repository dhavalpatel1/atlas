#pragma once

#include "jobs_graph.h"

typedef u64 JobId;

JobId jobs_scheduler_run(JobGraph* graph);

bool jobs_scheduler_is_finished(JobId jobId);

void jobs_scheduler_wait(JobId jobId);

void jobs_scheduler_wait_help(JobId jobId);