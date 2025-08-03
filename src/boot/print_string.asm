print_string:
    push ax            ; Save registers that will be modified
    push bx
    push cx
    push dx
    ; Print a null-terminated string pointed to by SI
    mov ah, 0x0E       ; BIOS teletype output function
.next_char:
    lodsb              ; Load byte at DS:SI into AL and increment SI
    test al, al        ; Check if we reached the null terminator
    jz .done           ; If zero, we are done
    int 0x10           ; Call BIOS to print character in AL
    jmp .next_char     ; Repeat for the next character
.done:
    pop dx             ; Restore registers
    pop cx
    pop bx
    pop ax
    ret                ; Return from print_string