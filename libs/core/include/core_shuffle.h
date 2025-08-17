/**
 * @file core_shuffle.h
 * @brief Shuffling algorithms for randomizing data collections
 *
 * This header provides algorithms for shuffling arrays and other data collections
 * using cryptographically secure random number generation. The primary algorithm
 * implemented is the Fisher-Yates shuffle, which provides uniform random permutations
 * of input data.
 */

#pragma once

#include "core_rng.h"

/**
 * @brief Perform Fisher-Yates shuffle on an array of elements
 * @param rng Random number generator to use for shuffling
 * @param begin Pointer to the first element of the array
 * @param end Pointer past the last element of the array
 * @param stride Size in bytes of each element in the array
 */
void shuffle_fisheryates(Rng* rng, u8* begin, u8* end, u16 stride);