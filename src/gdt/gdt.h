#ifndef GDT_H
#define GDT_H

#include "stdint.h"

#define KERNEL_CODE_SELECTOR 0x08
#define KERNEL_DATA_SELECTOR 0x10
#define USER_CODE_SELECTOR   0x1B
#define USER_DATA_SELECTOR   0x23
#define TSS_SELECTOR          0x28

void init_gdt(void);
void tss_set_kernel_stack(uint32_t stack_top);
int tss_kernel_stack_contains(uint32_t address);

#endif /* GDT_H */
