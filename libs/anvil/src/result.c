/**
 * @file result.c
 * @brief Implementation of Anvil test result management
 *
 * This file implements the functionality for creating, managing, and
 * destroying test results including error collection, timing, and
 * lifecycle management for the Anvil testing framework.
 */

#include "core_alloc.h"
#include "core_diag.h"

#include "result.h"

/**
 * @brief Create a new test result
 *
 * Allocates and initializes a new AnvilResult structure with an empty
 * error list using the provided allocator.
 *
 * @param alloc The allocator to use for memory management
 * @return Pointer to the newly created AnvilResult, or null on failure
 */
AnvilResult* anvil_result_create(Allocator* alloc) {
    AnvilResult* result = alloc_alloc_t(alloc, AnvilResult);
    *result = (AnvilResult) {
        .alloc = alloc,
        .errors = dynarray_create_t(alloc, AnvilError, 0),
    };

    return result;
}

/**
 * @brief Destroy a test result and free its resources
 *
 * Cleans up all error messages, destroys the error array, and frees
 * the AnvilResult structure itself.
 *
 * @param result The AnvilResult to destroy
 */
void anvil_result_destroy(AnvilResult* result) {
    dynarray_for_t(&result->errors, AnvilError, err, { string_free(result->alloc, err->msg); });
    dynarray_destroy(&result->errors);
    alloc_free_t(result->alloc, result);
}

/**
 * @brief Add an error to the test result
 *
 * Records an error message and source location in the result's error list.
 * The message is duplicated for storage. Cannot be called after the result
 * has been finished.
 *
 * @param result The test result to add the error to
 * @param msg The error message (will be duplicated)
 * @param source Source location where the error occurred
 */
void anvil_result_error(AnvilResult* result, String msg, const SourceLoc source) {
    diag_assert_msg(!result->finished, "Result is already finished");

    *dynarray_push_t(&result->errors, AnvilError) = (AnvilError) {
        .msg = string_dup(result->alloc, msg),
        .source = source,
    };
}

/**
 * @brief Mark the test result as finished and record execution time
 *
 * Finalizes the test result by setting the finished flag and recording
 * the execution duration. Cannot be called multiple times on the same result.
 *
 * @param result The test result to finish
 * @param duration The execution time for the test (must be non-negative)
 */
void anvil_result_finish(AnvilResult* result, TimeDuration duration) {
    diag_assert_msg(!result->finished, "Result is already finished");
    diag_assert_msg(duration >= 0, "Negative duration {} is not valid", fmt_duration(duration));

    result->finished = true;
    result->duration = duration;
}