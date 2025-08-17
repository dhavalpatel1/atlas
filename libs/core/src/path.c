/**
 * @file path.c
 * @brief Cross-platform file path manipulation utilities
 *
 * This file implements file path operations that work consistently across
 * different operating systems. It handles path normalization, absolute/relative
 * path detection, path joining, and working directory management with support
 * for both POSIX (/path/to/file) and Windows (C:\path\to\file) style paths.
 */

#include "core_array.h"
#include "core_ascii.h"
#include "core_bits.h"
#include "core_diag.h"
#include "core_dynarray.h"
#include "core_dynstring.h"
#include "core_memory.h"
#include "core_path.h"
#include "core_sentinel.h"
#include "core_string.h"
#include "init_internal.h"
#include "path_internal.h"

/** @brief Path separators for cross-platform compatibility */
static String g_path_seperators = string_static("/\\");

/**
 * @brief Check if a path string ends with a path separator
 * @param str The path string to check
 * @return true if the path ends with '/' or '\', false otherwise
 */
static bool path_ends_with_seperator(String str) {
    return mem_contains(g_path_seperators, *string_last(str));
}

/**
 * @brief Check if a path starts with a POSIX-style root (/)
 * @param path The path string to check
 * @return true if the path starts with '/', false otherwise
 */
static bool path_starts_with_posix_root(String path) {
    return !string_is_empty(path) && *string_begin(path) == '/';
}

/**
 * @brief Check if a path starts with a Windows-style root (C:\ or C:/)
 * @param path The path string to check
 * @return true if the path starts with a drive letter and colon, false otherwise
 */
static bool path_starts_with_win32_root(String path) {
    if (path.size < 3) {
        return false;
    }
    
    if (!ascii_is_letter(*string_begin(path))) {
        return false;
    }

    String postDriveLetter = string_slice(path, 1, 2);
    return string_eq(postDriveLetter, string_lit(":/")) || string_eq(postDriveLetter, string_lit(":\\"));
}

static u8 g_path_workingdir_buffer[path_pal_max_size];

String g_path_workingdir = {0};

static u8 g_path_executable_buffer[path_pal_max_size];

String g_path_executable = {0};

void path_init() {
    g_path_workingdir = path_pal_workingdir(array_mem(g_path_workingdir_buffer));
    g_path_executable = path_pal_executable(array_mem(g_path_executable_buffer));
}

bool path_is_absolute(String path) {
    return path_starts_with_posix_root(path) || path_starts_with_win32_root(path);
}

bool path_is_root(String path) {
    return (path.size == 1 && path_starts_with_posix_root(path)) || 
           (path.size == 3 && path_starts_with_win32_root(path));
}

String path_filename(String path) {
    const usize lastSegStart = string_find_last_any(path, g_path_seperators);

    return sentinel_check(lastSegStart) ? path : string_slice(path, lastSegStart + 1, path.size - lastSegStart - 1);
}

String path_extension(String path) {
    String filename = path_filename(path);
    const usize extensionStart = string_find_last_any(filename, string_lit("."));

    return sentinel_check(extensionStart) ? string_empty : string_slice(filename, extensionStart + 1, filename.size - extensionStart - 1);
}

String path_stem(String path) {
    String filename = path_filename(path);
    const usize extensionStart = string_find_first_any(filename, string_lit("."));
    
    return sentinel_check(extensionStart) ? filename : string_slice(filename, 0, extensionStart);
}

String path_parent(String path) {
    const usize lastSegStart = string_find_last_any(path, g_path_seperators);
    if (sentinel_check(lastSegStart)) {
        return string_empty;
    }

    String parentWithSep = string_slice(path, 0, lastSegStart + 1);

    return path_is_root(parentWithSep) ? parentWithSep : string_slice(path, 0, lastSegStart);
}

bool path_canonize(DynString *str, String path) {
    if (path_starts_with_posix_root(path)) {
        dynstring_append_char(str, '/');
        path = string_consume(path, 1);
    } else if (path_starts_with_win32_root(path)) {
        dynstring_append_char(str, ascii_to_upper(*string_begin(path)));
        dynstring_append(str, string_lit(":/"));
        path = string_consume(path, 3);
    }

    static usize maxSegments = 64;
    DynArray segStarts = dynarray_create_over_t(mem_stack(maxSegments * sizeof(usize)), usize);
    *dynarray_push_t(&segStarts, usize) = str->size;

    bool success = true;
    while (path.size) {
        const usize segEnd = string_find_first_any(path, g_path_seperators);
        String seg;
        if (sentinel_check(segEnd)) {
            seg = path;
            path = string_empty;
        } else {
            seg = string_slice(path, 0, segEnd);
            path = string_consume(path, segEnd + 1);
        }

        if (string_is_empty(seg) || string_eq(seg, string_lit("."))) {
            continue;
        }

        if (string_eq(seg, string_lit(".."))) {
            if (segStarts.size > 1) {
                str->size = *dynarray_at_t(&segStarts, segStarts.size - 1, usize);
                dynarray_pop(&segStarts, 1);
            }

            continue;
        }

        if (segStarts.size > 1 && !path_ends_with_seperator(dynstring_view(str))) {
            dynstring_append_char(str, '/');
        }

        *dynarray_push_t(&segStarts, usize) = str->size;
        if (segStarts.size == maxSegments) {
            success = false;
            break;
        }

        dynstring_append(str, seg);
    }

    dynarray_destroy(&segStarts);

    return success;
}

void path_append(DynString *str, String path) {
    if (str->size && !path_ends_with_seperator(dynstring_view(str))) {
        dynstring_append_char(str, '/');
    }

    dynstring_append(str, path);
}