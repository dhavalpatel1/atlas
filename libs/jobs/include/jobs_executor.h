#pragma once

#include "core_annotation.h"
#include "core_types.h"

typedef u16 JobWorkerId;

extern u16 g_jobsWorkerCount;

extern THREAD_LOCAL JobWorkerId g_jobsWorkerId;

extern THREAD_LOCAL bool g_jobsIsWorker;

extern THREAD_LOCAL bool g_jobsIsWorking;