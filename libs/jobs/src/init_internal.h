/**
 * @file init_internal.h
 * @brief Internal jobs system initialization interface
 *
 * This header defines the internal initialization and teardown functions
 * for the job system components including scheduler and executor subsystems.
 */

#pragma once

#include "jobs_init.h"

void scheduler_init();

void executor_init();

void scheduler_teardown();

void executor_teardown();