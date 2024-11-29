#include "drivers/screen/screen.h"
#include "idt/idt.h"

void test_divide_by_zero() {
    int a = 1;
    int b = 0;
    int c = a / b;
}

void main(void)
{
    set_color(COLOR_WHITE, COLOR_BLACK);
    clear_screen();
    print_str("Hello, from a new S3E!\n");
    print_str("My name's Alexis VASSEUR\n");
    init_idt();
    test_divide_by_zero();
}