#pragma once

#include "log_logger.h"

typedef enum {
    LogSinkPrettyFlags_None = 0,
    LogSinkPrettyFlags_DestroyFile = 1 << 0,
} LogSinkPrettyFlags;

LogSink* log_sink_pretty(Allocator*, File*, LogMask, LogSinkPrettyFlags);

LogSink* log_sink_pretty_default(Allocator*, LogMask);
