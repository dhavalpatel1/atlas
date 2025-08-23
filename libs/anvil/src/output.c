#include "output.h"

void anvil_output_destroy(AnvilOutput* out) {
    out->destroy(out);
}