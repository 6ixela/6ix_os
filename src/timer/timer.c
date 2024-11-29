#include "timer.h"
#include "../idt/isr.h"
#include "../drivers/screen/screen.h"
#include "../drivers/screen/io.h"

uint32_t tick = 0;

static void timer_callback(registers_t regs)
{
    tick++;
    print_str("Tick: ");
    print_str("\n");
}

void init_timer(uint32_t freq)
{
    irq_install_handler(0, &timer_callback); // IRQ0
    uint32_t div = 1193180 / freq;

    outb(0x43, 0x36);

    uint8_t l = (uint8_t)(div & 0xff);
    uint8_t h = (uint8_t)((div >> 8) & 0xff);
    outb(0x40, l);
    outb(0x40, h);
}

