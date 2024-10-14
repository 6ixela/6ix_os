; boot.asm
start:
    mov ah, 0x03
    int 0x10

    mov al, 'H'
    int 0x10
    mov al, 'e'
    int 0x10
    mov al, 'l'
    int 0x10
    int 0x10
    mov al, 'o'
    int 0x10

    ; Boucle infinie
loop:
    jmp loop

; Remplissage pour atteindre 512 octets (taille d'un secteur)
times 510 - ($ - $$) db 0
dw 0xaa55             ; Signature du bootloader
