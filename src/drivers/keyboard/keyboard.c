#include "keyboard.h"
#include "../../idt/isr.h"
#include "../screen/screen.h"
#include "../screen/io.h"
#include "../../shell/shell.h"

#define IRQ1 1

static const char scancode_chars[] = {
    0, '~', '&', 'é', '"', '\'', '(', '-', '`', '_', '>', '@', ')', '=', '\b',
    '\t', 'a', 'z', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '^', '$', '\n', 0,
    'q', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 'm', '%', '*', 0, '<',
    'w', 'x', 'c', 'v', 'b', 'n', ',', ';', ':', '!', 0, '*', 0, ' ', 0
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
        shell_handle_input(c);
    }
}

void init_keyboard()
{
    irq_install_handler(IRQ1, keyboard_callback);
}