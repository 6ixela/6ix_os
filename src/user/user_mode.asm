bits 32

global enter_user_mode
enter_user_mode:
    cli

    mov eax, [esp + 4]
    mov edx, [esp + 8]

    push dword 0x23
    push edx
    push dword 0x02
    push dword 0x1B
    push eax

    mov ax, 0x23
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    iretd
