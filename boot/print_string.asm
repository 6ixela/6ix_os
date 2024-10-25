print_string:
    pusha
    mov ah, 0x0e
loops:
    mov al, [bx]
    int 0x10
    add bx, 1
    cmp al, 0
    jne loops
    popa
    ret