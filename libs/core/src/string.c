/**
 * @file string.c
 * @brief Core string manipulation and searching utilities.
 *
 * This file provides fundamental string operations including creation, comparison,
 * searching, and pattern matching. It implements efficient string algorithms for
 * finding substrings, glob pattern matching, and various string utilities that
 * work with the String type. All functions perform bounds checking and validation.
 */

#include "core_ascii.h"
#include "core_diag.h"
#include "core_math.h"
#include "core_memory.h"
#include "core_sentinel.h"
#include "core_string.h"

#include <string.h>

/**
 * @brief Create a String from a null-terminated C string.
 *
 * This function creates a String structure from a null-terminated C string
 * by calculating its length and wrapping it in the String type.
 *
 * @param cstr The null-terminated C string
 * @return A String structure representing the same string data
 */
String string_from_null_term(const char *cstr) {
    return (String) {
        .ptr = (void*)cstr,
        .size = strlen(cstr)
    };
}

/**
 * @brief Duplicate a string using the specified allocator.
 *
 * This function creates a copy of the string by allocating new memory
 * and copying the string contents into it.
 *
 * @param alloc The allocator to use for the new string
 * @param str The string to duplicate
 * @return A new String containing a copy of the data
 */
String string_dup(Allocator* alloc, String str) {
    Mem mem = alloc_alloc(alloc, str.size, 1);
    mem_cpy(mem, str);

    return mem;
}

/**
 * @brief Free a string that was allocated with an allocator.
 *
 * This function frees the memory used by a string that was previously
 * allocated using string_dup or similar allocation functions.
 *
 * @param alloc The allocator that was used to allocate the string
 * @param str The string to free
 */
void string_free(Allocator* alloc, String str) {
    alloc_free(alloc, str);
}

/**
 * @brief Compare two strings lexicographically.
 *
 * This function compares two strings and returns the sign of the comparison
 * result. It compares up to the length of the shorter string.
 *
 * @param a The first string
 * @param b The second string
 * @return -1 if a < b, 0 if equal (up to min length), 1 if a > b
 */
i8 string_cmp(String a, String b) {
    return math_sign(strncmp((const char*)a.ptr, (const char*)b.ptr, math_min(a.size, b.size)));
}

/**
 * @brief Check if two strings are equal in length and content.
 *
 * This function checks if two strings have the same length and contain
 * identical byte sequences.
 *
 * @param a The first string
 * @param b The second string
 * @return true if the strings are equal, false otherwise
 */
bool string_eq(String a, String b) {
    return mem_eq(a, b);
}

/**
 * @brief Check if a string starts with a specific prefix.
 *
 * This function checks if the string begins with the specified prefix string.
 *
 * @param str The string to check
 * @param start The prefix to look for
 * @return true if str starts with start, false otherwise
 */
bool string_starts_with(String str, String start) {
    return str.size >= start.size && string_eq(string_slice(str, 0, start.size), start);
}

/**
 * @brief Check if a string ends with a specific suffix.
 *
 * This function checks if the string ends with the specified suffix string.
 *
 * @param str The string to check
 * @param end The suffix to look for
 * @return true if str ends with end, false otherwise
 */
bool string_ends_with(String str, String end) {
    return str.size >= end.size && string_eq(string_slice(str, str.size - end.size, end.size), end);
}

String string_slice(String str, usize offset, usize size) {
    return mem_slice(str, offset, size);
}

String string_consume(String str, usize amount) {
    return mem_consume(str, amount);
}

usize string_find_first(String str, String subStr) {
    // Iterate through each possible starting position
    for (u8* itr = mem_begin(str); itr <= string_end(str) - subStr.size; ++itr) {
        // Check if substring matches at current position
        if (mem_eq(mem_create(itr, subStr.size), subStr)) {
            return itr - string_begin(str);
        }
    }

    return sentinel_usize;
}

usize string_find_first_any(String str, String chars) {
    mem_for_u8(str, c, {
        if (mem_contains(chars, c)) {
            return c_itr - string_begin(str);
        }
    });

    return sentinel_usize;
}

usize string_find_last(String str, String subStr) {
    // Start from last possible position and work backwards
    for (u8* itr = mem_end(str) - subStr.size + 1; itr-- > string_begin(str);) {
        // Check if substring matches at current position
        if (mem_eq(mem_create(itr, subStr.size), subStr)) {
            return itr - string_begin(str);
        }
    }

    return sentinel_usize;
}

usize string_find_last_any(String str, String chars) {
    // Iterate backwards through the string
    for (u8* itr = mem_end(str); itr-- != mem_begin(str);) {
        // Check if current character exists in character set
        if (mem_contains(chars, *itr)) {
            return itr - string_begin(str);
        }
    }

    return sentinel_usize;
}

bool string_match_glob(String str, String pattern, StringMatchFlags flags) {
    usize patternIdx = 0;
    usize strIdx = 0;
    usize nextPatternIdx = 0;
    usize nextStrIdx = 0;

    while (patternIdx < pattern.size || strIdx < str.size) {
        if (patternIdx < pattern.size) {
            const u8 patternChar = *string_at(pattern, patternIdx);
            switch (patternChar) {
                case '?': {
                    if (strIdx < str.size) {
                        ++patternIdx;
                        ++strIdx;

                        continue;
                    }
                } break;

                case '*': {
                    nextPatternIdx = patternIdx++;
                    nextStrIdx = strIdx + 1;

                    continue;
                }

                default: {
                    if (strIdx < str.size && flags & StringMatchFlags_IgnoreCase ? ascii_to_lower(*string_at(str, strIdx)) == ascii_to_lower(patternChar) : *string_at(str, strIdx) == patternChar) {
                        ++patternIdx;
                        ++strIdx;

                        continue;
                    }
                } break;
            }

        }

        if (nextStrIdx && nextStrIdx <= str.size) {
            patternIdx = nextPatternIdx;
            strIdx = nextStrIdx;

            continue;
        }

        return false;
    }

    return true;
}