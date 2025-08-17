/**
 * @file anvil_def.h
 * @brief Anvil test framework definition and registration functions
 *
 * This header provides the core definition structure and functions for the Anvil
 * testing framework, including test suite creation, destruction, and spec registration.
 */

#pragma once

#include "anvil_spec.h"

/**
 * @brief Opaque structure representing an Anvil test definition
 *
 * This structure holds the state and configuration for an Anvil test suite.
 * It should only be accessed through the provided API functions.
 */
typedef struct sAnvilDef AnvilDef;

/**
 * @brief Function pointer type for test specification routines
 *
 * @param context The test specification context to execute within
 */
typedef void (*AnvilSpecRoutine)(AnvilSpecContext*);

/**
 * @brief Macro to register a test specification with the framework
 *
 * This macro automatically declares the specification function and registers it
 * with the given context using the provided name.
 *
 * @param _CTX_ The AnvilDef context to register the spec with
 * @param _NAME_ The name of the specification (used for function naming)
 */
#define register_spec(_CTX_, _NAME_)                                    \
    void spec_name(_NAME_)(AnvilSpecContext*);                          \
    anvil_register_spec(_CTX_, string_lit(#_NAME_), &spec_name(_NAME_))

/**
 * @brief Create a new Anvil test definition
 *
 * @param allocator The allocator to use for memory management
 * @return Pointer to the newly created AnvilDef, or null on failure
 */
AnvilDef* anvil_create(Allocator*);

/**
 * @brief Destroy an Anvil test definition and free its resources
 *
 * @param def The AnvilDef to destroy
 */
void anvil_destroy(AnvilDef*);

/**
 * @brief Register a test specification with the framework
 *
 * @param def The AnvilDef to register the spec with
 * @param name The name of the specification
 * @param routine The function pointer to the specification routine
 */
void anvil_register_spec(AnvilDef*, String name, AnvilSpecRoutine);