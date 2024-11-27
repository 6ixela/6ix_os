; null segment descriptor
gdt_start:

gdt_null:   ; the mandatory null descriptor
    dd 0x0  ; dd means define double word (i.e. 4 bytes)
    dd 0x0

; code segment descriptor
gdt_code:
    dw 0xffff    ; segment length, bits 0-15
    dw 0x0       ; segment base, bits 0-15
    db 0x0       ; segment base, bits 16-23
    db 10011010b ; flags (8 bits)
    ; 100 / 1 => used vitual memory | 00 => privilege and ring 0
    ; 1 => data segment
    ; 1 => code segment
    ; 010 / conforming, readable, accessed
    db 11001111b ; flags (4 bits) + segment length, bits 16-19
    db 0x0       ; segment base, bits 24-31

; data segment descriptor
gdt_data:
    dw 0xffff
    dw 0x0
    db 0x0
    db 10010010b ; same before but it's not code segment so 0
    db 11001111b
    db 0x0

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start