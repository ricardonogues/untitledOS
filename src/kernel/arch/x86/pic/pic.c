#include "../../../include/arch/x86/pic/pic.h"
#include <stdint.h>
#include "../../../include/arch/x86/ports.h"

void pic_remap(uint8_t offset1, uint8_t offset2)
{
    // Save current state
    uint8_t a1 = inb(PIC1_DATA);
    uint8_t a2 = inb(PIC2_DATA);

    // Start initialization sequence
    outb(PIC1_COMMAND_PORT, PIC_ICW1_INIT | PIC_ICW1_ICW4);
    io_wait();
    outb(PIC2_COMMAND_PORT, PIC_ICW1_INIT | PIC_ICW1_ICW4);
    io_wait();

    // Set vector offsets
    outb(PIC1_DATA, offset1);
    io_wait();
    outb(PIC2_DATA, offset2);
    io_wait();

    // Configure cascading
    outb(PIC1_DATA, 4);
    io_wait();
    outb(PIC2_DATA, 2);
    io_wait();

    // Set mode to 8086
    outb(PIC1_DATA, PIC_ICW4_8086);
    io_wait();
    outb(PIC2_DATA, PIC_ICW4_8086);
    io_wait();

    // Restore saved state
    outb(PIC1_DATA, a1);
    outb(PIC2_DATA, a2);
}

void pic_send_eoi(uint8_t irq)
{
    if (irq >= 8)
        outb(PIC2_COMMAND_PORT, PIC_ACKNOWLEDGE);
    outb(PIC1_COMMAND_PORT, PIC_ACKNOWLEDGE);
}