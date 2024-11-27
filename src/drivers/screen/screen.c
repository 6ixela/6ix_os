#include "screen.h"

#include "io.h"

struct Char* start_video_addr = (struct Char*)VIDEO_ADDRESS;

size_t col = 0;
size_t row = 0;

uint8_t color = COLOR_BLACK << 4 | COLOR_WHITE;


// cursor need to be split into 2 parts:
// io part take 8bytes and cursor_location take 16 bytes
static void move_cursor()
{
    uint16_t cursor_location = row * 80 + col;
    outb(0x3D4, 14);                    // Tell the VGA board we are setting the high cursor byte.
    outb(0x3D5, cursor_location >> 8);  // Send the high cursor byte.
    outb(0x3D4, 15);                    // Tell the VGA board we are setting the low cursor byte.
    outb(0x3D5, cursor_location);
}

void set_color(uint8_t foreground, uint8_t background)
{
    color = background << 4 | foreground;
}

// TODO: do the action of scrooling
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
    
    if (col > MAX_COLS)
    {
        print_newline();
    }
    // scrool();
    start_video_addr[row * MAX_COLS + col] = value;
    col++;
    
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