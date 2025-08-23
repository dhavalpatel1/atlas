#include "core_path.h"
#include "core_thread.h"
#include "core_tty.h"

#include "jobs_executor.h"

#include "output_pretty.h"

typedef struct {
    AnvilOutput api;
    Allocator* alloc;
    File* file;
    String suiteName;
    AnvilRunFlags runFlags;
    bool style;
} AnvilOutputPretty;

static FormatArg arg_style_bold(AnvilOutputPretty* prettyOut) {
    return prettyOut->style ? fmt_ttystyle(.flags = TtyStyleFlags_Bold) : fmt_nop();
}

static FormatArg arg_style_dim(AnvilOutputPretty* prettyOut) {
    return prettyOut->style ? fmt_ttystyle(.flags = TtyStyleFlags_Faint) : fmt_nop();
}

static FormatArg arg_style_reset(AnvilOutputPretty* prettyOut) {
    return prettyOut->style ? fmt_ttystyle() : fmt_nop();
}

static FormatArg arg_style_result(AnvilOutputPretty* prettyOut, const AnvilResultType result) {
    const TtyFgColor color = result == AnvilResultType_Pass ? TtyFgColor_BrightGreen : TtyFgColor_BrightRed;

    return prettyOut->style ? fmt_ttystyle(.fgColor = color, .flags = TtyStyleFlags_Bold) : fmt_nop();
}

static void output_write(AnvilOutputPretty* prettyOut, const String str) {
    file_write_sync(prettyOut->file, str);
}

static void output_run_started(AnvilOutput* out) {
    AnvilOutputPretty* prettyOut = (AnvilOutputPretty*)out;

    const String str = fmt_write_scratch(
        "{}{}{}: Starting test run. {}(pid: {}, workers: {}){}\n",
        arg_style_bold(prettyOut),
        fmt_text(prettyOut->suiteName),
        arg_style_reset(prettyOut),
        arg_style_dim(prettyOut),
        fmt_int(g_thread_pid),
        fmt_int(g_jobsWorkerCount),
        arg_style_reset(prettyOut)
    );

    output_write(prettyOut, str);
}

static void output_tests_discovered(AnvilOutput* out, const usize count, const TimeDuration dur) {
    AnvilOutputPretty* prettyOut = (AnvilOutputPretty*)out;

    const String str = fmt_write_scratch(
        "> Discovered {}{}{} tests. {}({}){}\n",
        arg_style_bold(prettyOut),
        fmt_int(count),
        arg_style_reset(prettyOut),
        arg_style_dim(prettyOut),
        fmt_duration(dur),
        arg_style_reset(prettyOut)
    );

    output_write(prettyOut, str);
}

static void output_test_finished(AnvilOutput* out, const AnvilSpec* spec, const AnvilTest* test, const AnvilResultType type, AnvilResult* result) {
    AnvilOutputPretty* prettyOut = (AnvilOutputPretty*)out;

    if (!(prettyOut->runFlags & AnvilRunFlags_OutputPassingTests) && type != AnvilResultType_Fail) {
        return;
    }

    DynString str = dynstring_create(g_alloc_heap, 1024);
    fmt_write(&str, "* {}{}{}: ",
              arg_style_result(prettyOut, type),
              type == AnvilResultType_Pass ? fmt_text_lit("PASS") : fmt_text_lit("FAIL"),
              arg_style_reset(prettyOut));

    fmt_write(
        &str,
        "{}{}{}: {}. {}({}){}\n",
        arg_style_bold(prettyOut),
        fmt_text(spec->def->name),
        arg_style_reset(prettyOut),
        fmt_text(test->description),
        arg_style_dim(prettyOut),
        fmt_duration(result->duration),
        arg_style_reset(prettyOut)
    );

    dynarray_for_t(&result->errors, AnvilError, err, {
        fmt_write(
            &str,
            "  {}{}{} {}[file: {} line: {}]{}\n",
            arg_style_result(prettyOut, type),
            fmt_text(err->msg),
            arg_style_reset(prettyOut),
            arg_style_dim(prettyOut),
            fmt_path(err->source.file),
            fmt_int(err->source.line),
            arg_style_reset(prettyOut)
        );
    });

    output_write(prettyOut, dynstring_view(&str));
    dynstring_destroy(&str);
}

static void output_run_finished(AnvilOutput* out, const AnvilResultType type, const TimeDuration dur, const usize numPassed, const usize numFailed, const usize numSkipped) {
    AnvilOutputPretty* prettyOut = (AnvilOutputPretty*)out;

    const String str = fmt_write_scratch(
        "> Finished: {}{}{} [Passed: {}, Failed: {}, Skipped: {}] {}({}){}\n",
        arg_style_result(prettyOut, type),
        type == AnvilResultType_Pass ? fmt_text_lit("PASS") : fmt_text_lit("FAIL"),
        arg_style_reset(prettyOut),
        fmt_int(numPassed),
        fmt_int(numFailed),
        fmt_int(numSkipped),
        arg_style_dim(prettyOut),
        fmt_duration(dur),
        arg_style_reset(prettyOut)
    );

    output_write(prettyOut, str);
}

static void output_destroy(AnvilOutput* out) {
    AnvilOutputPretty* prettyOut = (AnvilOutputPretty*)out;
    alloc_free_t(prettyOut->alloc, prettyOut);
}

AnvilOutput* anvil_output_pretty_create(Allocator* alloc, File* file, AnvilRunFlags runFlags) {
    AnvilOutputPretty* prettyOut = alloc_alloc_t(alloc, AnvilOutputPretty);
    *prettyOut = (AnvilOutputPretty) {
        .api = {
            .runStarted = output_run_started,
            .testDiscovered = output_tests_discovered,
            .testFinished = output_test_finished,
            .runFinished = output_run_finished,
            .destroy = output_destroy,
        },
        .alloc = alloc,
        .file = file,
        .suiteName = path_stem(g_path_executable),
        .runFlags = runFlags,
        .style = tty_isatty(file),
    };

    return (AnvilOutput*)prettyOut;
}