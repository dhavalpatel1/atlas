/**
 * @file def.c
 * @brief Implementation of Anvil test definition management
 *
 * This file implements the core functionality for creating, destroying, and
 * managing Anvil test definitions including specification registration.
 */

#include "core_alloc.h"

#include "def_internal.h"

/**
 * @brief Create a new Anvil test definition
 *
 * Allocates and initializes a new AnvilDef structure with an empty
 * specification list using the provided allocator.
 *
 * @param alloc The allocator to use for memory management
 * @return Pointer to the newly created AnvilDef, or null on failure
 */
AnvilDef* anvil_create(Allocator* alloc) {
    AnvilDef* ctx = alloc_alloc_t(alloc, AnvilDef);
    *ctx = (AnvilDef) {
        .specs = dynarray_create_t(alloc, AnvilSpecDef, 64),
        .alloc = alloc
    };

    return ctx;
}

/**
 * @brief Destroy an Anvil test definition and free its resources
 *
 * Cleans up all registered specifications, frees their names, destroys
 * the specification array, and frees the AnvilDef structure itself.
 *
 * @param ctx The AnvilDef to destroy
 */
void anvil_destroy(AnvilDef* ctx) {
    dynarray_for_t(&ctx->specs, AnvilSpecDef, spec, { string_free(ctx->alloc, spec->name); });

    dynarray_destroy(&ctx->specs);

    alloc_free_t(ctx->alloc, ctx);
}

/**
 * @brief Register a test specification with the framework
 *
 * Adds a new specification to the AnvilDef's list of registered specifications.
 * The specification name is duplicated for storage.
 *
 * @param ctx The AnvilDef to register the spec with
 * @param name The name of the specification (will be duplicated)
 * @param routine The function pointer to the specification routine
 */
void anvil_register_spec(AnvilDef* ctx, String name, AnvilSpecRoutine routine) {
    *dynarray_push_t(&ctx->specs, AnvilSpecDef) = (AnvilSpecDef) {
        .name = string_dup(ctx->alloc, name),
        .routine = routine
    };
}