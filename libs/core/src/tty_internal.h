
#pragma once

#include "core_tty.h"

void tty_pal_init();

void tty_pal_teardown();

bool tty_pal_isatty(File* file);

u16 tty_pal_width(File* file);

u16 tty_pal_height(File* file);