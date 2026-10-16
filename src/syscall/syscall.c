#include "syscall.h"

#include "../drivers/screen/screen.h"
#include "../gdt/gdt.h"

static void write_user_character(uint8_t character)
{
    print_char((char)character);
}

void syscall_dispatch(registers_t *regs)
{
    if (regs->cs != USER_CODE_SELECTOR ||
        regs->ss != USER_DATA_SELECTOR ||
        (regs->eflags & 0x00003000) != 0 ||
        regs->eip >= USER_ADDRESS_LIMIT ||
        regs->useresp >= USER_ADDRESS_LIMIT)
    {
        regs->eax = (uint32_t)-(int32_t)SYSCALL_ERROR_INVALID_ARGUMENT;
        return;
    }

    switch (regs->eax)
    {
        case SYSCALL_WRITE_CHAR:
            if (regs->ebx > 0xFF)
            {
                regs->eax = (uint32_t)-(int32_t)SYSCALL_ERROR_INVALID_ARGUMENT;
                return;
            }
            write_user_character((uint8_t)regs->ebx);
            regs->eax = 0;
            return;

        default:
            regs->eax = (uint32_t)-(int32_t)SYSCALL_ERROR_NOT_IMPLEMENTED;
            return;
    }
}
