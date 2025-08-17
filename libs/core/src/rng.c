/**
 * @file rng.c
 * @brief Random number generator implementations and utilities
 *
 * This file implements multiple random number generator algorithms including
 * XorWow and SplitMix64, along with utilities for generating floating-point
 * random numbers and Gaussian distributions. The default implementation uses
 * thread-local storage for performance in multi-threaded environments.
 */

#include "core_alloc.h"
#include "core_diag.h"
#include "core_math.h"
#include "core_rng.h"
#include "core_time.h"

/**
 * @brief Virtual function table for random number generators
 *
 * This structure defines the interface that all RNG implementations must
 * provide, enabling polymorphic behavior for different generator types.
 */
struct sRng {
    /** @brief Generate the next random 32-bit unsigned integer */
    u32 (*next)(Rng*);
    /** @brief Clean up resources when the generator is destroyed */
    void (*destroy)(Rng*);
};

/**
 * @brief XorWow random number generator implementation
 *
 * XorWow is a fast, high-quality PRNG that combines xorshift operations
 * with a Weyl sequence. It has excellent statistical properties and good
 * performance characteristics for most applications.
 */
struct RngXorWow {
    /** @brief Base RNG interface */
    Rng api;
    /** @brief Internal state: 4 xorshift state words + 1 counter */
    u32 state[5];
    /** @brief Allocator used for this generator (for cleanup) */
    Allocator* alloc;
};

/**
 * @brief SplitMix64 generator for seeding other generators
 *
 * SplitMix64 is a fast, splittable pseudorandom number generator primarily
 * used for seeding other generators. It takes a 64-bit seed and produces
 * high-quality pseudorandom output suitable for initializing other PRNGs.
 *
 * @param state Pointer to the generator state (modified in-place)
 * @return A 64-bit pseudorandom number
 */
static u64 rng_splitmix64(u64* state) {
    // Add the golden ratio constant to advance the state
    u64 result = *state += 0x9E3779B97f4A7C15;
    
    // Apply mixing function to improve bit distribution
    result     = (result ^ (result >> 30)) * 0xBF58476D1CE4E5B9;
    result     = (result ^ (result >> 27)) * 0x94D049BB133111EB;

    // Final xor to complete the mixing
    return result ^ (result >> 31);
}

/**
 * @brief Initialize XorWow generator state from a 64-bit seed
 *
 * Uses SplitMix64 to convert the seed into high-quality initial state
 * for the XorWow generator. This ensures good distribution even when
 * seeds are sequential or otherwise poorly distributed.
 *
 * @param xorwow Pointer to the XorWow generator to initialize
 * @param seed 64-bit seed value (should be non-zero)
 */
static void rng_xorwow_init(struct RngXorWow* xorwow, u64 seed) {
    // Generate two 64-bit values from the seed using SplitMix64
    const u64 val1 = rng_splitmix64(&seed);
    const u64 val2 = rng_splitmix64(&seed);

    // Split the 64-bit values into 32-bit state components
    xorwow->state[0] = (u32)val1;           // Lower 32 bits of val1
    xorwow->state[1] = (u32)(val1 >> 32);   // Upper 32 bits of val1
    xorwow->state[2] = (u32)(val2);         // Lower 32 bits of val2
    xorwow->state[3] = (u32)(val2 >> 32);   // Upper 32 bits of val2
    xorwow->state[4] = 0;                   // Counter starts at zero
}

/**
 * @brief Destructor for XorWow generators
 *
 * Frees the memory allocated for a dynamically created XorWow generator.
 * 
 * @param rng Pointer to the RNG to destroy
 */
static void rng_xorwow_destroy(Rng* rng) {
    struct RngXorWow* rngXorWow = (struct RngXorWow*)rng;

    alloc_free_t(rngXorWow->alloc, rngXorWow);
}

/**
 * @brief Generate the next pseudorandom number using XorWow algorithm
 *
 * XorWow combines a 4-word xorshift generator with a Weyl sequence (counter).
 * The algorithm performs xorshift operations on a rotating state, then adds
 * the counter value to produce the final output. This provides excellent
 * statistical properties with good performance.
 *
 * @param rng Pointer to the XorWow generator
 * @return A 32-bit pseudorandom number
 */
