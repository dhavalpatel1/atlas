#include "core_alloc.h"
#include "core_annotation.h"
#include "core_dynarray.h"
#include "core_dynstring.h"
#include "core_memory.h"

DynString dynstring_create(Allocator* allocator, usize capacity) {
    return dynarray_create(allocator, 1, 1, capacity);
}

DynString dynstring_create_over(Mem memory) {
    return dynarray_create_over(memory, 1u);
}

void dynstring_destroy(DynString* dynstr) {
    dynarray_destroy(dynstr);
}

FORCE_INLINE usize dynstring_size(const DynString* dynstr) {
    return dynarray_size(dynstr);
}

FORCE_INLINE String dynstring_view(const DynString* dynstr) {
    return dynarray_at(dynstr, 0, dynstr->size);
}

FORCE_INLINE void dynstring_clear(DynString* dynstr) {
    dynarray_clear(dynstr);
}

FORCE_INLINE void dynstring_append(DynString* dynstr, String val) {
    mem_cpy(dynarray_push(dynstr, val.size), val);
}

FORCE_INLINE void dynstring_append_char(DynString* dynstr, u8 val) {
    *dynarray_push_t(dynstr, u8) = val;
}

FORCE_INLINE void dynstring_append_chars(DynString *str, u8 c, usize amount) {
    mem_set(dynarray_push(str, amount), c);
}

FORCE_INLINE void dynstring_insert_chars(DynString *str, u8 val, usize idx, usize amount) {
    mem_set(dynarray_insert(str, idx, amount), val);
}

FORCE_INLINE String dynstring_push(DynString* str, usize amount) {
    return dynarray_push(str, amount);
}