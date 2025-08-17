/**
 * @file core_utf8.h
 * @brief UTF-8 string utilities for Unicode text processing
 *
 * This header provides utilities for working with UTF-8 encoded text strings.
 * UTF-8 is a variable-width encoding where characters can be represented by
 * 1-4 bytes. These utilities help with proper UTF-8 text processing including
 * character counting and validation.
 */

#pragma once 

#include "core_string.h"

/**
 * @brief Check if a byte is a UTF-8 continuation byte
 * @param byte Byte to check
 * @return true if the byte is a UTF-8 continuation byte (10xxxxxx pattern), false otherwise
 */
bool utf8_contchar(u8 byte);

/**
 * @brief Count the number of Unicode code points in a UTF-8 string
 * @param text UTF-8 encoded string to count
 * @return Number of Unicode code points (characters) in the string
 */
usize utf8_cp_count(String text);