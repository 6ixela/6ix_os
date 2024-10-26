; boot.asm
[bits 32]
VIDEO_MEMORY equ 0xb8000
WHITE_ON_BLACK equ 0x0f

print_string_pm:
    pusha
    mov edx, VIDEO_MEMORY

print_string_pm_loop:

start:
    mov dx , 0x1fb6 ; store the value to print in dx
    call print_hex
    ; Boucle infinie
    jmp $

HELLO_WORLD:
    db 'Hello, World!', 0

%include "print_string.asm"
%include "print_hex.asm"

times 510 - ($ - $$) db 0
dw 0xaa55                   ; Signature du bootloader
