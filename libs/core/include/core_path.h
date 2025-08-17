/**
 * @file core_path.h
 * @brief Path manipulation utilities for file system operations
 *
 * This header provides utilities for manipulating file system paths including
 * path parsing, canonicalization, and construction. It supports both absolute
 * and relative paths and provides cross-platform path operations.
 */

#pragma once

#include "core_dynstring.h"

/** @brief Global string containing the current working directory path */
extern String g_path_workingdir;

/** @brief Global string containing the path to the current executable */
extern String g_path_executable;

/**
 * @brief Check if a path is absolute
 * @param path Path string to check
 * @return true if the path is absolute, false if relative
 */
bool path_is_absolute(String path);

/**
 * @brief Check if a path represents the root directory
 * @param path Path string to check
 * @return true if the path is the root directory, false otherwise
 */
bool path_is_root(String path);

/**
 * @brief Extract the filename component from a path
 * @param path Path string to extract filename from
 * @return String containing the filename (including extension)
 */
String path_filename(String path);

/**
 * @brief Extract the file extension from a path
 * @param path Path string to extract extension from
 * @return String containing the file extension (including the dot)
 */
String path_extension(String path);

/**
 * @brief Extract the stem (filename without extension) from a path
 * @param path Path string to extract stem from
 * @return String containing the filename without extension
 */
String path_stem(String path);

/**
 * @brief Extract the parent directory from a path
 * @param path Path string to extract parent from
 * @return String containing the parent directory path
 */
String path_parent(String path);

/**
 * @brief Canonicalize a path by resolving relative components
 * @param result Dynamic string to store the canonicalized path
 * @param path Path string to canonicalize
 * @return true if canonicalization succeeded, false on error
 */
bool path_canonize(DynString* result, String path);

/**
 * @brief Append a path component to a base path
 * @param basePath Dynamic string containing the base path to append to
 * @param path Path component to append
 */
void path_append(DynString* basePath, String path);
