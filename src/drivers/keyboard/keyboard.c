#include "keyboard.h"
#include "../../idt/isr.h"
#include "../screen/screen.h"
#include "../screen/io.h"

#define IRQ1 1

static const char scancode_chars[] = {
    0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0,
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\',
    'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ', 0
};

static void keyboard_callback(registers_t *regs)
{
    uint8_t scancode = inb(0x60);

    if (scancode & 0x80)
    {
        return;
    }

    if (scancode >= sizeof(scancode_chars) / sizeof(scancode_chars[0]))
    {
        return;
    }

    char c = scancode_chars[scancode];
    if (c)
    {
        print_char(c);
    }
}

void init_keyboard()
{
    irq_install_handler(IRQ1, keyboard_callback);
}