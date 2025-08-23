#pragma once

#include "core_annotation.h"
#include "core_types.h"

#define rng_sample_range(_RNG_, _MIN_, _MAX_) ((_MIN_) + ((_MAX_) - (_MIN_)) * rng_sample_f32(_RNG_))

typedef struct sAllocator Allocator;

typedef struct sRng Rng;

extern THREAD_LOCAL Rng* g_rng;

f32 rng_sample_f32(Rng* rng);

typedef struct {
    f32 a;
    f32 b;
} RngGaussPairF32;

RngGaussPairF32 rng_sample_gauss_f32(Rng* rng);

Rng* rng_create_xorwow(Allocator* allocator, u64 seed);

void rng_destroy(Rng* rng);