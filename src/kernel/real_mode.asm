section .text

; global rm_hcf
; rm_hcf:
;     ; Load BIOS IVT
;     lidt [.rm_idt]

;     ; Jump to real mode
;     jmp 0x08:.bits16
; bits 16
;   .bits16:
;     mov ax, 0x10
;     mov ds, ax
;     mov es, ax
;     mov fs, ax
;     mov gs, ax
;     mov ss, ax
;     mov eax, cr0
;     btr ax, 0
;     mov cr0, eax
;     jmp 0x00:.cszero
;   .cszero:
;     xor ax, ax
;     mov ds, ax
;     mov es, ax
;     mov fs, ax
;     mov gs, ax
;     mov ss, ax

;     sti
;   .hang:
;     hlt
;     jmp .hang
;     bits 32

;   .rm_idt:   dw 0x3ff
;              dd 0

global call_bios_interrupt_real_mode
call_bios_interrupt_real_mode:
    ; Self-modifying code: int $int_no
    mov al, byte [esp+4]
    mov byte [int_no], al

    ; Save out_regs
    mov eax, dword [esp+8]
    mov dword [0x8000], eax

    ; Save in_regs
    mov eax, dword [esp+12]
    mov dword [0x8004], eax

    ; Save GDT in case BIOS overwrites it
    sgdt [0x8008]

    ; Save IDT
    sidt [0x8010]

    ; Load BIOS IVT
    lidt [rm_idt]

    ; Save non-scratch GPRs
    push ebx
    push esi
    push edi
    push ebp

    ; Jump to real mode
    jmp 0x08:bits16
BITS 16
bits16:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov eax, cr0
    and al, 0xfe
    mov cr0, eax
    jmp 0x00:cszero
cszero:
    xor ax, ax
    mov ss, ax

    ; Load in_regs
    mov dword [0x8018], esp
    mov esp, dword [0x8004]
    pop gs
    pop fs
    pop es
    pop ds
    popfd
    pop ebp
    pop edi
    pop esi
    pop edx
    pop ecx
    pop ebx
    pop eax
    mov esp, dword [0x8018]

    sti

    ; Indirect interrupt call
    db 0xcd
int_no:
    db 0

    cli

    ; Load out_regs
    mov dword [0x8018], esp
    mov esp, dword [0x8000]
    lea esp, [esp + 10*4]
    push eax
    push ebx
    push ecx
    push edx
    push esi
    push edi
    push ebp
    pushfd
    push ds
    push es
    push fs
    push gs
    mov esp, dword [0x8018]

    ; Restore GDT
    o32 lgdt [0x8008]

    ; Restore IDT
    o32 lidt [0x8010]

    ; Jump back to pmode
    mov eax, cr0
    or al, 1
    mov cr0, eax
    jmp 0x18:bits32
BITS 32
bits32:
    mov ax, 0x20
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Restore non-scratch GPRs
    pop ebp
    pop edi
    pop esi
    pop ebx

    ; Exit
    ret

rm_idt:   dw 0x3ff
             dd 0
