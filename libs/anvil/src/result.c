#include "core_alloc.h"
#include "core_diag.h"

#include "result.h"

AnvilResult* anvil_result_create(Allocator* alloc) {
    AnvilResult* result = alloc_alloc_t(alloc, AnvilResult);
    *result = (AnvilResult) {
        .alloc = alloc,
        .errors = dynarray_create_t(alloc, AnvilError, 0),
    };

    return result;
}

void anvil_result_destroy(AnvilResult* result) {
    dynarray_for_t(&result->errors, AnvilError, err, { string_free(result->alloc, err->msg); });
    dynarray_destroy(&result->errors);
    alloc_free_t(result->alloc, result);
}

void anvil_result_error(AnvilResult* result, String msg, const SourceLoc source) {
    diag_assert_msg(!result->finished, "Result is already finished");

    *dynarray_push_t(&result->errors, AnvilError) = (AnvilError) {
        .msg = string_dup(result->alloc, msg),
        .source = source,
    };
}

void anvil_result_finish(AnvilResult* result, TimeDuration duration) {
    diag_assert_msg(!result->finished, "Result is already finished");
    diag_assert_msg(duration >= 0, "Negative duration {} is not valid", fmt_duration(duration));

    result->finished = true;
    result->duration = duration;
}