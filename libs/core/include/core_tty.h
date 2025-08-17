/**
 * @file core_tty.h
 * @brief Terminal/TTY utilities for text styling and terminal control
 *
 * This header provides utilities for working with terminal/TTY devices including
 * text styling with ANSI escape codes, terminal size detection, and terminal
 * capability testing. It supports foreground and background colors, text styling
 * flags, and terminal window manipulation.
 */

#pragma once

#include "core_file.h"
#include "core_types.h"

/**
 * @brief Enumeration of ANSI foreground (text) colors
 */
PACKED(typedef enum {
    TtyFgColor_None          = 0,   /**< No color specified */
    TtyFgColor_Default       = 39,  /**< Terminal default foreground color */
    TtyFgColor_Black         = 30,  /**< Black text */
    TtyFgColor_Red           = 31,  /**< Red text */
    TtyFgColor_Green         = 32,  /**< Green text */
    TtyFgColor_Yellow        = 33,  /**< Yellow text */
    TtyFgColor_Blue          = 34,  /**< Blue text */
    TtyFgColor_Magenta       = 35,  /**< Magenta text */
    TtyFgColor_Cyan          = 36,  /**< Cyan text */
    TtyFgColor_White         = 37,  /**< White text */
    TtyFgColor_Bright        = 37,  /**< Alias for white (deprecated) */
    TtyFgColor_BrightBlack   = 90,  /**< Bright black (dark gray) text */
    TtyFgColor_BrightRed     = 91,  /**< Bright red text */
    TtyFgColor_BrightGreen   = 92,  /**< Bright green text */
    TtyFgColor_BrightYellow  = 93,  /**< Bright yellow text */
    TtyFgColor_BrightBlue    = 94,  /**< Bright blue text */
    TtyFgColor_BrightMagenta = 95,  /**< Bright magenta text */
    TtyFgColor_BrightCyan    = 96,  /**< Bright cyan text */
    TtyFgColor_BrightWhite   = 97,  /**< Bright white text */
}) TtyFgColor;

/**
 * @brief Enumeration of ANSI background colors
 */
PACKED(typedef enum {
    TtyBgColor_None          = 0,   /**< No background color specified */
    TtyBgColor_Default       = 49,  /**< Terminal default background color */
    TtyBgColor_Black         = 40,  /**< Black background */
    TtyBgColor_Red           = 41,  /**< Red background */
    TtyBgColor_Green         = 42,  /**< Green background */
    TtyBgColor_Yellow        = 43,  /**< Yellow background */
    TtyBgColor_Blue          = 44,  /**< Blue background */
    TtyBgColor_Magenta       = 45,  /**< Magenta background */
    TtyBgColor_Cyan          = 46,  /**< Cyan background */
    TtyBgColor_White         = 47,  /**< White background */
    TtyBgColor_Bright        = 47,  /**< Alias for white (deprecated) */
    TtyBgColor_BrightBlack   = 100, /**< Bright black (dark gray) background */
    TtyBgColor_BrightRed     = 101, /**< Bright red background */
    TtyBgColor_BrightGreen   = 102, /**< Bright green background */
    TtyBgColor_BrightYellow  = 103, /**< Bright yellow background */
    TtyBgColor_BrightBlue    = 104, /**< Bright blue background */
    TtyBgColor_BrightMagenta = 105, /**< Bright magenta background */
    TtyBgColor_BrightCyan    = 106, /**< Bright cyan background */
    TtyBgColor_BrightWhite   = 107, /**< Bright white background */
}) TtyBgColor;

/**
 * @brief Flags for ANSI text styling options
 */
PACKED(typedef enum {
    TtyStyleFlags_None      = 0,        /**< No text styling */
    TtyStyleFlags_Bold      = 1 << 0,   /**< Bold/bright text (SGR 1) */
    TtyStyleFlags_Faint     = 1 << 1,   /**< Faint/dim text (SGR 2) */
    TtyStyleFlags_Italic    = 1 << 2,   /**< Italic text (SGR 3) */
    TtyStyleFlags_Underline = 1 << 3,   /**< Underlined text (SGR 4) */
    TtyStyleFlags_Blink     = 1 << 4,   /**< Blinking text (SGR 5) */
    TtyStyleFlags_Reversed  = 1 << 5,   /**< Reversed colors (SGR 7) */
}) TtyStyleFlags;

/**
 * @brief Structure representing complete terminal text styling
 */
typedef struct {
    TtyFgColor fgColor;     /**< Foreground (text) color */
    TtyBgColor bgColor;     /**< Background color */
    TtyStyleFlags flags;    /**< Text styling flags */
} TtyStyle;

/**
 * @brief Create a TtyStyle with default values and optional overrides
 * @param ... Optional structure member assignments to override defaults
 * @return TtyStyle structure with specified styling
 */
#define ttystyle(...)               \
    ((TtyStyle) {                   \
        .fgColor = TtyFgColor_None, \
        .bgColor = TtyBgColor_None, \
        .flags = TtyStyleFlags_None,\
        __VA_ARGS__                 \
    })

/**
 * @brief Check if a file is connected to a terminal (TTY)
 * @param file File to check
 * @return true if the file is a terminal, false otherwise
 */
bool tty_isatty(File* file);

/**
 * @brief Get the width of the terminal in columns
 * @param file Terminal file to query
 * @return Terminal width in characters, or 0 if not determinable
 */
u16 tty_width(File* file);

/**
 * @brief Get the height of the terminal in rows
 * @param file Terminal file to query
 * @return Terminal height in characters, or 0 if not determinable
 */
u16 tty_height(File* file);

/**
 * @brief Set the terminal window title
 * @param title New title for the terminal window
 */
void tty_set_window_title(String title);

/**
 * @brief Write ANSI escape sequences for text styling to a dynamic string
 * @param dynstr Dynamic string to write sequences to
 * @param style Text styling to apply
 */
void tty_write_style_sequence(DynString* dynstr, TtyStyle style);

/**
 * @brief Write ANSI escape sequence for setting window title to a dynamic string
 * @param dynstr Dynamic string to write sequence to
 * @param title Window title to set
 */
void tty_write_window_title_sequence(DynString* dynstr, String title);