#include "drivers/screen/screen.h"
#include "idt/idt.h"
#include "timer/timer.h"

void main(void)
{
    init_idt();
    set_color(COLOR_WHITE, COLOR_BLACK);
    clear_screen();
    print_str("Hello, from a new S3E!\n");
    print_str("My name's Alexis VASSEUR\n");
}