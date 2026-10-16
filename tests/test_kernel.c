#include "../src/drivers/screen/screen.h"
#include "../src/drivers/screen/io.h"
#include "../src/gdt/gdt.h"
#include "../src/idt/idt.h"
#include "../src/idt/isr.h"
#include "../src/libc/memory/memory.h"
#include "../src/user/user_mode.h"

#define PAGE_SIZE 4096
#define PAGE_COUNT 1024
#define USER_TEST_PAGE_DIRECTORY_INDEX 2
#define USER_CODE_ADDRESS 0x00800000
#define USER_STACK_BASE 0x00BFE000
#define USER_STACK_TOP 0x00BFF000
#define KERNEL_TEST_FAULT_ADDRESS 0x00400000
#define KERNEL_PROTECTED_ADDRESS 0x00001000

enum
{
    TEST_FAILURE_HANDLER_REGISTRATION = 0x00000001,
    TEST_FAILURE_KERNEL_FAULT_NOT_RECOVERED = 0x00000002,
    TEST_FAILURE_USER_MODE_RETURNED = 0x00000004,
    TEST_FAILURE_TSS_NOT_LOADED = 0x00000008,
    TEST_FAILURE_INVALID_TEST_MAPPING = 0x00000010,
    TEST_FAILURE_KERNEL_STACK = 0x00000020,
    TEST_FAILURE_UNEXPECTED_FAULT = 0x00000040,
    TEST_FAILURE_UNEXPECTED_PROTECTION_FAULT = 0x00000080,
    TEST_FAILURE_USER_FLOW = 0x00000100
};

static uint32_t page_directory[PAGE_COUNT] __attribute__((aligned(PAGE_SIZE)));
static uint32_t kernel_page_table[PAGE_COUNT] __attribute__((aligned(PAGE_SIZE)));
static uint32_t user_page_table[PAGE_COUNT] __attribute__((aligned(PAGE_SIZE)));
static uint8_t user_code_page[PAGE_SIZE] __attribute__((aligned(PAGE_SIZE)));
static uint8_t user_stack_page[PAGE_SIZE] __attribute__((aligned(PAGE_SIZE)));

extern const uint8_t user_program_start[];
extern const uint8_t user_program_end[];
extern const uint8_t user_read_fault[];
extern const uint8_t user_write_fault[];
extern const uint8_t user_after_protection_tests[];
extern const uint8_t user_program_done[];
extern void test_supervisor_page_fault(void);
extern const uint8_t test_supervisor_fault_resume[];

static uint32_t user_read_fault_address;
static uint32_t user_write_fault_address;
static uint32_t user_after_protection_address;
static uint32_t user_done_address;
static uint32_t test_failures;
static uint32_t kernel_fault_seen;
static uint32_t user_read_fault_seen;
static uint32_t user_write_fault_seen;

static void test_fail(uint32_t failure)
{
    test_failures |= failure;
}

static void test_finish(void)
{
    const char *message = test_failures == 0
        ? "OS SECURITY TESTS PASS\n"
        : "OS SECURITY TESTS FAIL\n";

    while (*message != '\0')
    {
        outb(0xE9, (uint8_t)*message);
        message++;
    }

    if (test_failures != 0)
    {
        outb(0xE9, '0');
        outb(0xE9, 'x');
        for (int shift = 28; shift >= 0; shift -= 4)
        {
            uint8_t digit = (uint8_t)((test_failures >> shift) & 0x0F);
            outb(0xE9, digit < 10 ? (uint8_t)('0' + digit) : (uint8_t)('A' + digit - 10));
        }
        outb(0xE9, '\n');
    }

    asm volatile("outl %0, %1" : : "a"(test_failures == 0 ? 0x10 : 0x11),
                 "Nd"((uint16_t)0xF4));
    for (;;)
    {
        asm volatile("hlt");
    }
}

static int register_test_handlers(void);
static void page_fault_test_handler(registers_t *regs);
static void invalid_opcode_test_handler(registers_t *regs);
static void prepare_user_address_space(void);
static void check_tss_loaded(void);

static int is_in_kernel_identity_map(uint32_t address, uint32_t size)
{
    return address < KERNEL_TEST_FAULT_ADDRESS &&
           size <= KERNEL_TEST_FAULT_ADDRESS - address;
}

void main(void)
{
    test_failures = 0;
    kernel_fault_seen = 0;
    user_read_fault_seen = 0;
    user_write_fault_seen = 0;

    init_gdt();
    init_idt();
    set_color(COLOR_WHITE, COLOR_BLACK);
    clear_screen();

    check_tss_loaded();
    if (test_failures != 0)
    {
        test_finish();
    }

    if (register_test_handlers() != 0)
    {
        test_fail(TEST_FAILURE_HANDLER_REGISTRATION);
        test_finish();
    }

    prepare_user_address_space();
    test_supervisor_page_fault();
    if (kernel_fault_seen == 0)
    {
        test_fail(TEST_FAILURE_KERNEL_FAULT_NOT_RECOVERED);
        test_finish();
    }

    enter_user_mode(USER_CODE_ADDRESS, USER_STACK_TOP);
    test_fail(TEST_FAILURE_USER_MODE_RETURNED);
    test_finish();
}

static int register_test_handlers(void)
{
    int page_fault_result = isr_install_exception_handler(14, page_fault_test_handler);
    int invalid_opcode_result = isr_install_exception_handler(6, invalid_opcode_test_handler);
    return page_fault_result == 0 && invalid_opcode_result == 0 ? 0 : -1;
}

