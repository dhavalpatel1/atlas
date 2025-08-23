#include "anvil_spec.h"

#include "core_alloc.h"
#include "core_diag.h"
#include "core_time.h"

#include "spec_internal.h"

typedef struct {
    AnvilSpecContext api;
    AnvilSpec* spec;
} ContextDiscover;

typedef struct {
    AnvilSpecContext api;
    AnvilTestId testToExec;
    AnvilTestContext* testCtx;
} ContextExec;

static AnvilTestContext* anvil_spec_test_discover(AnvilSpecContext* ctx, AnvilTest test) {
    ContextDiscover* discoverCtx = (ContextDiscover*)ctx;

    discoverCtx->spec->focus |= test.flags & AnvilTestFlags_Focus;

    *dynarray_push_t(&discoverCtx->spec->tests, AnvilTest) = test;

    return null;
}

static AnvilTestContext* anvil_spec_test_exec(AnvilSpecContext* ctx, AnvilTest test) {
    ContextExec* execCtx = (ContextExec*)ctx;

    if (test.id != execCtx->testToExec) {
        return null;
    }

    execCtx->testCtx->started = true;

    return execCtx->testCtx;
}

static bool anvil_assert_handler(String msg, const SourceLoc source, void* context) {
    anvil_report_error(context, msg, source);

    return true;
}

bool anvil_visit_setup(AnvilSpecContext* ctx) {
    return (ctx->flags & AnvilSpecContextFlags_Setup) != 0;
}

bool anvil_visit_teardown(AnvilSpecContext* ctx) {
    return (ctx->flags & AnvilSpecContextFlags_Teardown) != 0;
}

AnvilTestContext* anvil_visit_test(AnvilSpecContext* ctx, const AnvilTest test) {
    return ctx->visitTest ? ctx->visitTest(ctx, test) : null;
}

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

void anvil_spec_destroy(AnvilSpec* spec) {
    dynarray_destroy(&spec->tests);
}

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

void anvil_report_error(AnvilTestContext* ctx, String msg, const SourceLoc source) {
    diag_break();
    anvil_result_error(ctx->result, msg, source);
}

NORETURN void anvil_finish(AnvilTestContext* ctx) {
    longjmp(ctx->finishJumpDest, true);
}