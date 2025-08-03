; Fixed load_from_disk function
load_from_disk:
    push si
    push di
    push ax
    push bx
    push cx
    push dx

    mov si, dap
    
    ; Setup DAP structure correctly
    mov byte [si], 0x10             ; Size of DAP (16 bytes)
    mov byte [si + 1], 0x00         ; Reserved
    mov word [si + 2], cx           ; Number of sectors to read
    mov word [si + 4], 0x0000       ; Buffer offset (0x0000)
    mov word [si + 6], dx           ; Buffer segment (from parameter)
    mov word [si + 8], bx           ; Starting LBA (lower 16 bits)
    mov word [si + 10], ax          ; Starting LBA (next 16 bits)
    mov dword [si + 12], 0x00000000 ; Starting LBA (upper 32 bits)

    ; Test for BIOS extended support
    mov ah, 0x41
    mov bx, 0x55AA
    mov dl, 0x80
    int 0x13
    jc disk_error
    cmp bx, 0xAA55
    jne disk_error

    ; Load sectors from disk
    mov ah, 0x42
    mov dl, 0x80
    mov si, dap
    int 0x13
    jc disk_error

    ; Restore registers and return
    pop dx
    pop cx
    pop bx
    pop ax
    pop di
    pop si
    ret

disk_error:
    mov si, error_msg
    call print_string
    hlt

error_msg:
    db 'Disk read error!', 0x0D, 0x0A, 0x00

; Corrected DAP structure
dap:
    db 0x10               ; Size of DAP structure (16 bytes) - FIXED
    db 0x00               ; Reserved
    dw 0x0000             ; Number of sectors (filled at runtime)
    dw 0x0000             ; Buffer offset
    dw 0x0000             ; Buffer segment
    dw 0x0000             ; Starting LBA (bits 0-15)
    dw 0x0000             ; Starting LBA (bits 16-31)
    dd 0x00000000         ; Starting LBA (bits 32-63)