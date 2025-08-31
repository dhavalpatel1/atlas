#include "core_sourceloc.h"
#include "core_time.h"
#include "log_logger.h"

struct sLogSink {
    void (*write)(LogSink*, LogLevel, SourceLoc, TimeReal, String, const LogParam* params);

    void (*destroy)(LogSink*);
};
