#include "core_file.h"

#include "output.h"

AnvilOutput* anvil_output_mocha(Allocator*, File*);

AnvilOutput* anvil_output_mocha_to_path(Allocator*, String path);

AnvilOutput* anvil_output_mocha_default(Allocator*);
