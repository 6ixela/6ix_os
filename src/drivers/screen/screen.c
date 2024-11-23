#include "screen.h"

struct Char* start_video_addr = (struct Char*)VIDEO_ADDRESS;

size_t col = 0;
size_t row = 0;

uint8_t color = COLOR_BLACK << 4 | COLOR_WHITE;

void set_color(uint8_t foreground, uint8_t background)
{
    color = background << 4 | foreground;
}

// TODO: do the action of scrooling
static void print_newline()
{
    col = 0;
    row++;
}

void print_char(char c)
{
    struct Char value = { c, color };
    if (c == '\n')
    {
        print_newline();
        return;
    }
    
    if (col > MAX_COLS)
    {
        print_newline();
    }
    start_video_addr[col + MAX_COLS * row] = value;
    
    col++;
}

void print_str(char* str)
{
    for (size_t i = 0; str[i] != '\0'; i++)
    {
        print_char(str[i]);
    }
}

void clear_screen()
{
    struct Char c = { ' ', color };
    col = 0;
    row = 0;
    for (size_t i = 0; i < MAX_ROWS; i++)
    {
        for (size_t j = 0; j < MAX_COLS; j++)
        {
            start_video_addr[j + MAX_COLS * i] = c;
        }
    }
}