#pragma once

#include "core_alloc.h"
#include "core_format.h"
#include "core_sourceloc.h"

typedef struct sAllocator Allocator;

typedef enum {
    LogLevel_Debug,
    LogLevel_Info,
    LogLevel_Warn,
    LogLevel_Error,

    LogLevel_Count,
} LogLevel;

typedef enum {
    LogMask_None = 0,
    LogMask_Debug = 1 << LogLevel_Debug,
    LogMask_Info = 1 << LogLevel_Info,
    LogMask_Warn = 1 << LogLevel_Warn,
    LogMask_Error = 1 << LogLevel_Error,
    LogMask_All = ~0,
} LogMask;

typedef struct {
    String name;
    FormatArg arg;
} LogParam;

typedef struct sLogger Logger;

typedef struct sLogSink LogSink;

#define log_param(_NAME_LIT_, _ARG_) ((LogParam){ .name = string_lit(_NAME_LIT_), .arg = (_ARG_) })

#define log_params(...) (const LogParam[]) { VA_ARGS_SKIP_FIRST(0, ##__VA_ARGS__, (LogParam){0}) }

#define log(_LOGGER_, _LVL_, _TXT_LIT_, ...)                                                                    \
    do {                                                                                                        \
        if (_LOGGER_) {                                                                                         \
            log_append((_LOGGER_), (_LVL_), source_location(), string_lit(_TXT_LIT_), log_params(__VA_ARGS__)); \
        }                                                                                                       \
    } while (false)

#define log_debug(_TXT_LIT_, ...) log(g_logger, LogLevel_Debug, _TXT_LIT_, __VA_ARGS__)
#define log_info(_TXT_LIT_, ...) log(g_logger, LogLevel_Info, _TXT_LIT_, __VA_ARGS__)
#define log_warn(_TXT_LIT_, ...) log(g_logger, LogLevel_Warn, _TXT_LIT_, __VA_ARGS__)
#define log_error(_TXT_LIT_, ...) log(g_logger, LogLevel_Error, _TXT_LIT_, __VA_ARGS__)

extern Logger* g_logger;

Logger* log_create(Allocator*);

void log_destroy(Logger*);

void log_add_sink(Logger*, LogSink*);

void log_append(Logger*, LogLevel, SourceLoc, String text, const LogParam* params);
