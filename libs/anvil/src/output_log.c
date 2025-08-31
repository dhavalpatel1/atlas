#include "anvil_runner.h"
#include "core_alloc.h"
#include "core_format.h"
#include "core_path.h"
#include "core_thread.h"

#include "core_time.h"
#include "jobs_executor.h"

#include "log_logger.h"
#include "output.h"

#include "output_log.h"

typedef struct {
    AnvilOutput api;
    Allocator* alloc;
    Logger* logger;
} AnvilOutputLog;

static void output_run_started(AnvilOutput* out) {
    AnvilOutputLog* logOut = (AnvilOutputLog*)out;

    log(logOut->logger, LogLevel_Info, "Starting test run",
        log_param("pid", fmt_int(g_thread_pid)),
        log_param("workers", fmt_int(g_jobsWorkerCount)),
        log_param("executable", fmt_path(g_path_executable)));
}

static void output_tests_discovered(AnvilOutput* out, const usize count, const TimeDuration dur) {
    AnvilOutputLog* logOut = (AnvilOutputLog*)out;

    log(logOut->logger, LogLevel_Debug, "Test discovery complete",
        log_param("count", fmt_int(count)),
        log_param("duration", fmt_duration(dur))
    );
}

static void output_test_finished(AnvilOutput* out, const AnvilSpec* spec, const AnvilTest* test, const AnvilResultType type, AnvilResult* result) {
    AnvilOutputLog* logOut = (AnvilOutputLog*)out;

    log(logOut->logger, LogLevel_Info, "Test finished",
        log_param("spec", fmt_text(spec->def->name)),
        log_param("test", fmt_text(test->description)),
        log_param("result", type == AnvilResultType_Pass ? fmt_text_lit("pass") : fmt_text_lit("fail")),
        log_param("duration", fmt_duration(result->duration))
    );

    dynarray_for_t(&result->errors, AnvilError, err, {
        log(logOut->logger, LogLevel_Error, "Test anvil failure",
            log_param("message", fmt_text(err->msg)),
            log_param("source-file", fmt_path(err->source.file)),
            log_param("source-line", fmt_int(err->source.line))
        );
    });
}

static void output_run_finished(AnvilOutput* out, const AnvilResultType type, const TimeDuration dur, const usize numPassed, const usize numFailed, const usize numSkipped) {
    AnvilOutputLog* logOut = (AnvilOutputLog*)out;

    log(logOut->logger, LogLevel_Info, "Finished test run",
        log_param("passed", fmt_int(numPassed)),
        log_param("failed", fmt_int(numFailed)),
        log_param("skipped", fmt_int(numSkipped)),
        log_param("result", type == AnvilResultType_Pass ? fmt_text_lit("pass") : fmt_text_lit("fail")),
        log_param("duration", fmt_duration(dur))
    );
}

static void output_destroy(AnvilOutput* out) {
    AnvilOutputLog* logOut = (AnvilOutputLog*) out;
    alloc_free_t(logOut->alloc, logOut);
}

AnvilOutput* anvil_output_log_create(Allocator* alloc, Logger* logger) {
    AnvilOutputLog* logOut = alloc_alloc_t(alloc, AnvilOutputLog);
    *logOut = (AnvilOutputLog) {
        .api = {
            .runStarted = output_run_started,
            .testDiscovered = output_tests_discovered,
            .testFinished = output_test_finished,
            .runFinished = output_run_finished,
            .destroy = output_destroy,
        },
        .alloc = alloc,
        .logger = logger,
    };

    return (AnvilOutput*)logOut;
}