static u32 rng_xorwow_next(Rng* rng) {
    struct RngXorWow* rngXorWow = (struct RngXorWow*)rng;

    // Verify that the state is properly initialized (non-zero for xorshift)
    diag_assert(rngXorWow->state[0]);
    diag_assert(rngXorWow->state[1]);
    diag_assert(rngXorWow->state[2]);
    diag_assert(rngXorWow->state[3]);

    u32* counter = &rngXorWow->state[4];
    u32 t = rngXorWow->state[3];           // Get the rightmost state word
    const u32 s = rngXorWow->state[0];     // Get the leftmost state word
    
    // Rotate the state array: shift all words left by one position
    rngXorWow->state[3] = rngXorWow->state[2];
    rngXorWow->state[2] = rngXorWow->state[1];
    rngXorWow->state[1] = s;

    // Apply XorShift operations to mix the bits
    t ^= t >> 2U;                          // Right shift and XOR
    t ^= t << 1U;                          // Left shift and XOR
    t ^= s ^ (s << 4U);                    // XOR with modified leftmost word
    
    // Store the new leftmost state word
    rngXorWow->state[0] = t;

    // Advance the Weyl sequence counter by a large odd number
    *counter += 362437U;

    // Combine xorshift output with counter for final result
    return t + *counter;
}

// Thread-local storage for default RNG instances
/** @brief Thread-local XorWow generator instance */
THREAD_LOCAL struct RngXorWow g_rng_xorwow = { .api =  { .next = rng_xorwow_next }};
/** @brief Thread-local pointer to the current default RNG */
THREAD_LOCAL Rng* g_rng;

/**
 * @brief Initialize the thread-local random number generator
 *
 * Sets up the default RNG for the current thread using the current time
 * as a seed. This should be called once per thread that needs random numbers.
 * The seed is automatically derived from the real-time clock to ensure
 * different threads get different sequences.
 */
void rng_init_thread() {
    // Seed with current time to get different sequences per thread
    rng_xorwow_init(&g_rng_xorwow, time_real_clock());
    g_rng = (Rng*)&g_rng_xorwow;
}

f32 rng_sample_f32(Rng* rng) {
    diag_assert_msg(rng, "rng next: Rng is not initialized");
    
    // Conversion factor: 1.0 / (2^32) to map [0, 2^32-1] to [0, 1)
    static const f32 toFloat = 1.0f / ((f32)u32_max + 1.0f);

    return rng->next(rng) * toFloat;
}

RngGaussPairF32 rng_sample_gauss_f32(Rng* rng) {
    f32 a, b;
    
    // Generate two uniform random numbers, avoiding values too close to 0
    // to prevent numerical issues in the logarithm
    do {
        a = rng_sample_f32(rng);
        b = rng_sample_f32(rng);
    } while (a <= 1e-8f);

    // Box-Muller transform: convert uniform [0,1) to normal(0,1)
    // sqrt(-2 * ln(a)) gives the magnitude
    // 2π * b gives the angle for the polar form
    const f32 magnitude = math_sqrt_f32(-2.0f * math_log_f32(a));
    const f32 angle = math_pi_f32 * 2.0f * b;
    
    return (RngGaussPairF32) {
        .a = magnitude * math_cos_f32(angle),  // First normal variate
        .b = magnitude * math_sin_f32(angle)   // Second normal variate
    };
}

Rng* rng_create_xorwow(Allocator* alloc, u64 seed) {
    diag_assert_msg(seed, "rng_create_xorwow(): 0 seed is invalid");
    
    // Allocate memory for the generator
    struct RngXorWow* rng = alloc_alloc_t(alloc, struct RngXorWow);
    
    // Set up the virtual function table
    rng->api = (Rng) {
        .next = rng_xorwow_next,
        .destroy = rng_xorwow_destroy
    };
    rng->alloc = alloc;

    // Initialize the generator state with the provided seed
    rng_xorwow_init(rng, seed);

    return (Rng*)rng;
}

void rng_destroy(Rng* rng) {
    diag_assert_msg(rng->destroy, "rng_destroy(): Given Rng cannot be destroyed");
    rng->destroy(rng);
}