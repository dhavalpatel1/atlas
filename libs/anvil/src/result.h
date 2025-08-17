/**
 * @file result.h
 * @brief Test result tracking and error management for Anvil framework
 *
 * This header provides structures and functions for tracking test execution
 * results, including error collection, timing information, and test state
 * management within the Anvil testing framework.
 */

#pragma once

#include "core_dynarray.h"
#include "core_sourceloc.h"
#include "core_time.h"

/**
 * @brief Represents a single error that occurred during test execution
 *
 * Contains the error message and source location information for debugging
 * and reporting purposes.
 */
typedef struct {
    /** @brief Error message describing what went wrong */
    String msg;
    /** @brief Source location where the error occurred */
    SourceLoc source;
} AnvilError;

/**
 * @brief Tracks the result of a test execution
 *
 * Contains all information about a test run including execution state,
 * timing information, and any errors that occurred during execution.
 */
typedef struct {
    /** @brief Allocator used for dynamic memory management */
    Allocator* alloc;
    /** @brief Whether the test has finished execution */
    bool finished;
    /** @brief Total duration of test execution */
    TimeDuration duration;
    /** @brief Dynamic array of errors that occurred during execution */
    DynArray errors;
} AnvilResult;

/**
 * @brief Create a new test result tracker
 * @param alloc Allocator to use for memory management
 * @return Pointer to the newly created result tracker, or null on failure
 */
AnvilResult* anvil_result_create(Allocator*);

/**
 * @brief Destroy a test result tracker and free its resources
 * @param result The result tracker to destroy
 */
void anvil_result_destroy(AnvilResult*);

/**
 * @brief Record an error in the test result
 * @param result The result tracker to add the error to
 * @param msg Error message
 * @param source Source location where the error occurred
 */
void anvil_result_error(AnvilResult*, String msg, SourceLoc);

/**
 * @brief Mark the test as finished and record its duration
 * @param result The result tracker to finish
 * @param duration Total execution time for the test
 */
void anvil_result_finish(AnvilResult*, TimeDuration);