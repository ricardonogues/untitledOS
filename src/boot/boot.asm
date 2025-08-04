; Bootloader for Untitled OS
; This should load the kernel and jump to it
; Written in NASM syntax
; Assembled with NASM: nasm -f bin boot.s -o boot.bin
; This bootloader is designed to be loaded by the BIOS
; It will be loaded at address 0x7C00 and expects the kernel to be
; loaded at a specific location in memory.

ORG 0x7C00              ; The BIOS loads the bootloader at this address
                        ; so we need to make our code start here and generate
                        ; address offsets accordingly
BITS 16                 ; We are writing 16-bit code
                        ; address offsets accordingly

section .text           ; Code section

global _start           ; Entry point for the bootloader

_start:
    jmp 0:start         ; Jump to the start label

start:
    cli                 ; Clear interrupts to prevent any interrupts during boot
    cld                 ; Clear direction flag for string operations
    jmp 0x0000:initialise_segments

initialise_segments:
; Setup segment registers
    xor ax, ax          ; Clear AX
    mov ds, ax          ; Set data segment to 0
    mov es, ax          ; Set extra segment to 0
    mov fs, ax          ; Set fs segment to 0
    mov gs, ax          ; Set gs segment to 0

    ; Untitled OS isn't made for floppy disks, these are dead anyways.
    ; So if the value the BIOS passed is <0x80, just assume it has passed
    ; an incorrect value.
    cmp dl, 0x80
    jb not_supported
    ; Values above 0x8f are dubious so we assume we weren't booted properly
    ; for those either
    cmp dl, 0x8f
    ja not_supported

; Set up the stack
    xor ax, ax          ; Clear AX
    mov ss, ax          ; Set stack segment to 0
    mov sp, 0x7C00      ; Set stack pointer to the top of the bootloader
                        ; The stack will grow downwards from 0x7C00

; Setup text mode
    mov ax, 0x0003      ; Set video mode to 80x25 text mode
    int 0x10            ; Call BIOS video interrupt
    mov ah, 0x02        ; Set cursor position
    mov bh, 0x00        ; Page number
    mov dh, 0x00        ; Row (Y)
    mov dl, 0x00        ; Column (X)
    int 0x10

; Enable A20 line (Fast A20 Gate)
    in al, 0x92         ; Read keyboard controller status
    or al, 0x02         ; Set the A20 line enable bit
    out 0x92, al        ; Write back to the keyboard controller

    ; mov ax, 0x0000     ; Set segment to 0
    ; mov es, ax         ; Set extra segment to 0
    ; mov di, 0x0504          ; Set di to 0x0504. Otherwise this code will get stuck in `int 0x15` after some entries are fetched
    ; call do_e820         ; Call the memory map function

; Test for BIOS extended support
    mov ah, 0x41
    mov bx, 0x55AA
    mov dl, 0x80
    int 0x13           ; Call BIOS disk service to check for extended support
    jc disk_error      ; Jump if carry flag is set (error)
    cmp bx, 0xAA55     ; Check if the signature is correct
    jne disk_error     ; If not, jump to disk error handling

; Load the kernel from disk (int 13h ah=0x42)
    mov ah, 0x42       ; Function to read sectors
    mov dl, 0x80       ; Drive number (0x80 for first hard disk)
    mov si, dap        ; Load address of DAP

; Setup DAP for reading the kernel
    mov byte [si], 0x10        ; Set size of DAP structure (16 bytes)
    mov byte [si + 1], 0x00    ; Reserved byte
    mov word [si + 2], 0x0012  ; Set number of sectors to read
    mov dword [si + 4], 0x00001000  ; Set buffer address (0x1000:0x0000)
    mov dword [si + 8], 0x00000001  ; Set starting sector (lower part of LBA)
    mov dword [si + 12], 0x00000000 ; Set starting sector (upper part of LBA)

; Read the kernel from disk
    int 0x13           ; Call BIOS disk service
    jc disk_error      ; Jump if carry flag is set (error)

; Jump to the loaded kernel
    jmp success         ; Jump to success label

disk_error:
    ; Handle disk read error
    mov si, error_msg  ; Load address of error message
    call print_string   ; Print error message
    hlt                 ; Halt the system

; Setup GDT to switch to protected mode
success:
    lgdt [gdt_descriptor] ; Load GDT descriptor
    mov eax, cr0       ; Read control register 0
    or eax, 0x1        ; Set PE (Protection Enable) bit
    mov cr0, eax       ; Write back to control register 0

    jmp 0x08:protected_mode_start ; Jump to protected mode code

BITS 32                 ; Switch to 32-bit mode
                        ; Now we are in protected mode
protected_mode_start:
    ; Set up segment registers for protected mode
    mov ax, 0x10       ; Data segment selector (GDT entry 1)
    mov ds, ax         ; Set data segment
    mov es, ax         ; Set extra segment
    mov fs, ax         ; Set fs segment
    mov gs, ax         ; Set gs segment
    mov ss, ax         ; Set stack segment
    mov esp, 0x90000   ; Set stack pointer to the top of the bootloader

; Jump to the kernel entry point
    ; The kernel should be loaded at 0x1000:0000
    jmp 0x1000     ; Call the kernel entry point
loop:
    hlt                 ; Halt the system
    jmp loop            ; Infinite loop to prevent falling through

; Define GDT
gdt:
    dq 0x0000000000000000 ; Null descriptor
    dq 0x00CF9A000000FFFF ; Code segment descriptor (base=0, limit=4GB, present, executable, readable)
    dq 0x00CF92000000FFFF ; Data segment descriptor (base=0, limit=4GB, present, writable)
gdt_descriptor:
    dw gdt_end - gdt - 1 ; Size of GDT in bytes
    dd gdt                ; Address of GDT
gdt_end:
    dq 0x0000000000000000 ; End of GDT

; Define the DAP (Disk Address Packet) structure
; This structure is used to read sectors from the disk
; It is defined as per the BIOS disk services specification
dap:
    db 0x00               ; Size of DAP structure (16 bytes 0x10)
    db 0x00               ; Reserved
    dw 0x0000             ; Number of sectors to read (will be set later)
    dd 0x00000000         ; Buffer address (segment:offset as dword, e.g., 0x1000:0x0000)
    dq 0x0000000000000000 ; Starting LBA (Logical Block Addressing, 8 bytes)

; Error message to display on disk read error
error_msg:
    db 'Disk read error!', 0x0D, 0x0A, 0x00 ; Null-terminated string

not_supported:
    db 'Not supported!', 0x0D, 0x0A, 0x00 ; Null-terminated string

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

%include "src/boot/mmap.asm" ; Include the memory map function

;; Fill the bootloader with zeros to ensure it is 512 bytes
times 510 - ($ - $$) db 0 ; Fill the rest of the bootloader
dw 0xAA55               ; Boot signature (must be at offset 510-511)
                        ; This is required for the BIOS to recognize it as a bootable sector
                        ; The boot signature is 0xAA55, which is a standard for bootloaders
                        ; The BIOS checks for this signature to determine if the bootloader is valid
                        ; If the signature is not present, the BIOS will not boot from this sector
                        ; The bootloader must be exactly 512 bytes in size, so we fill the remaining
                        ; space with zeros to ensure it is the correct size
