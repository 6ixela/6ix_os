#include "screen.h"

#include "io.h"

struct Char* start_video_addr = (struct Char*)VIDEO_ADDRESS;

size_t col = 0;
size_t row = 0;

uint8_t color = COLOR_BLACK << 4 | COLOR_WHITE;

static void move_cursor()
{
    uint16_t cursor_location = row * 80 + col;
    outb(0x3D4, 14);
    outb(0x3D5, cursor_location >> 8);
    outb(0x3D4, 15);
    outb(0x3D5, cursor_location);
}

void set_color(uint8_t foreground, uint8_t background)
{
    color = background << 4 | foreground;
}

static void scrool()
{
    if (row >= MAX_ROWS)
    {
        for (uint8_t i = 0; i < MAX_ROWS - 1; i++)
        {
            for (uint8_t j = 0; j < MAX_COLS; j++)
            {
                start_video_addr[i * MAX_COLS + j] = start_video_addr[(i + 1) * MAX_COLS + j];
            }
        }
        struct Char reset = { ' ', color };
        for (size_t j = 0; j < MAX_COLS; j++)
        {
            start_video_addr[(MAX_ROWS - 1) * MAX_COLS + j] = reset;
        }
        row--;
    }
}

static void print_newline()
{   
    col = 0;
    row = row >= MAX_ROWS ? row : row + 1;
    scrool();
    move_cursor();
}

void print_hex(uint32_t n)
{
    char hex[9] = {0};
    for (int i = 7; i >= 0; i--)
    {
        uint8_t digit = n & 0xF;
        if (digit < 10)
        {
            hex[i] = '0' + digit;
        }
        else
        {
            hex[i] = 'A' + (digit - 10);
        }
        n >>= 4;
    }
    print_str(hex);
}

// TODO Handle the backspace, tab
void print_char(char c)
{
    struct Char value = { c, color };
    if (c == '\n')
    {
        print_newline();
        return;
    }

    if (c == '\r')
    {
        col = 0;
        move_cursor();
        return;
    }

    if (c == '\b')
    {
        if (col > 0)
        {
            col--;
        }
        else if (row > 0)
        {
            row--;
            col = MAX_COLS - 1;
        }

        start_video_addr[row * MAX_COLS + col] = (struct Char){ ' ', color };
        move_cursor();
        return;
    }
    
    if (col >= MAX_COLS)
    {
        print_newline();
    }

    start_video_addr[row * MAX_COLS + col] = value;
    col++;
    move_cursor();
}

void print_str(char* str)
{
    for (size_t i = 0; str[i] != '\0'; i++)
    {
        print_char(str[i]);
    }
    move_cursor();
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
            start_video_addr[i * MAX_COLS + j] = c;
        }
    }
}