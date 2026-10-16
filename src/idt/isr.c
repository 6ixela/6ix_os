#include "isr.h"

#include "../drivers/screen/io.h"
#include "../drivers/screen/screen.h"
#include "../syscall/syscall.h"

static const char *exception_messages[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",
    "Double fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment not present",
    "Stack fault",
    "General protection fault",
    "Page fault",
    "Unknown Interrupt",
    "Coprocessor Fault",
    "Alignment Fault",
    "Machine Check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

static void (*exception_handlers[32])(registers_t *regs);
static void (*irq_routines[16])(registers_t *regs);

void isr_initialize(void)
{
    for (uint32_t vector = 0; vector < 32; vector++)
    {
        exception_handlers[vector] = 0;
    }

    for (uint32_t irq = 0; irq < 16; irq++)
    {
        irq_routines[irq] = 0;
    }
}

static void print_page_fault(registers_t *regs)
{
    print_str("Page fault address: 0x");
    print_hex(regs->cr2);
    print_str("\nPage fault error code: 0x");
    print_hex(regs->err_code);
    print_str("\nPage fault access: ");
    print_str((regs->err_code & 0x1) ? "protection violation" : "not-present page");
    print_str((regs->err_code & 0x2) ? ", write" : ", read");
    print_str((regs->err_code & 0x4) ? ", user access\n" : ", supervisor access\n");
}

int isr_install_exception_handler(uint8_t vector, void (*handler)(registers_t *regs))
{
    if (vector >= 32 || handler == 0)
    {
        return -1;
    }

    exception_handlers[vector] = handler;
    return 0;
}

void isr_uninstall_exception_handler(uint8_t vector)
{
    if (vector < 32)
    {
        exception_handlers[vector] = 0;
    }
}

void isr_handler(registers_t *regs)
{
    if (regs->int_no == 128)
    {
        syscall_dispatch(regs);
        return;
    }

    if (regs->int_no >= 32)
    {
        return;
    }

    void (*handler)(registers_t *regs) = exception_handlers[regs->int_no];
    if (handler != 0)
    {
        handler(regs);
        return;
    }

    print_str("EXCEPTION: ");
    print_hex(regs->int_no);
    print_str(" - ");
    print_str((char *)exception_messages[regs->int_no]);
    print_str("\n");

    if (regs->int_no == 14)
    {
        print_page_fault(regs);
    }

    print_str("Exception! System halted.\n");
    for (;;)
    {
        asm volatile("hlt");
    }
}

void irq_install_handler(int irq, void (*handler)(registers_t *regs))
{
    if (irq >= 0 && irq < 16)
    {
        irq_routines[irq] = handler;
    }
}

void irq_uninstall_handler(int irq)
{
    if (irq >= 0 && irq < 16)
    {
        irq_routines[irq] = 0;
    }
}

void irq_handler(registers_t *regs)
{
    if (regs->int_no < 32 || regs->int_no >= 48)
    {
        return;
    }

    void (*handler)(registers_t *regs) = irq_routines[regs->int_no - 32];
    if (handler != 0)
    {
        handler(regs);
    }

    if (regs->int_no >= 40)
    {
        outb(0xA0, 0x20);
    }
    outb(0x20, 0x20);
}
