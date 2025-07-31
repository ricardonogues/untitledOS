#ifndef PIC_H
#define PIC_H

#include <stdint.h>

#define PIC1 0x20
#define PIC2 0xA0
#define PIC1_COMMAND PIC1
#define PIC1_DATA    PIC1 + 1
#define PIC2_COMMAND PIC2
#define PIC2_DATA    PIC2 + 1

#define PIC1_OFFSET 0x20
#define PIC2_OFFSET 0x28
#define PIC2_END PIC2_OFFSET + 7

#define PIC1_COMMAND_PORT PIC1_COMMAND
#define PIC2_COMMAND_PORT PIC2_COMMAND
#define PIC_ACKNOWLEDGE 0x20

#define PIC_ICW1_ICW4 0x01
#define PIC_ICW1_SINGLE 0x02
#define PIC_ICW1_INTERVAL_4 0x04
#define PIC_ICW1_LEVEL_TRIGGER 0x08
#define PIC_ICW1_INIT 0x10

#define PIC_ICW4_8086 0x01
#define PIC_ICW4_AUTO_EOI 0x02
#define PIC_ICW4_BUF_SLAVE 0x08
#define PIC_ICW4_BUF_MASTER 0x0C
#define PIC_ICW4_SFNM 0x10

void pic_remap(uint8_t offset1, uint8_t offset2);
void pic_send_eoi(uint8_t irq);

#endif // PIC_H