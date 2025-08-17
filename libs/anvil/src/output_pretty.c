/**
 * @file output_pretty.c
 * @brief Implementation of pretty-printed test output formatting
 *
 * This file implements a formatted output handler for Anvil test results
 * with support for color styling, timing information, and detailed error
 * reporting suitable for terminal display.
 */

#include "core_path.h"
#include "core_thread.h"
#include "core_tty.h"

#include "jobs_executor.h"

#include "output_pretty.h"

/**
 * @brief Pretty output handler implementation
 *
 * Extends the base AnvilOutput interface with formatting state
 * and styling configuration for terminal-friendly output.
 */
typedef struct {
    AnvilOutput api;           ///< Base output interface
    Allocator* alloc;          ///< Memory allocator
    File* file;                ///< Output file handle
    String suiteName;          ///< Name of the test suite
    AnvilRunFlags runFlags;    ///< Runtime configuration flags
    bool style;                ///< Whether to use terminal styling
} AnvilOutputPretty;

/**
 * @brief Generate bold style formatting argument
 *
 * Returns a formatting argument for bold text if styling is enabled,
 * or a no-op formatting argument if styling is disabled.
 *
 * @param prettyOut The pretty output handler
 * @return Formatting argument for bold style or no-op
 */
static FormatArg arg_style_bold(AnvilOutputPretty* prettyOut) {
    return prettyOut->style ? fmt_ttystyle(.flags = TtyStyleFlags_Bold) : fmt_nop();
}

/**
 * @brief Generate dim style formatting argument
 *
 * Returns a formatting argument for faint/dim text if styling is enabled,
 * or a no-op formatting argument if styling is disabled.
 *
 * @param prettyOut The pretty output handler
 * @return Formatting argument for dim style or no-op
 */
static FormatArg arg_style_dim(AnvilOutputPretty* prettyOut) {
    return prettyOut->style ? fmt_ttystyle(.flags = TtyStyleFlags_Faint) : fmt_nop();
}

/**
 * @brief Generate style reset formatting argument
 *
 * Returns a formatting argument to reset terminal styling if styling is enabled,
 * or a no-op formatting argument if styling is disabled.
 *
 * @param prettyOut The pretty output handler
 * @return Formatting argument for style reset or no-op
 */
static FormatArg arg_style_reset(AnvilOutputPretty* prettyOut) {
    return prettyOut->style ? fmt_ttystyle() : fmt_nop();
}

/**
 * @brief Generate result-specific style formatting argument
 *
 * Returns a formatting argument with color coding based on test result:
 * green for pass, red for fail. Uses bold styling if enabled.
 *
 * @param prettyOut The pretty output handler
 * @param result The test result type
 * @return Formatting argument with result-appropriate color or no-op
 */
static FormatArg arg_style_result(AnvilOutputPretty* prettyOut, const AnvilResultType result) {
    const TtyFgColor color = result == AnvilResultType_Pass ? TtyFgColor_BrightGreen : TtyFgColor_BrightRed;

    return prettyOut->style ? fmt_ttystyle(.fgColor = color, .flags = TtyStyleFlags_Bold) : fmt_nop();
}

/**
 * @brief Write formatted output to the file handle
 *
 * Synchronously writes the provided string to the output file handle.
 *
 * @param prettyOut The pretty output handler
 * @param str The string to write
 */
static void output_write(AnvilOutputPretty* prettyOut, const String str) {
    file_write_sync(prettyOut->file, str);
}

/**
 * @brief Handle test run start event
 *
 * Outputs a formatted message indicating the start of test execution
 * with process ID and worker count information.
 *
 * @param out The output handler (cast to AnvilOutputPretty)
 */
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

/**
 * @brief Handle test discovery completion event
 *
 * Outputs the number of discovered tests and the time taken for discovery.
 *
 * @param out The output handler (cast to AnvilOutputPretty)
 * @param count Number of tests discovered
 * @param dur Time taken for test discovery
 */
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

/**
 * @brief Handle individual test completion event
 *
 * Outputs formatted test results including pass/fail status, timing,
 * and detailed error information if the test failed. Respects output
 * flags for showing passing tests.
 *
 * @param out The output handler (cast to AnvilOutputPretty)
 * @param spec The specification containing the test
 * @param test The completed test
 * @param type The result type (pass/fail)
 * @param result The detailed test result with errors and timing
 */
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

/**
 * @brief Handle test run completion event
 *
 * Outputs a summary of the entire test run including overall result,
 * timing, and counts of passed, failed, and skipped tests.
 *
 * @param out The output handler (cast to AnvilOutputPretty)
 * @param type Overall result type (pass if no failures, fail otherwise)
 * @param dur Total execution time
 * @param numPassed Number of tests that passed
 * @param numFailed Number of tests that failed
 * @param numSkipped Number of tests that were skipped
 */
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

/**
 * @brief Destroy the pretty output handler
 *
 * Frees all resources associated with the pretty output handler.
 *
 * @param out The output handler to destroy (cast to AnvilOutputPretty)
 */
static void output_destroy(AnvilOutput* out) {
    AnvilOutputPretty* prettyOut = (AnvilOutputPretty*)out;
    alloc_free_t(prettyOut->alloc, prettyOut);
}

/**
 * @brief Create a new pretty output handler
 *
 * Allocates and initializes a new pretty output handler with the specified
 * configuration. Automatically detects terminal capability for styling.
 *
 * @param alloc The allocator to use for memory management
 * @param file The file handle to write output to
 * @param runFlags Runtime flags controlling output behavior
 * @return Pointer to the newly created output handler, or null on failure
 */
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