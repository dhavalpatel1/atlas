
#pragma once

#include "core_memory.h"

typedef struct sAllocator Allocator;

typedef Mem String;

typedef enum {
    StringMatchFlags_None = 0,
    StringMatchFlags_IgnoreCase = 1 << 0,
} StringMatchFlags;

#define string_empty ((String){0})

#define string_is_empty(_STRING_) ((_STRING_).size == 0)

#define string_static(_LIT_) \
    { .ptr = (void*)(_LIT_), .size = sizeof(_LIT_) - 1u, }

#define string_lit(_LIT_) ((String)string_static(_LIT_))

#define string_at(_STRING_, _IDX_) ((u8*)(_STRING_).ptr + (_IDX_))

#define string_begin(_STRING_) ((u8*)(_STRING_).ptr)

#define string_end(_STRING_) ((u8*)(_STRING_).ptr + (_STRING_).size)

#define string_last(_STRING_) ((u8*)(_STRING_).ptr + (_STRING_).size - 1)

String string_from_null_term(const char* cstr);

String string_dup(Allocator* allocator, String string);

void string_free(Allocator* allocator, String string);

i8 string_cmp(String a, String b);

bool string_eq(String a, String b);

bool string_starts_with(String string, String start);

bool string_ends_with(String string, String end);

String string_slice(String string, usize offset, usize size);

String string_consume(String string, usize amount);

usize string_find_first(String string, String subStr);

usize string_find_first_any(String string, String chars);

usize string_find_last(String string, String subStr);

usize string_find_last_any(String string, String chars);

bool string_match_glob(String string, String pattern, StringMatchFlags flags);