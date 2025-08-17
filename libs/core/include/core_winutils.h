/**
 * @file core_winutils.h
 * @brief Windows-specific utilities for platform integration
 *
 * This header provides utilities specific to the Windows platform, primarily
 * focused on string conversion between UTF-8 and Windows wide character strings
 * (UTF-16). These functions are only available when building for Windows and
 * help bridge the gap between the library's UTF-8 string handling and Windows
 * APIs that expect wide character strings.
 */

#pragma once

#include "core_string.h"

#ifdef ATLAS_WIN32

/**
 * @brief Calculate the size needed for converting UTF-8 string to wide string
 * @param input UTF-8 string to convert
 * @return Number of bytes needed for the wide string conversion (including null terminator)
 */
usize winutils_to_widestr_size(String input);

/**
 * @brief Convert UTF-8 string to Windows wide character string
 * @param output Memory buffer to store the wide string
 * @param input UTF-8 string to convert
 * @return Number of bytes written to the output buffer
 */
usize winutils_to_widestr(Mem output, String input);

/**
 * @brief Calculate the size needed for converting wide string to UTF-8
 * @param input Wide character string to convert
 * @param inputCharCount Number of characters in the input wide string
 * @return Number of bytes needed for the UTF-8 conversion (including null terminator)
 */
usize winutils_from_widestr_size(void* input, usize inputCharCount);

/**
 * @brief Convert Windows wide character string to UTF-8 string
 * @param output String buffer to store the UTF-8 result
 * @param input Wide character string to convert
 * @param inputCharCount Number of characters in the input wide string
 * @return Number of bytes written to the output buffer
 */
usize winutils_from_widestr(String output, void* input, usize inputCharCount);

#endif
