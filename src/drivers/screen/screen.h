#ifndef SCREEN_H
#define SCREEN_H

#include <stdint.h>
#include <stddef.h>

#define VIDEO_ADDRESS 0xb8000
#define MAX_ROWS 25
#define MAX_COLS 80

enum {
    COLOR_BLACK = 0,
    COLOR_BLUE = 1,
    COLOR_GREEN = 2,
    COLOR_CYAN = 3,
    COLOR_RED = 4,
    COLOR_MAGENTA = 5,
    COLOR_BROWN = 6,
    COLOR_LIGHT_GRAY = 7,
    COLOR_DARK_GRAY = 8,
    COLOR_LIGHT_BLUE = 9,
    COLOR_LIGHT_GREEN = 10,
    COLOR_LIGHT_CYAN = 11,
    COLOR_LIGHT_RED = 12,
    COLOR_PINK = 13,
    COLOR_YELLOW = 14,
    COLOR_WHITE = 15,
};

struct Char
{
    uint8_t character;
    uint8_t color;
};

void clear_screen();
void print_char(char c);
void print_str(char* str);

// set the color
// foreground: 0x0f
// background: 0xf0
void set_color(uint8_t foreground, uint8_t background);

#endif /* !SCREEN_H */