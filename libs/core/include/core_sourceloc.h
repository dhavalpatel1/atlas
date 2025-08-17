/**
 * @file core_sourceloc.h
 * @brief Source location tracking utilities
 *
 * This header provides utilities for capturing and working with source code
 * location information including file names and line numbers. This is useful
 * for debugging, logging, and assertion systems.
 */

#pragma once

#include "core_string.h"

/**
 * @brief Structure containing source code location information
 */
typedef struct {
    String file;  /**< Source file name */
    u32 line;     /**< Line number in the source file */
} SourceLoc;

/**
 * @brief Get the current source file name as a string
 * @return String containing the current file name
 */
#define source_file() string_lit(__FILE__)

/**
 * @brief Get the current line number
 * @return Line number as u32
 */
#define source_line() ((u32)(__LINE__))

/**
 * @brief Create a SourceLoc structure for the current location
 * @return SourceLoc containing current file and line information
 */
#define source_location()      \
    ((SourceLoc) {             \
        .file = source_file(), \
        .line = source_line()  \
    })
