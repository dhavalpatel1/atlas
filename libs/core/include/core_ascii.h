/**
 * @file core_ascii.h
 * @brief ASCII character classification and conversion utilities
 *
 * This header provides functions for classifying ASCII characters (digits, letters,
 * whitespace, etc.) and converting between upper and lower case. All functions
 * work with single byte ASCII characters.
 */

#pragma once

#include "core_types.h"

/**
 * @brief Check if a character is valid ASCII (0-127)
 * @param c Character to check
 * @return true if the character is valid ASCII, false otherwise
 */
bool ascii_is_valid(u8 c);

/**
 * @brief Check if a character is a decimal digit (0-9)
 * @param c Character to check
 * @return true if the character is a digit, false otherwise
 */
bool ascii_is_digit(u8 c);

/**
 * @brief Check if a character is a hexadecimal digit (0-9, A-F, a-f)
 * @param c Character to check
 * @return true if the character is a hex digit, false otherwise
 */
bool ascii_is_hex_digit(u8 c);

/**
 * @brief Check if a character is a letter (A-Z, a-z)
 * @param c Character to check
 * @return true if the character is a letter, false otherwise
 */
bool ascii_is_letter(u8 c);

/**
 * @brief Check if a character is lowercase (a-z)
 * @param c Character to check
 * @return true if the character is lowercase, false otherwise
 */
bool ascii_is_lower(u8 c);

/**
 * @brief Check if a character is uppercase (A-Z)
 * @param c Character to check
 * @return true if the character is uppercase, false otherwise
 */
bool ascii_is_upper(u8 c);

/**
 * @brief Check if a character is a control character (0-31, 127)
 * @param c Character to check
 * @return true if the character is a control character, false otherwise
 */
bool ascii_is_control(u8 c);

/**
 * @brief Check if a character is whitespace (space, tab, etc.)
 * @param c Character to check
 * @return true if the character is whitespace, false otherwise
 */
bool ascii_is_whitespace(u8 c);

/**
 * @brief Check if a character is a newline (\n or \r)
 * @param c Character to check
 * @return true if the character is a newline, false otherwise
 */
bool ascii_is_newline(u8 c);

/**
 * @brief Check if a character is printable (32-126)
 * @param c Character to check
 * @return true if the character is printable, false otherwise
 */
bool ascii_is_printable(u8 c);

/**
 * @brief Toggle the case of a letter (upper <-> lower)
 * @param c Character to toggle
 * @return Toggled character, or original if not a letter
 */
u8 ascii_toggle_case(u8 c);

/**
 * @brief Convert a character to uppercase
 * @param c Character to convert
 * @return Uppercase character, or original if not a letter
 */
u8 ascii_to_upper(u8 c);

/**
 * @brief Convert a character to lowercase
 * @param c Character to convert
 * @return Lowercase character, or original if not a letter
 */
u8 ascii_to_lower(u8 c);

u8 ascii_to_integer(u8 c);