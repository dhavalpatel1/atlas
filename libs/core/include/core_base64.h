/**
 * @file core_base64.h
 * @brief Base64 encoding and decoding utilities
 *
 * This header provides utilities for Base64 encoding and decoding operations.
 * Base64 is a binary-to-text encoding scheme that represents binary data in
 * an ASCII string format. It's commonly used for encoding binary data in
 * contexts where only text is supported, such as email or JSON.
 */

#pragma once

#include "core_dynstring.h"
#include "core_types.h"

/**
 * @brief Calculate the size of decoded data from Base64 encoded string
 * @param encoded Base64 encoded string
 * @return Size in bytes of the decoded data
 */
usize base64_decoded_size(String encoded);

/**
 * @brief Decode a Base64 encoded string into a dynamic string
 * @param str Dynamic string to store the decoded data
 * @param encoded Base64 encoded string to decode
 */
void base64_decode(DynString* str, String encoded);

/**
 * @brief Decode a Base64 encoded string using scratch allocator
 * @param encoded Base64 encoded string to decode
 * @return String containing the decoded data (allocated with scratch allocator)
 */
String base64_decode_scratch(String encoded);