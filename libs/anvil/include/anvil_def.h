#pragma once

#include "anvil_spec.h"

typedef struct sAnvilDef AnvilDef;

typedef void (*AnvilSpecRoutine)(AnvilSpecContext*);

#define register_spec(_CTX_, _NAME_)                                    \
    void spec_name(_NAME_)(AnvilSpecContext*);                          \
    anvil_register_spec(_CTX_, string_lit(#_NAME_), &spec_name(_NAME_))

AnvilDef* anvil_create(Allocator*);

void anvil_destroy(AnvilDef*);

void anvil_register_spec(AnvilDef*, String name, AnvilSpecRoutine);