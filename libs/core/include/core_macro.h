/**
 * @file core_macro.h
 * @brief Utility macros for variadic argument processing
 *
 * This header provides helper macros for working with variadic arguments
 * in C preprocessor macros, including counting arguments and manipulating
 * argument lists.
 */

#pragma once

/** @brief Internal helper macro to extract the 10th argument */
#define IMPL_GET_10TH_ARG(_1_, _2_, _3_, _4_, _5_, _6_, _7_, _8_, _9_, _10_, _11_, _N_, ...) _N_

/**
 * @brief Count the number of variadic arguments (up to 10)
 * @param ... Variadic arguments to count
 * @return Number of arguments as a compile-time constant
 */
#define COUNT_VA_ARGS(...) \
    IMPL_GET_10TH_ARG("ignored", ##__VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

/**
 * @brief Skip the first variadic argument and return the rest
 * @param _FIRST_ First argument to skip
 * @param ... Remaining arguments to return
 * @return All arguments except the first
 */
#define VA_ARGS_SKIP_FIRST(_FIRST_, ...) __VA_ARGS__