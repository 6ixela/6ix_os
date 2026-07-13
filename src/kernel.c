#include "drivers/screen/screen.h"
#include "drivers/keyboard/keyboard.h"
#include "idt/idt.h"
#include "timer/timer.h"
#include "shell/shell.h"

void main(void)
{
    init_idt();
    init_keyboard();
    init_timer(100);
    asm volatile("sti");

    set_color(COLOR_WHITE, COLOR_BLACK);
    clear_screen();
    print_str("Hello, from a new S3E!\n");
    print_str("My name's Alexis VASSEUR\n");
    print_str("Keyboard ready. Type here:\n");
    shell_init();
}