/**
 * @file output.h
 * @brief Output interface for Anvil test framework results
 *
 * This header defines the output interface for the Anvil testing framework,
 * providing a polymorphic way to handle test results and events during
 * test execution. Output implementations can format and display test results
 * in various ways (console, files, XML, JSON, etc.).
 */

#pragma once

#include "anvil_runner.h"

#include "spec_internal.h"

/**
 * @brief Opaque structure representing an Anvil output handler
 *
 * This structure provides a polymorphic interface for handling test output
 * events. Different implementations can provide different formatting and
 * destination options for test results.
 */
typedef struct sAnvilOutput AnvilOutput;

struct sAnvilOutput {
    /** @brief Called when a test run begins */
    void (*runStarted)(AnvilOutput*);

    /** @brief Called when tests are discovered
     * @param output The output handler
     * @param count Number of tests discovered
     * @param duration Time taken for discovery
     */
    void (*testDiscovered)(AnvilOutput*, usize count, TimeDuration);

    /** @brief Called when an individual test completes
     * @param output The output handler
     * @param spec The test specification
     * @param test The individual test
     * @param type The result type (passed/failed/skipped)
     * @param result The detailed test result
     */
    void (*testFinished)(AnvilOutput*, const AnvilSpec*, const AnvilTest*, AnvilResultType, AnvilResult*);

    /** @brief Called when the entire test run completes
     * @param output The output handler
     * @param type Overall result type
     * @param duration Total execution time
     * @param numPassed Number of tests that passed
     * @param numFailed Number of tests that failed
     * @param numSkipped Number of tests that were skipped
     */
    void (*runFinished)(AnvilOutput*, AnvilResultType, TimeDuration, usize numPassed, usize numFailed, usize numSkipped);

    /** @brief Cleanup function for the output handler */
    void (*destroy)(AnvilOutput*);
};

/**
 * @brief Destroy an Anvil output handler and free its resources
 * @param output The output handler to destroy
 */
void anvil_output_destroy(AnvilOutput*);