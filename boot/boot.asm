; boot.asm
[org 0x7c00]


start:
    mov bx, HELLO_WORLD
    pusha
    call print_string
    popa
    ; Boucle infinie
    jmp $

HELLO_WORLD:
    db 'Hello, World!', 0

%include "print_string.asm"
times 510 - ($ - $$) db 0
dw 0xaa55                   ; Signature du bootloader
