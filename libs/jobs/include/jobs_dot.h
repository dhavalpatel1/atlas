#pragma once

#include "core_dynstring.h"
#include "jobs_graph.h"

typedef struct sFile File;

void jobs_dot_write_graph(DynString* str, JobGraph* graph);

void jobs_dot_dump_graph(File* file, JobGraph* graph);