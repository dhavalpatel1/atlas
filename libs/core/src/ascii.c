/**
 * @file ascii.c
 * @brief ASCII character classification and conversion utilities.
 *
 * This file provides a comprehensive set of functions for classifying and
 * converting ASCII characters. It includes functions for checking character
 * types (digits, letters, whitespace, etc.), case conversion, and converting
 * characters to numeric values. All functions are designed to work efficiently
 * with single ASCII characters and include proper validation.
 */

#include "core_annotation.h"
#include "core_ascii.h"
#include "core_sentinel.h"


/**
 * @brief Check if a character is a valid ASCII character.
 *
 * @param c The character to check
 * @return true if the character is in the ASCII range (0-127), false otherwise
 */
FORCE_INLINE bool ascii_is_valid(const u8 c) {
    return (c & 0b10000000) == 0;
}

/**
 * @brief Check if a character is a decimal digit (0-9).
 *
 * @param c The character to check
 * @return true if the character is a digit, false otherwise
 */
FORCE_INLINE bool ascii_is_digit(const u8 c) {
    return c >= '0' && c <= '9';
}

/**
 * @brief Check if a character is a hexadecimal digit (0-9, a-f, A-F).
 *
 * @param c The character to check
 * @return true if the character is a hex digit, false otherwise
 */
FORCE_INLINE bool ascii_is_hex_digit(const u8 c) {
    return ascii_is_digit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

FORCE_INLINE bool ascii_is_letter(const u8 c) {
    return ascii_is_lower(c) || ascii_is_upper(c);
}

FORCE_INLINE bool ascii_is_lower(const u8 c) {
    return c >= 'a' && c <= 'z';
}

FORCE_INLINE bool ascii_is_upper(const u8 c) {
    return c >= 'A' && c <= 'Z';
}

FORCE_INLINE bool ascii_is_control(const u8 c) {
    return c <= 0x1f || c == 0x7f;
}

FORCE_INLINE bool ascii_is_whitespace(const u8 c) {
    return c == ' ' || c == '\t' || (c >= 0x0A && c <= 0x0D);
}

FORCE_INLINE bool ascii_is_newline(const u8 c) {
    return c == '\n' || c == '\r';
}

FORCE_INLINE bool ascii_is_printable(const u8 c) {
    return !ascii_is_control(c) && c < 127;
}

FORCE_INLINE u8 ascii_toggle_case(const u8 c) {
    return c ^ 0x20;
}

/**
 * @brief Convert a character to uppercase.
 *
 * @param c The character to convert
 * @return The uppercase version of the character, or the original if not a letter
 */
FORCE_INLINE u8 ascii_to_upper(const u8 c) {
    return ascii_is_lower(c) ? ascii_toggle_case(c) : c;
}

/**
 * @brief Convert a character to lowercase.
 *
 * @param c The character to convert
 * @return The lowercase version of the character, or the original if not a letter
 */
FORCE_INLINE u8 ascii_to_lower(const u8 c) {
    return ascii_is_upper(c) ? ascii_toggle_case(c) : c;
}

/**
 * @brief Convert a character to its numeric value.
 *
 * This function converts hex digits (0-9, a-f, A-F) to their numeric values.
 *
 * @param c The character to convert
 * @return The numeric value (0-15), or sentinel_u8 if not a hex digit
 */
FORCE_INLINE u8 ascii_to_integer(const u8 c) {
    if (ascii_is_digit(c)) {
        return c - '0';
    }

    if (c >= 'a' && c <= 'f') {
        return c - ('a' - 10);
    }

    if (c >= 'A' && c <= 'F') {
        return c - ('A' - 10);
    }

    return sentinel_u8;
}