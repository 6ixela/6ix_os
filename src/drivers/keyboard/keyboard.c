#include "keyboard.h"
#include "../../idt/isr.h"
#include "../screen/screen.h"
#include "../screen/io.h"

#define IRQ1 33

static uint8_t *sc_name[] = {
    "escape", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0",
    "-", "=", "backspace", "Q", "W", "E", "R", "T", "Y", "U",
    "I", "O", "P", "[", "]",  "enter", "lctrl", "A", "S", "D", "F",
    "G", "H", "J", "K", "L", ";", "'", "`", "lshift", "\\", "Z", "X",
    "C", "X", "C", "V", "B", "N", "M", ",", ".", "/", "rshift", "*",
    "lalt", "space", "capsLock", "F1", "F2", "F3", "F4", "F5", "F6",
    "F7", "F8", "F9", "F10", "NumLock"
};

static const uint8_t scancode_letter[] = {
    
};

static void keyboard_callback(registers_t *regs) {
    uint8_t scancode = inb(0x60);
    print_str(sc_name[scancode]);
    print_char('\n');
}

void init_keyboard()
{
    irq_install_handler(IRQ1, keyboard_callback);
}