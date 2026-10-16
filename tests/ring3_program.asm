bits 32

%define SYSCALL_WRITE_CHAR 1
%define ERROR_INVALID_ARGUMENT 22
%define ERROR_NOT_IMPLEMENTED 38

section .rodata
global user_program_start
global user_program_end
global user_read_fault
global user_write_fault
global user_after_protection_tests
global user_program_done
global user_program_failed

user_program_start:
user_read_fault:
    mov eax, [dword 0x00001000]
user_write_fault:
    mov dword [dword 0x00001000], 0x12345678
user_after_protection_tests:
    mov eax, SYSCALL_WRITE_CHAR
    mov ebx, '#'
    int 0x80
    test eax, eax
    jnz user_program_failed

    mov eax, SYSCALL_WRITE_CHAR
    mov ebx, '@'
    int 0x80
    test eax, eax
    jnz user_program_failed

    mov eax, SYSCALL_WRITE_CHAR
    mov ebx, 0x100
    int 0x80
    cmp eax, -ERROR_INVALID_ARGUMENT
    jne user_program_failed

    mov eax, 0xFFFFFFFF
    int 0x80
    cmp eax, -ERROR_NOT_IMPLEMENTED
    jne user_program_failed

    mov eax, 0x600D
user_program_done:
    ud2

user_program_failed:
    mov eax, 0x0BAD
    jmp user_program_done
user_program_end:

global test_supervisor_page_fault
global test_supervisor_fault_resume
section .text
test_supervisor_page_fault:
    mov dword [0x00400000], 0x12345678
test_supervisor_fault_resume:
    ret
