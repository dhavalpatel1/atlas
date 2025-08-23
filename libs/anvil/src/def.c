#include "core_alloc.h"

#include "def_internal.h"

AnvilDef* anvil_create(Allocator* alloc) {
    AnvilDef* ctx = alloc_alloc_t(alloc, AnvilDef);
    *ctx = (AnvilDef) {
        .specs = dynarray_create_t(alloc, AnvilSpecDef, 64),
        .alloc = alloc
    };

    return ctx;
}

void anvil_destroy(AnvilDef* ctx) {
    dynarray_for_t(&ctx->specs, AnvilSpecDef, spec, { string_free(ctx->alloc, spec->name); });

    dynarray_destroy(&ctx->specs);

    alloc_free_t(ctx->alloc, ctx);
}

void anvil_register_spec(AnvilDef* ctx, String name, AnvilSpecRoutine routine) {
    *dynarray_push_t(&ctx->specs, AnvilSpecDef) = (AnvilSpecDef) {
        .name = string_dup(ctx->alloc, name),
        .routine = routine
    };
}