static void check_tss_loaded(void)
{
    uint16_t task_register;
    asm volatile("str %0" : "=r"(task_register));
    if (task_register != TSS_SELECTOR)
    {
        test_fail(TEST_FAILURE_TSS_NOT_LOADED);
    }
}

static void prepare_user_address_space(void)
{
    uint32_t code_size = (uint32_t)(user_program_end - user_program_start);
    uint32_t code_physical_address = (uint32_t)user_code_page;
    uint32_t stack_physical_address = (uint32_t)user_stack_page;

    if ((code_physical_address & (PAGE_SIZE - 1)) != 0 ||
        (stack_physical_address & (PAGE_SIZE - 1)) != 0 ||
        code_size > PAGE_SIZE ||
        !is_in_kernel_identity_map((uint32_t)page_directory, sizeof(page_directory)) ||
        !is_in_kernel_identity_map((uint32_t)kernel_page_table, sizeof(kernel_page_table)) ||
        !is_in_kernel_identity_map((uint32_t)user_page_table, sizeof(user_page_table)) ||
        !is_in_kernel_identity_map(code_physical_address, PAGE_SIZE) ||
        !is_in_kernel_identity_map(stack_physical_address, PAGE_SIZE))
    {
        test_fail(TEST_FAILURE_INVALID_TEST_MAPPING);
        test_finish();
    }

    memset(page_directory, 0, sizeof(page_directory));
    memset(kernel_page_table, 0, sizeof(kernel_page_table));
    memset(user_page_table, 0, sizeof(user_page_table));
    memset(user_code_page, 0x90, sizeof(user_code_page));
    memset(user_stack_page, 0, sizeof(user_stack_page));
    memcpy(user_code_page, user_program_start, code_size);

    user_read_fault_address =
        USER_CODE_ADDRESS + (uint32_t)(user_read_fault - user_program_start);
    user_write_fault_address =
        USER_CODE_ADDRESS + (uint32_t)(user_write_fault - user_program_start);
    user_after_protection_address =
        USER_CODE_ADDRESS + (uint32_t)(user_after_protection_tests - user_program_start);
    user_done_address =
        USER_CODE_ADDRESS + (uint32_t)(user_program_done - user_program_start);

    for (uint32_t page = 0; page < PAGE_COUNT; page++)
    {
        kernel_page_table[page] = (page * PAGE_SIZE) | 0x03;
    }

    page_directory[0] = (uint32_t)kernel_page_table | 0x03;
    page_directory[USER_TEST_PAGE_DIRECTORY_INDEX] = (uint32_t)user_page_table | 0x07;
    user_page_table[0] = code_physical_address | 0x05;
    user_page_table[(USER_STACK_BASE >> 12) & 0x3FF] = stack_physical_address | 0x07;

    uint32_t cr0;
    asm volatile("mov %0, %%cr3" : : "r"(page_directory) : "memory");
    asm volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80010000;
    asm volatile("mov %0, %%cr0" : : "r"(cr0) : "memory");
}

static void page_fault_test_handler(registers_t *regs)
{
    uint32_t handler_stack = (uint32_t)&regs;
    if (regs->cs == USER_CODE_SELECTOR &&
        (!tss_kernel_stack_contains((uint32_t)regs) ||
         !tss_kernel_stack_contains(handler_stack)))
    {
        test_fail(TEST_FAILURE_KERNEL_STACK);
    }

    if (regs->cs == KERNEL_CODE_SELECTOR &&
        regs->cr2 == KERNEL_TEST_FAULT_ADDRESS &&
        regs->err_code == 0x02 &&
        kernel_fault_seen == 0)
    {
        kernel_fault_seen = 1;
        regs->eip = (uint32_t)test_supervisor_fault_resume;
        return;
    }

    if (regs->cs != USER_CODE_SELECTOR || regs->cr2 != KERNEL_PROTECTED_ADDRESS ||
        (regs->err_code & 0x05) != 0x05)
    {
        test_fail(TEST_FAILURE_UNEXPECTED_FAULT);
        test_finish();
    }

    if (regs->eip == user_read_fault_address &&
        regs->err_code == 0x05 && user_read_fault_seen == 0)
    {
        user_read_fault_seen = 1;
        regs->eip = user_write_fault_address;
        return;
    }

    if (regs->eip == user_write_fault_address &&
        regs->err_code == 0x07 && user_read_fault_seen != 0 &&
        user_write_fault_seen == 0)
    {
        user_write_fault_seen = 1;
        regs->eip = user_after_protection_address;
        return;
    }

    test_fail(TEST_FAILURE_UNEXPECTED_PROTECTION_FAULT);
    test_finish();
}

static void invalid_opcode_test_handler(registers_t *regs)
{
    uint32_t handler_stack = (uint32_t)&regs;
    volatile uint16_t *video_memory = (volatile uint16_t *)VIDEO_ADDRESS;

    if (regs->cs != USER_CODE_SELECTOR ||
        regs->ss != USER_DATA_SELECTOR ||
        regs->eip != user_done_address ||
        regs->eax != 0x600D ||
        regs->useresp != USER_STACK_TOP ||
        !tss_kernel_stack_contains((uint32_t)regs) ||
        !tss_kernel_stack_contains(handler_stack) ||
        kernel_fault_seen == 0 ||
        user_read_fault_seen == 0 ||
        user_write_fault_seen == 0 ||
        (video_memory[0] & 0x00FF) != '#' ||
        (video_memory[1] & 0x00FF) != '@')
    {
        test_fail(TEST_FAILURE_USER_FLOW);
    }
    test_finish();
}
