
print_hex:
    push ax ; val which transform int => char ''
    push bx ; ptr of HEX_OUT
    push cx ; => cpt
    push dx ; int dx => contain hex value

    mov bx, HEX_OUT + 2
    mov cx, 4
loop:
    mov ax, dx
    and ax, 0xf000
    shr ax, 12

    shl dx, 4

    call to_char
    mov [bx], ax
    inc bx
    dec cx
    cmp cx, 0
    jne loop
done:
    mov bx, HEX_OUT
    call print_string
    pop ax
    pop dx
    pop cx
    pop bx
reset:
    mov byte [HEX_OUT + 2], '0'
    mov byte [HEX_OUT + 3], '0'
    mov byte [HEX_OUT + 4], '0'
    mov byte [HEX_OUT + 5], '0'
    ret

to_char:
    cmp ax, 0xa
    jl digits
    sub ax, 0xa
    add ax, 'a'
    ret

digits:
    add ax, '0'
    ret

HEX_OUT: db '0x0000', 0