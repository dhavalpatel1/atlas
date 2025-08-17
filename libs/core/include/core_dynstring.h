/**
 * @file core_dynstring.h
 * @brief Dynamic string implementation with automatic memory management
 *
 * This header provides a dynamic string implementation built on top of the
 * dynamic array system. It supports automatic memory management, string
 * concatenation, insertion, and various string manipulation operations.
 */

#pragma once

#include "core_dynarray.h"
#include "core_string.h"

/** @brief Dynamic string type alias for dynamic character array */
typedef DynArray DynString;

/**
 * @brief Create a new dynamic string with specified capacity
 * @param allocator Allocator to use for memory management
 * @param capacity Initial capacity for the string
 * @return New dynamic string
 */
DynString dynstring_create(Allocator* allocator, usize capacity);

/**
 * @brief Create a dynamic string over existing memory
 * @param memory Memory region to use for the string
 * @return Dynamic string using the provided memory
 */
DynString dynstring_create_over(Mem memory);

/**
 * @brief Destroy a dynamic string and free its resources
 * @param str Dynamic string to destroy
 */
void dynstring_destroy(DynString* str);

/**
 * @brief Get the current size (length) of a dynamic string
 * @param str Dynamic string to query
 * @return Size in characters
 */
usize dynstring_size(const DynString* str);

/**
 * @brief Get a string view of the dynamic string
 * @param str Dynamic string to view
 * @return String view of the dynamic string contents
 */
String dynstring_view(const DynString* str);

/**
 * @brief Clear the contents of a dynamic string
 * @param str Dynamic string to clear
 */
void dynstring_clear(DynString* str);

/**
 * @brief Append a string to the end of a dynamic string
 * @param str Dynamic string to append to
 * @param append String to append
 */
void dynstring_append(DynString* str, String append);

/**
 * @brief Append a single character to a dynamic string
 * @param str Dynamic string to append to
 * @param c Character to append
 */
void dynstring_append_char(DynString* str, u8 c);

/**
 * @brief Append multiple copies of a character to a dynamic string
 * @param str Dynamic string to append to
 * @param c Character to append
 * @param amount Number of times to append the character
 */
void dynstring_append_chars(DynString* str, u8 c, usize amount);

/**
 * @brief Insert multiple copies of a character at a specific position
 * @param str Dynamic string to insert into
 * @param val Character to insert
 * @param idx Index to insert at
 * @param amount Number of characters to insert
 */
void dynstring_insert_chars(DynString* str, u8 val, usize idx, usize amount);