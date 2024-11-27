[bits 32]

; Define some constants
VIDEO_MEMORY equ 0xb8000
WHITE_ON_BLACK equ 0x0f

print_string_pm:
    pusha
    mov edx , VIDEO_MEMORY ; Set edx to the start of vid mem.


print_string_pm_loop:
    mov al, [ebx]
    mov ah, WHITE_ON_BLACK

    cmp al, 0 ; check if end of string
    je print_string_pm_done
    
    mov [edx], ax

    inc ebx
    add edx, 2 ; Move to next character cell in vid mem.

    jmp print_string_pm_loop

print_string_pm_done:
    popa
    ret
    
