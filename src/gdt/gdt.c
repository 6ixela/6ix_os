#include "gdt.h"

#include "../libc/memory/memory.h"

#define GDT_ENTRY_COUNT 6
#define KERNEL_STACK_SIZE 8192

typedef struct gdt_entry
{
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
} __attribute__((packed)) gdt_entry_t;

typedef struct gdt_pointer
{
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) gdt_pointer_t;

typedef struct tss_entry
{
    uint32_t previous_tss;
    uint32_t esp0;
    uint32_t ss0;
    uint32_t esp1;
    uint32_t ss1;
    uint32_t esp2;
    uint32_t ss2;
    uint32_t cr3;
    uint32_t eip;
    uint32_t eflags;
    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
    uint32_t es;
    uint32_t cs;
    uint32_t ss;
    uint32_t ds;
    uint32_t fs;
    uint32_t gs;
    uint32_t ldt;
    uint16_t trap;
    uint16_t iomap_base;
} __attribute__((packed)) tss_entry_t;

typedef char tss_entry_must_be_104_bytes[(sizeof(tss_entry_t) == 104) ? 1 : -1];

static gdt_entry_t gdt[GDT_ENTRY_COUNT];
static gdt_pointer_t gdt_pointer;
static tss_entry_t tss;
static uint8_t kernel_stack[KERNEL_STACK_SIZE] __attribute__((aligned(16)));

extern void gdt_flush(uint32_t pointer);
extern void tss_load(uint16_t selector);

static void set_gdt_entry(uint32_t index, uint32_t base, uint32_t limit,
                          uint8_t access, uint8_t flags)
{
    gdt[index].limit_low = (uint16_t)(limit & 0xFFFF);
    gdt[index].base_low = (uint16_t)(base & 0xFFFF);
    gdt[index].base_middle = (uint8_t)((base >> 16) & 0xFF);
    gdt[index].access = access;
    gdt[index].granularity = (uint8_t)(((limit >> 16) & 0x0F) | (flags & 0xF0));
    gdt[index].base_high = (uint8_t)((base >> 24) & 0xFF);
}

void init_gdt(void)
{
    memset(gdt, 0, sizeof(gdt));
    memset(&tss, 0, sizeof(tss));

    gdt_pointer.limit = sizeof(gdt) - 1;
    gdt_pointer.base = (uint32_t)gdt;

    set_gdt_entry(1, 0, 0xFFFFF, 0x9A, 0xC0);
    set_gdt_entry(2, 0, 0xFFFFF, 0x92, 0xC0);
    set_gdt_entry(3, 0, 0xFFFFF, 0xFA, 0xC0);
    set_gdt_entry(4, 0, 0xFFFFF, 0xF2, 0xC0);

    tss.ss0 = KERNEL_DATA_SELECTOR;
    tss.esp0 = (uint32_t)(kernel_stack + sizeof(kernel_stack));
    tss.iomap_base = sizeof(tss);
    set_gdt_entry(5, (uint32_t)&tss, sizeof(tss) - 1, 0x89, 0x00);

    gdt_flush((uint32_t)&gdt_pointer);
    tss_load(TSS_SELECTOR);
}

void tss_set_kernel_stack(uint32_t stack_top)
{
    tss.esp0 = stack_top;
}

int tss_kernel_stack_contains(uint32_t address)
{
    uint32_t stack_bottom = (uint32_t)kernel_stack;
    uint32_t stack_top = stack_bottom + sizeof(kernel_stack);
    return address >= stack_bottom && address < stack_top;
}
