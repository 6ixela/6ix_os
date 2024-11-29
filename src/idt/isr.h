#ifndef ISR_H
#define ISR_H

#include "stdint.h"

typedef struct registers_t {
    uint32_t cr2;
    uint32_t ds;                           /* Data segment selector */
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; /* Pushed by pusha. */
    uint32_t int_no, err_code;             /* Interrupt number and error code (if applicable) */
    uint32_t eip, cs, eflags, useresp, ss; /* Pushed by the processor automatically */
} registers_t;

void isr_handler(registers_t* regs);
void irq_install_handler(int irq, void (*handler)(registers_t *r));
void irq_uninstall_handler(int irq);
void irq_handler(registers_t *regs);

#endif /* ISR_H */
