#ifndef SYSCALL_H
#define SYSCALL_H

#include "../idt/isr.h"

#define SYSCALL_WRITE_CHAR 1
#define SYSCALL_ERROR_INVALID_ARGUMENT 22
#define SYSCALL_ERROR_NOT_IMPLEMENTED 38
#define USER_ADDRESS_LIMIT 0xC0000000

void syscall_dispatch(registers_t *regs);

#endif /* SYSCALL_H */
