/**
 * @file path_internal.h
 * @brief Internal path manipulation platform abstraction layer interface
 *
 * This header defines the platform abstraction layer interface for path
 * operations including working directory and executable path retrieval.
 * Platform-specific implementations provide the actual functionality.
 */

#pragma once

#include "core_string.h"

#define path_pal_max_size 512

String path_pal_workingdir(Mem outputBuffer);

String path_pal_executable(Mem outputBuffer);