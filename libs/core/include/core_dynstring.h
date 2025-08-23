
#pragma once

#include "core_dynarray.h"
#include "core_string.h"

typedef DynArray DynString;

DynString dynstring_create(Allocator* allocator, usize capacity);

DynString dynstring_create_over(Mem memory);

void dynstring_destroy(DynString* str);

usize dynstring_size(const DynString* str);

String dynstring_view(const DynString* str);

void dynstring_clear(DynString* str);

void dynstring_append(DynString* str, String append);

void dynstring_append_char(DynString* str, u8 c);

void dynstring_append_chars(DynString* str, u8 c, usize amount);

void dynstring_insert_chars(DynString* str, u8 val, usize idx, usize amount);