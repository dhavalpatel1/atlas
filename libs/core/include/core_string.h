/**
 * @file core_string.h
 * @brief String manipulation and processing utilities
 *
 * This header provides string operations built on top of the memory abstraction.
 * Strings are represented as memory regions containing UTF-8 encoded text data,
 * with utilities for comparison, searching, and pattern matching.
 */

#pragma once

#include "core_memory.h"

/** @brief Forward declaration of allocator structure */
typedef struct sAllocator Allocator;

/** @brief String type alias for memory region containing text data */
typedef Mem String;

/**
 * @brief Flags for string matching operations
 */
typedef enum {
    StringMatchFlags_None = 0,                /**< No special matching flags */
    StringMatchFlags_IgnoreCase = 1 << 0,     /**< Perform case-insensitive matching */
} StringMatchFlags;

/** @brief Empty string constant */
#define string_empty ((String){0})

/**
 * @brief Check if a string is empty
 * @param _STRING_ String to check
 * @return true if the string has zero length, false otherwise
 */
#define string_is_empty(_STRING_) ((_STRING_).size == 0)

/**
 * @brief Create a static string structure from a string literal
 * @param _LIT_ String literal
 * @return String structure (not wrapped in parentheses for use in initializers)
 */
#define string_static(_LIT_) \
    { .ptr = (void*)(_LIT_), .size = sizeof(_LIT_) - 1u, }

/**
 * @brief Create a string from a string literal
 * @param _LIT_ String literal
 * @return String structure containing the literal
 */
#define string_lit(_LIT_) ((String)string_static(_LIT_))

/**
 * @brief Get pointer to a character at a specific index
 * @param _STRING_ String to index into
 * @param _IDX_ Index of the character
 * @return Pointer to the character at the specified index
 */
#define string_at(_STRING_, _IDX_) ((u8*)(_STRING_).ptr + (_IDX_))

/**
 * @brief Get pointer to the beginning of a string
 * @param _STRING_ String to get the beginning of
 * @return Pointer to the first character
 */
#define string_begin(_STRING_) ((u8*)(_STRING_).ptr)

/**
 * @brief Get pointer to one past the end of a string
 * @param _STRING_ String to get the end of
 * @return Pointer to one past the last character
 */
#define string_end(_STRING_) ((u8*)(_STRING_).ptr + (_STRING_).size)

/**
 * @brief Get pointer to the last character of a string
 * @param _STRING_ String to get the last character of
 * @return Pointer to the last character
 */
#define string_last(_STRING_) ((u8*)(_STRING_).ptr + (_STRING_).size - 1)

/**
 * @brief Create a string from a null-terminated C string
 * @param cstr Null-terminated C string
 * @return String structure containing the C string data
 */
String string_from_null_term(const char* cstr);

/**
 * @brief Duplicate a string using an allocator
 * @param allocator Allocator to use for memory allocation
 * @param string String to duplicate
 * @return New string containing a copy of the original data
 */
String string_dup(Allocator* allocator, String string);

/**
 * @brief Free a string allocated with an allocator
 * @param allocator Allocator used to allocate the string
 * @param string String to free
 */
void string_free(Allocator* allocator, String string);

/**
 * @brief Compare two strings lexicographically
 * @param a First string
 * @param b Second string
 * @return Negative if a < b, zero if equal, positive if a > b
 */
i8 string_cmp(String a, String b);

/**
 * @brief Check if two strings are equal
 * @param a First string
 * @param b Second string
 * @return true if the strings are equal, false otherwise
 */
bool string_eq(String a, String b);

/**
 * @brief Check if a string starts with a specific prefix
 * @param string String to check
 * @param start Prefix to look for
 * @return true if the string starts with the prefix, false otherwise
 */
bool string_starts_with(String string, String start);

/**
 * @brief Check if a string ends with a specific suffix
 * @param string String to check
 * @param end Suffix to look for
 * @return true if the string ends with the suffix, false otherwise
 */
bool string_ends_with(String string, String end);

/**
 * @brief Create a substring from a string
 * @param string Source string
 * @param offset Offset from the beginning of the string
 * @param size Length of the substring
 * @return New string representing the substring
 */
String string_slice(String string, usize offset, usize size);

/**
 * @brief Remove characters from the beginning of a string
 * @param string Source string
 * @param amount Number of characters to remove from the beginning
 * @return New string with the specified characters removed
 */
String string_consume(String string, usize amount);

/**
 * @brief Find the first occurrence of a substring
 * @param string String to search in
 * @param subStr Substring to search for
 * @return Index of the first occurrence, or usize_max if not found
 */
usize string_find_first(String string, String subStr);

/**
 * @brief Find the first occurrence of any character from a set
 * @param string String to search in
 * @param chars Set of characters to search for
 * @return Index of the first occurrence, or usize_max if not found
 */
usize string_find_first_any(String string, String chars);

/**
 * @brief Find the last occurrence of a substring
 * @param string String to search in
 * @param subStr Substring to search for
 * @return Index of the last occurrence, or usize_max if not found
 */
usize string_find_last(String string, String subStr);

/**
 * @brief Find the last occurrence of any character from a set
 * @param string String to search in
 * @param chars Set of characters to search for
 * @return Index of the last occurrence, or usize_max if not found
 */
usize string_find_last_any(String string, String chars);

/**
 * @brief Match a string against a glob pattern
 * @param string String to match
 * @param pattern Glob pattern (supports * and ? wildcards)
 * @param flags Matching flags (case sensitivity, etc.)
 * @return true if the string matches the pattern, false otherwise
 */
bool string_match_glob(String string, String pattern, StringMatchFlags flags);