/**
 * @file output.c
 * @brief Implementation of Anvil output interface utilities
 *
 * This file provides utility functions for managing Anvil output handlers
 * that format and display test execution results.
 */

#include "output.h"

/**
 * @brief Destroy an output handler and free its resources
 *
 * Calls the output handler's destroy function to clean up all
 * associated resources and memory.
 *
 * @param out The output handler to destroy
 */
void anvil_output_destroy(AnvilOutput* out) {
    out->destroy(out);
}