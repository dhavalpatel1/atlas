#include "core_diag.h"
#include "core_math.h"
#include "core_memory.h"

#include <string.h>

FORCE_INLINE void mem_set(const Mem mem, const u8 val) {
    diag_assert(mem_valid(mem));

    memset(mem.ptr, val, mem.size);
}

FORCE_INLINE void mem_cpy(const Mem dst, const Mem src) {
    diag_assert(mem_valid(dst));
    diag_assert(!src.size || mem_valid(src));
    diag_assert(dst.size >= src.size);

    memcpy(dst.ptr, src.ptr, src.size);
}

FORCE_INLINE void mem_move(const Mem dst, const Mem src) {
    diag_assert(mem_valid(dst));
    diag_assert(mem_valid(src));
    diag_assert(dst.size >= src.size);

    memmove(dst.ptr, src.ptr, src.size);
}

FORCE_INLINE Mem mem_slice(Mem mem, const usize offset, const usize size) {
    diag_assert(!size || mem_valid(mem));
    diag_assert(mem.size >= offset + size);

    return mem_create((u8*)mem.ptr + offset, size);
}

FORCE_INLINE Mem mem_consume(Mem mem, usize amount) {
    diag_assert(mem.size >= amount);

    return (Mem) {
        .ptr = (u8*)mem.ptr + amount,
        .size = mem.size - amount,
    };
}

FORCE_INLINE void* mem_as(Mem mem, const usize size) {
    diag_assert(mem_valid(mem));
    diag_assert(mem.size >= size);

    return mem.ptr;
}

FORCE_INLINE i8 mem_cmp(const Mem a, const Mem b) {
    diag_assert(mem_valid(a));
    diag_assert(mem_valid(b));

    const int cmp = memcmp(a.ptr, b.ptr, math_min(a.size, b.size));

    return math_sign(cmp);
}

FORCE_INLINE bool mem_eq(Mem a, Mem b) {
    diag_assert(!a.size || mem_valid(a));
    diag_assert(!b.size || mem_valid(b));

    return a.size == b.size && memcmp(a.ptr, b.ptr, a.size) == 0;
}

bool mem_contains(Mem mem, const u8 byte) {
    mem_for_u8(mem, b, {
        if (b == byte) {
            return true;
        }
    });

    return false;
}

void mem_swap(Mem a, Mem b) {
    diag_assert(mem_valid(a));
    diag_assert(mem_valid(b));
    diag_assert(a.size == b.size);

    mem_swap_raw(a.ptr, b.ptr, (u16)a.size);
}

void mem_swap_raw(void *a, void *b, const u16 size) {
    diag_assert(size <= 1024);

    Mem buffer = mem_stack(size);
    memcpy(buffer.ptr, a, size);
    memcpy(a, b, size);
    memcpy(b, buffer.ptr, size);
}
