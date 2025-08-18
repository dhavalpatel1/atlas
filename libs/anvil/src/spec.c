/**
 * @file spec.c
 * @brief Implementation of Anvil test specification management and execution
 *
 * This file implements the core functionality for discovering, executing, and
 * managing test specifications including test context handling, assertion
 * processing, and specification lifecycle management.
 */

#include "anvil_spec.h"

#include "core_alloc.h"
#include "core_diag.h"
#include "core_time.h"

#include "spec_internal.h"

/**
 * @brief Context for test discovery phase
 *
 * Used during the test discovery phase to collect all tests defined
 * within a specification and track focus flags.
 */
typedef struct {
    AnvilSpecContext api;      ///< Base specification context interface
    AnvilSpec* spec;           ///< The specification being discovered
} ContextDiscover;

/**
 * @brief Context for test execution phase
 *
 * Used during test execution to identify which specific test to run
 * and provide the execution context.
 */
typedef struct {
    AnvilSpecContext api;      ///< Base specification context interface
    AnvilTestId testToExec;    ///< ID of the test to execute
    AnvilTestContext* testCtx; ///< The test execution context
} ContextExec;

/**
 * @brief Test visitor function for discovery phase
 *
 * Collects test definitions during the discovery phase and tracks
 * whether any tests have focus flags set.
 *
 * @param ctx The specification context
 * @param test The test definition to add
 * @return Always returns null (no test context needed during discovery)
 */
static AnvilTestContext* anvil_spec_test_discover(AnvilSpecContext* ctx, AnvilTest test) {
    ContextDiscover* discoverCtx = (ContextDiscover*)ctx;

    discoverCtx->spec->focus |= test.flags & AnvilTestFlags_Focus;

    *dynarray_push_t(&discoverCtx->spec->tests, AnvilTest) = test;

    return null;
}

/**
 * @brief Test visitor function for execution phase
 *
 * Identifies the target test for execution and provides the test context
 * if this is the test to be executed.
 *
 * @param ctx The specification context
 * @param test The test definition being visited
 * @return Test context if this is the target test, null otherwise
 */
static AnvilTestContext* anvil_spec_test_exec(AnvilSpecContext* ctx, AnvilTest test) {
    ContextExec* execCtx = (ContextExec*)ctx;

    if (test.id != execCtx->testToExec) {
        return null;
    }

    execCtx->testCtx->started = true;

    return execCtx->testCtx;
}

/**
 * @brief Custom assertion handler for test execution
 *
 * Handles assertion failures during test execution by reporting them
 * as test errors rather than terminating the program.
 *
 * @param msg The assertion failure message
 * @param source Source location where the assertion failed
 * @param context Test context for error reporting
 * @return Always returns true to continue execution after assertion failure
 */
static bool anvil_assert_handler(String msg, const SourceLoc source, void* context) {
    anvil_report_error(context, msg, source);

    return true;
}

/**
 * @brief Check if setup phase should be executed
 *
 * Determines whether the current specification context should execute
 * setup code based on the context flags.
 *
 * @param ctx The specification context
 * @return True if setup should be executed, false otherwise
 */
bool anvil_visit_setup(AnvilSpecContext* ctx) {
    return (ctx->flags & AnvilSpecContextFlags_Setup) != 0;
}

/**
 * @brief Check if teardown phase should be executed
 *
 * Determines whether the current specification context should execute
 * teardown code based on the context flags.
 *
 * @param ctx The specification context
 * @return True if teardown should be executed, false otherwise
 */
bool anvil_visit_teardown(AnvilSpecContext* ctx) {
    return (ctx->flags & AnvilSpecContextFlags_Teardown) != 0;
}

/**
 * @brief Visit a test definition during specification execution
 *
 * Calls the appropriate test visitor function if one is configured
 * in the specification context.
 *
 * @param ctx The specification context
 * @param test The test definition to visit
 * @return Test context if the test should be executed, null otherwise
 */
AnvilTestContext* anvil_visit_test(AnvilSpecContext* ctx, const AnvilTest test) {
    return ctx->visitTest ? ctx->visitTest(ctx, test) : null;
}

/**
 * @brief Create and discover tests in a specification
 *
 * Creates a new AnvilSpec from a specification definition and runs the
 * specification routine in discovery mode to collect all test definitions.
 *
 * @param alloc The allocator to use for memory management
 * @param def The specification definition containing the test routine
 * @return Fully initialized AnvilSpec with discovered tests
 */
AnvilSpec anvil_spec_create(Allocator* alloc, const AnvilSpecDef* def) {
    AnvilSpec spec = {
        .def = def,
        .tests = dynarray_create_t(alloc, AnvilTest, 64)
    };

    ContextDiscover ctx = {
        .api = {
            .visitTest = anvil_spec_test_discover
        },
        .spec = &spec
    };
    def->routine(&ctx.api);

    return spec;
}

/**
 * @brief Destroy a specification and free its resources
 *
 * Cleans up the test array and frees all resources associated
 * with the specification.
 *
 * @param spec The specification to destroy
 */
void anvil_spec_destroy(AnvilSpec* spec) {
    dynarray_destroy(&spec->tests);
}

/**
 * @brief Execute a specific test within a specification
 *
 * Executes a single test by ID within the given specification. Sets up
 * assertion handling, timing measurement, and error collection. Uses
 * setjmp/longjmp for early test termination.
 *
 * @param alloc The allocator to use for result memory management
 * @param spec The specification containing the test
 * @param id The ID of the test to execute
 * @return AnvilResult containing test execution results and timing
 */
AnvilResult* anvil_exec_test(Allocator* alloc, const AnvilSpec* spec, const AnvilTestId id) {
    AnvilResult* result = anvil_result_create(alloc);
    AnvilTestContext testCtx = { .result = result };
    const TimeSteady startTime = time_steady_clock();

    bool finished = setjmp(testCtx.finishJumpDest);

FinishedLabel:
    if (finished) {
        diag_set_assert_handler(null, null);
        
        const TimeSteady endTime = time_steady_clock();
        const TimeDuration duration = time_steady_duration(startTime, endTime);

        anvil_result_finish(result, duration);

        return result;
    }

    diag_set_assert_handler(anvil_assert_handler, &testCtx);

    ContextExec ctx = {
        .api = {
            .visitTest = anvil_spec_test_exec,
            .flags = AnvilSpecContextFlags_Setup | AnvilSpecContextFlags_Teardown
        },
        .testToExec = id,
        .testCtx = &testCtx
    };
    spec->def->routine(&ctx.api);
    diag_assert_msg(testCtx.started, "Unable to find a test with id: {}", fmt_int(id));

    finished = true;
    goto FinishedLabel;
}

/**
 * @brief Report an error during test execution
 *
 * Records an error message and source location in the test result.
 * Used by assertion handlers and other error reporting mechanisms.
 *
 * @param ctx The test context
 * @param msg The error message
 * @param source Source location where the error occurred
 */
void anvil_report_error(AnvilTestContext* ctx, String msg, const SourceLoc source) {
    diag_break();
    anvil_result_error(ctx->result, msg, source);
}

/**
 * @brief Terminate test execution early
 *
 * Performs a non-local jump to terminate the current test execution.
 * Used to exit tests early while ensuring proper cleanup and timing.
 *
 * @param ctx The test context containing the jump destination
 */
NORETURN void anvil_finish(AnvilTestContext* ctx) {
    longjmp(ctx->finishJumpDest, true);
}