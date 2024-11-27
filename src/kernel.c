#include "drivers/screen/screen.h"

void main(void)
{
    set_color(COLOR_WHITE, COLOR_BLACK);
    clear_screen();
    print_str("Hello, from a new S3E!\n");
    print_str("My name's Alexis VASSEUR");
}