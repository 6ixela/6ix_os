#include "keyboard.h"
#include "../../idt/isr.h"
#include "../screen/screen.h"
#include "../screen/io.h"
#include "../../shell/shell.h"

#define IRQ1 1

static int keyboard_number[10] = {
    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0'
};

// Todo : change
static int keyboard_scancode_to_char(uint8_t scancode)
{
    switch (scancode)
    {
        case 0x01: return 0;
        case 0x02: return '&';
        case 0x03: return 'e';
        case 0x04: return '"';
        case 0x05: return '\'';
        case 0x06: return '(';
        case 0x07: return '-';
        case 0x08: return 'e';
        case 0x09: return '_';
        case 0x0A: return 'c';
        case 0x0B: return 'a';
        case 0x0C: return ')';
        case 0x0D: return '=';
        case 0x0E: return '\b';
        case 0x0F: return '\t';
        case 0x10: return 'a';
        case 0x11: return 'z';
        case 0x12: return 'e';
        case 0x13: return 'r';
        case 0x14: return 't';
        case 0x15: return 'y';
        case 0x16: return 'u';
        case 0x17: return 'i';
        case 0x18: return 'o';
        case 0x19: return 'p';
        case 0x1A: return '^';
        case 0x1B: return '$';
        case 0x1C: return '\n';
        case 0x1D: return 0;
        case 0x1E: return 'q';
        case 0x1F: return 's';
        case 0x20: return 'd';
        case 0x21: return 'f';
        case 0x22: return 'g';
        case 0x23: return 'h';
        case 0x24: return 'j';
        case 0x25: return 'k';
        case 0x26: return 'l';
        case 0x27: return 'm';
        case 0x28: return 'u';
        case 0x29: return '*';
        case 0x2B: return '<';
        case 0x2C: return 'w';
        case 0x2D: return 'x';
        case 0x2E: return 'c';
        case 0x2F: return 'v';
        case 0x30: return 'b';
        case 0x31: return 'n';
        case 0x32: return ',';
        case 0x33: return ';';
        case 0x34: return ':';
        case 0x35: return '!';
        case 0x39: return ' ';
        case 0x56: return '>';
        default: print_hex(scancode); print_str("\n");
            return 0;
    }
}

static void keyboard_callback(registers_t *regs)
{
    uint8_t scancode = inb(0x60);
    static int ctrl_pressed = 0;
    static int maj_pressed = 0;

    if (scancode == 0x2A || scancode == 0x36)
    {
        maj_pressed = 1;
        return;
    }
    else if (scancode == 0xAA || scancode == 0xB6)
    {
        maj_pressed = 0;
        return;
    }

    if (scancode == 0x1D)
    {
        ctrl_pressed = 1;
        return;
    }

    if (scancode == 0x9D)
    {
        ctrl_pressed = 0;
        return;
    }

    if (scancode & 0x80)
    {
        return;
    }

    int c = keyboard_scancode_to_char(scancode);
    if (c == 0)
    {
        return;
    }

    if (ctrl_pressed)
    {
        if (c == 'c')
        {
            print_str("^C\n> ");
            return;
        }

        if (c == 'l')
        {
            clear_screen();
            print_str("> ");
            return;
        }
    }

    if (maj_pressed)
    {
        if (c >= 'a' && c <= 'z')
        {
            c -= 32;
        }
        if (scancode >= 0x02 && scancode <= 0x0B)
        {
            c = keyboard_number[scancode - 0x02];
        }
    }

    shell_handle_input(c);
}

void init_keyboard()
{
    irq_install_handler(IRQ1, keyboard_callback);
}