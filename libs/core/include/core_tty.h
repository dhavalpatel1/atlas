#pragma once

#include "core_file.h"
#include "core_types.h"

PACKED(typedef enum {
    TtyFgColor_None          = 0,
    TtyFgColor_Default       = 39,
    TtyFgColor_Black         = 30,
    TtyFgColor_Red           = 31,
    TtyFgColor_Green         = 32,
    TtyFgColor_Yellow        = 33,
    TtyFgColor_Blue          = 34,
    TtyFgColor_Magenta       = 35,
    TtyFgColor_Cyan          = 36,
    TtyFgColor_White         = 37,
    TtyFgColor_Bright        = 37,
    TtyFgColor_BrightBlack   = 90,
    TtyFgColor_BrightRed     = 91,
    TtyFgColor_BrightGreen   = 92,
    TtyFgColor_BrightYellow  = 93,
    TtyFgColor_BrightBlue    = 94,
    TtyFgColor_BrightMagenta = 95,
    TtyFgColor_BrightCyan    = 96,
    TtyFgColor_BrightWhite   = 97,
}) TtyFgColor;

PACKED(typedef enum {
    TtyBgColor_None          = 0,
    TtyBgColor_Default       = 49,
    TtyBgColor_Black         = 40,
    TtyBgColor_Red           = 41,
    TtyBgColor_Green         = 42,
    TtyBgColor_Yellow        = 43,
    TtyBgColor_Blue          = 44,
    TtyBgColor_Magenta       = 45,
    TtyBgColor_Cyan          = 46,
    TtyBgColor_White         = 47,
    TtyBgColor_Bright        = 47,
    TtyBgColor_BrightBlack   = 100,
    TtyBgColor_BrightRed     = 101,
    TtyBgColor_BrightGreen   = 102,
    TtyBgColor_BrightYellow  = 103,
    TtyBgColor_BrightBlue    = 104,
    TtyBgColor_BrightMagenta = 105,
    TtyBgColor_BrightCyan    = 106,
    TtyBgColor_BrightWhite   = 107,
}) TtyBgColor;

PACKED(typedef enum {
    TtyStyleFlags_None      = 0,
    TtyStyleFlags_Bold      = 1 << 0,
    TtyStyleFlags_Faint     = 1 << 1,
    TtyStyleFlags_Italic    = 1 << 2,
    TtyStyleFlags_Underline = 1 << 3,
    TtyStyleFlags_Blink     = 1 << 4,
    TtyStyleFlags_Reversed  = 1 << 5,
}) TtyStyleFlags;

typedef struct {
    TtyFgColor fgColor;
    TtyBgColor bgColor;
    TtyStyleFlags flags;
} TtyStyle;

#define ttystyle(...)               \
    ((TtyStyle) {                   \
        .fgColor = TtyFgColor_None, \
        .bgColor = TtyBgColor_None, \
        .flags = TtyStyleFlags_None,\
        __VA_ARGS__                 \
    })

bool tty_isatty(File* file);

u16 tty_width(File* file);

u16 tty_height(File* file);

void tty_set_window_title(String title);

void tty_write_style_sequence(DynString* dynstr, TtyStyle style);

void tty_write_window_title_sequence(DynString* dynstr, String title);