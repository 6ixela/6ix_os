; load DH sectors to ES:BX from drive DL
disk_load:
    pusha
    push dx
                    ; how many sectors were request to be read,
                    ; even if it is altered in the meantime
    mov ah, 0x02    ; BIOS read sector function
    mov al, dh      ; Read DH sectors
    mov ch, 0x00    ; Select cylinder 0
    mov dh, 0x00    ; Select head 0
    mov cl, 0x02    ; Start reading from second sector (i.e.
                    ; after the boot sector)
    int 0x13        ; BIOS interrupt

    jc disk_error   ; Jump if error (i.e. carry flag set)

    pop dx
    cmp dh, al      ; if AL (sectors read) != DH (sectors expected)
    jne disk_error
    popa
    ret
disk_error :
    mov bx, DISK_ERROR_MSG
    call print_string
    call print_new_line

    mov dh, ah
    call print_hex
    jmp disk_loop

sectors_error:
    mov bx, SECTORS_ERROR
    call print_string
    call print_new_line

disk_loop:
    jmp $

DISK_ERROR_MSG: db "Disk read error!", 0
SECTORS_ERROR: db "Incorrect number of sectors read", 0