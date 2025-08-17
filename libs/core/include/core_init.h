/**
 * @file core_init.h
 * @brief Core library initialization and cleanup functions
 *
 * This header provides initialization and teardown functions for the core library.
 * These functions should be called at the beginning and end of program execution
 * to properly set up and clean up global resources.
 */

#pragma once

/**
 * @brief Initialize the core library and its global resources
 *
 * This function must be called before using any other core library functions.
 * It sets up global allocators, threading primitives, and other system resources.
 */
void core_init();

/**
 * @brief Clean up core library resources and perform shutdown
 *
 * This function should be called at program exit to properly clean up
 * all global resources allocated by the core library.
 */
void core_teardown();