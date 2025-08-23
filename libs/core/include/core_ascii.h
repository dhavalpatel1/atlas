#pragma once

#include "core_types.h"

bool ascii_is_valid(u8 c);

bool ascii_is_digit(u8 c);

bool ascii_is_hex_digit(u8 c);

bool ascii_is_letter(u8 c);

bool ascii_is_lower(u8 c);

bool ascii_is_upper(u8 c);

bool ascii_is_control(u8 c);

bool ascii_is_whitespace(u8 c);

bool ascii_is_newline(u8 c);

bool ascii_is_printable(u8 c);

u8 ascii_toggle_case(u8 c);

u8 ascii_to_upper(u8 c);

u8 ascii_to_lower(u8 c);

u8 ascii_to_integer(u8 c);