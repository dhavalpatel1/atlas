#pragma once

#include "core_types.h"

typedef i8 (*CompareFunc)(const void* a, const void* b);

#define COMPARE_DECLARE_WITH_NAME(_TYPE_, _NAME_)   \
    i8 compare_##_NAME_(const void* a, const void* b); \
    i8 compare_##_NAME_##_reverse(const void* a, const void* b);

#define COMPARE_DECLARE(_TYPE_) COMPARE_DECLARE_WITH_NAME(_TYPE_, _TYPE_)

COMPARE_DECLARE(i8)
COMPARE_DECLARE(i16)
COMPARE_DECLARE(i32)
COMPARE_DECLARE(i64)

COMPARE_DECLARE(u8)
COMPARE_DECLARE(u16)
COMPARE_DECLARE(u32)
COMPARE_DECLARE(u64)

COMPARE_DECLARE(usize)

COMPARE_DECLARE(f32)
COMPARE_DECLARE(f64)

COMPARE_DECLARE_WITH_NAME(String, string)
