#pragma once

#include "core_memory.h"

#define array_elems(_ARRAY_) (sizeof(_ARRAY_) / sizeof((_ARRAY_)[0]))

#define array_mem(_ARRAY_) mem_create((void*)(_ARRAY_), sizeof(_ARRAY_))

#define array_for_t(_ARRAY_, _TYPE_, _VAR_, ...)              \
{                                                             \
    _TYPE_* _VAR_ = (_TYPE_*)(_ARRAY_);                                \
    _TYPE_* _VAR_##_end = (_TYPE_*)_VAR_ + array_elems(_ARRAY_); \
    for (; _VAR_ != _VAR_##_end; ++_VAR_) {                   \
        __VA_ARGS__                                           \
    }                                                         \
}
