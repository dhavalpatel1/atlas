/**
 * @file tty_internal.h
 * @brief Internal terminal/TTY platform abstraction layer interface
 *
 * This header defines the platform abstraction layer interface for terminal
 * operations including TTY detection, terminal size queries, and terminal
 * setup/teardown operations across different platforms.
 */

#pragma once

#include "core_tty.h"

void tty_pal_init();

void tty_pal_teardown();

bool tty_pal_isatty(File* file);

u16 tty_pal_width(File* file);

u16 tty_pal_height(File* file);