#include "drivers/screen/screen.h"

void main(void)
{
    set_color(COLOR_BROWN, COLOR_LIGHT_RED);
    clear_screen();
    print_str("Hello, from a new S3E!\n");
    print_str("My name's Alexis VASSEUR");
}