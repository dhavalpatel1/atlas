#pragma once

#include "core_alloc.h"
#include "log_logger.h"

typedef enum {
    LogSinkJsonFlags_None = 0,
    LogSinkJsonFlags_DestroyFile = 1 << 0,
} LogSinkJsonFlags;

LogSink* log_sink_json(Allocator*, File*, LogMask, LogSinkJsonFlags);

LogSink* log_sink_json_to_path(Allocator*, LogMask, String path);

LogSink* log_sink_json_default(Allocator*, LogMask);
