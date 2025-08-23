#pragma once

#include "core_dynstring.h"
#include "core_types.h"

usize base64_decoded_size(String encoded);

void base64_decode(DynString* str, String encoded);

String base64_decode_scratch(String encoded);