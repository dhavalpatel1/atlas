/**
 * @file core_rng.h
 * @brief Random number generation utilities
 *
 * This header provides random number generation functionality including
 * uniform and Gaussian distributions. It supports multiple RNG algorithms
 * and provides both global and per-instance random number generators.
 */

#pragma once

#include "core_annotation.h"
#include "core_types.h"

/**
 * @brief Generate a random number within a specified range
 * @param _RNG_ Random number generator to use
 * @param _MIN_ Minimum value (inclusive)
 * @param _MAX_ Maximum value (exclusive)
 * @return Random value between min and max
 */
#define rng_sample_range(_RNG_, _MIN_, _MAX_) ((_MIN_) + ((_MAX_) - (_MIN_)) * rng_sample_f32(_RNG_))

/** @brief Forward declaration of allocator structure */
typedef struct sAllocator Allocator;

/** @brief Opaque random number generator structure */
typedef struct sRng Rng;

/** @brief Thread-local global random number generator */
extern THREAD_LOCAL Rng* g_rng;

/**
 * @brief Generate a uniform random 32-bit float in [0, 1)
 * @param rng Random number generator to use
 * @return Random float value between 0.0 (inclusive) and 1.0 (exclusive)
 */
f32 rng_sample_f32(Rng* rng);

/**
 * @brief Pair of Gaussian-distributed random numbers
 */
typedef struct {
    f32 a;  /**< First Gaussian random number */
    f32 b;  /**< Second Gaussian random number */
} RngGaussPairF32;

/**
 * @brief Generate a pair of Gaussian-distributed random numbers
 * @param rng Random number generator to use
 * @return Pair of normally distributed random numbers with mean=0, stddev=1
 */
RngGaussPairF32 rng_sample_gauss_f32(Rng* rng);

/**
 * @brief Create a new XORWOW random number generator
 * @param allocator Allocator for RNG allocation
 * @param seed Seed value for the generator
 * @return Pointer to the created RNG, or null on failure
 */
Rng* rng_create_xorwow(Allocator* allocator, u64 seed);

/**
 * @brief Destroy a random number generator and free its resources
 * @param rng Random number generator to destroy
 */
void rng_destroy(Rng* rng);