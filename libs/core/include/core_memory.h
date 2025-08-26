
#pragma once

#include "core_types.h"

typedef struct {
    void* ptr;
    usize size;
} Mem;

#define mem_empty ((Mem){0})

#define mem_struct(_TYPE_, ...)          \
    ((Mem) {                             \
        .ptr = &(_TYPE_){ __VA_ARGS__ }, \
        .size = sizeof(_TYPE_)           \
    })

#define mem_create(_PTR_, _SIZE_) \
    ((Mem) {                      \
        .ptr = (void*)(_PTR_),    \
        .size = _SIZE_,           \
    })

#define mem_valid(_MEM_) ((_MEM_).ptr != null)

#define mem_begin(_MEM_) ((u8*)(_MEM_).ptr)

#define mem_end(_MEM_) ((u8*)(_MEM_).ptr + (_MEM_).size)

#define mem_at_u8(_MEM_, _IDX_) ((u8*)(_MEM_).ptr + (_IDX_))

#define mem_as_t(_MEM_, _TYPE_) ((_TYPE_*)mem_as(_MEM_, sizeof(_TYPE_)))

#define mem_for_u8(_MEM_, _VAR_, ...)                                                     \
{                                                                                         \
    const u8* _VAR_##_end = mem_end(_MEM_);                                               \
    for (u8* _VAR_##_itr = mem_begin(_MEM_); _VAR_##_itr != _VAR_##_end; ++_VAR_##_itr) { \
        const u8 _VAR_ = *_VAR_##_itr;                                                    \
        __VA_ARGS__                                                                       \
    }                                                                                     \
}                                                                                         \

#if defined(ATLAS_MSVC)
#define mem_stack(_SIZE_) mem_create(_alloca(_SIZE_), _SIZE_)
#else
#define mem_stack(_SIZE_) mem_create(__builtin_alloca(_SIZE_), _SIZE_)
#endif

void mem_set(Mem mem, u8 val);

void mem_cpy(Mem dst, Mem src);

void mem_move(Mem dst, Mem src);

Mem mem_slice(Mem mem, usize offset, usize size);

Mem mem_consume(Mem mem, usize amount);

void* mem_as(Mem mem, usize size);

i8 mem_cmp(Mem a, Mem b);

bool mem_eq(Mem a, Mem b);

bool mem_contains(Mem mem, u8 byte);

void mem_swap(Mem a, Mem b);

void mem_swap_raw(void* a, void* b, const u16 size);