[org 0x7c00]
mov bp, 0x9000 ; Set the stack.
mov sp, bp

mov bx, MSG_REAL_MODE
call print_string
call print_new_line
call switch_to_pm ; Note that we never return from here.
jmp $

%include "real_mode/print_string.asm"
%include "protected_mode/gdt.asm"
%include "protected_mode/print_string_pm.asm"
%include "protected_mode/switch_rm_to_pm.asm"
%include "real_mode/disk.asm"

[bits 32]

BEGIN_PM:
    mov ebx, MSG_PROT_MODE
    call print_string_pm
    jmp $
; Global variables
MSG_REAL_MODE db "Started in 16-bit Real Mode", 0
MSG_PROT_MODE db "Successfully landed in 32-bit Protected Mode", 0

times 510-($-$$) db 0
dw 0xaa55