/**
 * @file anvil_runner.h
 * @brief Anvil test framework runner and execution control
 *
 * This header provides the test execution engine for the Anvil testing framework,
 * including result types, execution flags, and the main test runner function.
 */

#pragma once

#include "anvil_def.h"

/**
 * @brief Enumeration of possible test execution results
 */
typedef enum {
    AnvilResultType_Pass = 0,  /**< All tests passed successfully */
    AnvilResultType_Fail = 1   /**< One or more tests failed */
} AnvilResultType;

/**
 * @brief Flags controlling test execution behavior
 */
typedef enum {
    AnvilRunFlags_None = 0,                      /**< Default execution behavior */
    AnvilRunFlags_OutputPassingTests = 1 << 0,  /**< Output information for passing tests */
} AnvilRunFlags;

/**
 * @brief Execute all registered test specifications
 *
 * Runs all test specifications that have been registered with the given
 * AnvilDef, applying the specified execution flags to control output behavior.
 *
 * @param def The AnvilDef containing registered test specifications
 * @param flags Flags controlling execution behavior
 * @return AnvilResultType_Pass if all tests pass, AnvilResultType_Fail otherwise
 */
AnvilResultType anvil_run(AnvilDef*, AnvilRunFlags);