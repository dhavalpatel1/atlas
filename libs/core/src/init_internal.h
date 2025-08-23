
#pragma once

#include "core_init.h"

void alloc_init();

void time_init();

void file_init();

void tty_init();

void path_init();

void thread_init();

void signal_init();

void alloc_init_thread();

void thread_init_thread();

void rng_init_thread();

void tty_teardown();

void alloc_teardown_thread();