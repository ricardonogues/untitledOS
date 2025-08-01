extern interrupt_handler

%macro no_error_code_interrupt_handler 1
global interrupt_handler_%1

interrupt_handler_%1:
    push dword 0    ; Push a dummy error code
    push dword %1   ; Push the interrupt number
    jmp common_interrupt_handler
%endmacro

%macro error_code_interrupt_handler 1
global interrupt_handler_%1
interrupt_handler_%1:
    ; In these cases the CPU pushes an error code onto the stack
    ; so we need to handle it accordingly by adding this many bytes to the esp below
    push dword $1   ; Push the interrupt number
    jmp common_interrupt_handler
%endmacro

common_interrupt_handler:
    ; Save registers
    push eax
    push ebx
    push ecx
    push edx
    push ebp
    push esi
    push edi

    ; Call the interrupt handler
    call interrupt_handler

    ; Restore registers
    pop edi
    pop esi
    pop ebp
    pop edx
    pop ecx
    pop ebx
    pop eax

    ; We always have 2 values on the stack: Either the dummy error code or the actual error code.
    ; In the case of the error code the CPU pushes it onto the stack before the interrupt number.
    add esp, 8  ; Remove the interrupt number and error code from the stack

    iret

no_error_code_interrupt_handler 0
no_error_code_interrupt_handler 1
no_error_code_interrupt_handler 2
no_error_code_interrupt_handler 3
no_error_code_interrupt_handler 4
no_error_code_interrupt_handler 5
no_error_code_interrupt_handler 6
no_error_code_interrupt_handler 7
error_code_interrupt_handler 8
no_error_code_interrupt_handler 9
error_code_interrupt_handler 10
error_code_interrupt_handler 11
error_code_interrupt_handler 12
error_code_interrupt_handler 13
error_code_interrupt_handler 14
no_error_code_interrupt_handler 15
no_error_code_interrupt_handler 16
error_code_interrupt_handler 17
no_error_code_interrupt_handler 18
no_error_code_interrupt_handler 19
no_error_code_interrupt_handler 20
error_code_interrupt_handler 21

; Hardware interrupt handlers (remaped to 32-47)
no_error_code_interrupt_handler 32 ; Timer
no_error_code_interrupt_handler 33 ; Keyboard
no_error_code_interrupt_handler 34 ; Cascade
no_error_code_interrupt_handler 35 ; COM2
no_error_code_interrupt_handler 36 ; COM3
no_error_code_interrupt_handler 37 ; COM4
no_error_code_interrupt_handler 38 ; LPT2
no_error_code_interrupt_handler 39 ; Floppy disk
no_error_code_interrupt_handler 40 ; LPT1
no_error_code_interrupt_handler 41 ; CMOS real-time clock
no_error_code_interrupt_handler 42 ; Primary ATA hard disk
no_error_code_interrupt_handler 43 ; Secondary ATA hard disk
no_error_code_interrupt_handler 44 ; Mouse
no_error_code_interrupt_handler 45 ; FPU (Floating Point Unit)
no_error_code_interrupt_handler 46 ; Primary SCSI controller
no_error_code_interrupt_handler 47 ; Secondary SCSI controller

; Software interrupt for testing
global interrupt_handler_0x80
interrupt_handler_0x80:
    push dword 0    ; Dummy error code
    push dword 0x80 ; Interrupt number
    jmp common_interrupt_handler