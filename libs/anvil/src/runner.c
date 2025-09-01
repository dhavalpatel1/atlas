#include "core_alloc.h"
#include "core_array.h"
#include "core_file.h"
#include "core_thread.h"

#include "jobs_graph.h"
#include "jobs_scheduler.h"

#include "log_logger.h"

#include "anvil_runner.h"

#include "output_log.h"
#include "output_mocha.h"
#include "output_pretty.h"

#include "spec_internal.h"

typedef struct {
    AnvilOutput** outputs;
    usize outputsCount;
    i64 numFailedTests;
} AnvilRunContext;

typedef struct {
    const AnvilSpec* spec;
    const AnvilTest* test;
    AnvilRunContext* ctx;
} AnvilTestData;

static void anvil_test_task(void* context) {
    AnvilTestData* data = context;

    AnvilResult* result = anvil_exec_test(g_alloc_heap, data->spec, data->test->id);
    const AnvilResultType type = result->errors.size ? AnvilResultType_Fail : AnvilResultType_Pass;

    for (usize i = 0; i != data->ctx->outputsCount; ++i) {
        AnvilOutput* out = data->ctx->outputs[i];
        out->testFinished(out, data->spec, data->test, type, result);
    }

    if (type == AnvilResultType_Fail) {
        thread_atomic_add_i64(&data->ctx->numFailedTests, 1);
    }

    anvil_result_destroy(result);
}

AnvilResultType anvil_run(AnvilDef* check, const AnvilRunFlags flags) {
    const TimeSteady startTime = time_steady_clock();

    AnvilOutput* outputs[] = {
        anvil_output_pretty(g_alloc_heap, g_file_stdout, flags),
        anvil_output_mocha_default(g_alloc_heap),
        anvil_output_log(g_alloc_heap, g_logger)
    };

    AnvilRunContext ctx = {
        .outputs = outputs,
        .outputsCount = array_elems(outputs)
    };

    array_for_t(outputs, AnvilOutput*, out, {
        (*out)->runStarted(*out);
    });

    DynArray specs = dynarray_create_t(g_alloc_heap, AnvilSpec, 64);
    bool focus = false;
    usize numTests = 0;
    dynarray_for_t(&check->specs, AnvilSpecDef, specDef, {
        AnvilSpec spec = anvil_spec_create(g_alloc_heap, specDef);
        focus |= spec.focus;
        numTests += spec.tests.size;
        *dynarray_push_t(&specs, AnvilSpec) = spec;
    });

    const TimeDuration discoveryTime = time_steady_duration(startTime, time_steady_clock());
    array_for_t(outputs, AnvilOutput*, out, {
        (*out)->testDiscovered(*out, specs.size, numTests, discoveryTime);
    });

    JobGraph* graph = jobs_graph_create(g_alloc_heap, string_lit("tests"), numTests);
    usize numSkipped = 0;
    dynarray_for_t(&specs, AnvilSpec, spec, {
        dynarray_for_t(&spec->tests, AnvilTest, test, {
            if (test->flags & AnvilTestFlags_Skip || (focus && !(test->flags & AnvilTestFlags_Focus))) {
                ++numSkipped;

                array_for_t(outputs, AnvilOutput*, out, {
                    (*out)->testSkipped(*out, spec, test);
                });

                continue;
            }

            jobs_graph_add_task(graph, fmt_write_scratch("{}-{}", fmt_text(spec->def->name), fmt_int(test->id)), anvil_test_task, mem_struct(AnvilTestData, .spec = spec, .test = test, .ctx = &ctx));
        });
    });

    jobs_scheduler_wait_help(jobs_scheduler_run(graph));

    const usize numFailed = ctx.numFailedTests;
    const usize numPassed = numTests - numSkipped - numFailed;
    const AnvilResultType resultType= numFailed ? AnvilResultType_Fail : AnvilResultType_Pass;
    const TimeDuration runTime = time_steady_duration(startTime, time_steady_clock());

    array_for_t(outputs, AnvilOutput*, out, {
        (*out)->runFinished(*out, resultType, runTime, numPassed, numFailed, numSkipped);
    });

    jobs_graph_destroy(graph);
    dynarray_for_t(&specs, AnvilSpec, spec, {
        anvil_spec_destroy(spec);
    });
    dynarray_destroy(&specs);

    array_for_t(outputs, AnvilOutput*, out, {
        (*out)->destroy(*out);
    });

    return resultType;
}
