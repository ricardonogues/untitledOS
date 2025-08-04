#include <stdint.h>

#ifndef REALMODE_H
#define REALMODE_H

struct rm_regs {
    uint16_t gs;
    uint16_t fs;
    uint16_t es;
    uint16_t ds;
    uint32_t eflags;
    uint32_t ebp;
    uint32_t edi;
    uint32_t esi;
    uint32_t edx;
    uint32_t ecx;
    uint32_t ebx;
    uint32_t eax;
} __attribute__((packed));

void call_bios_interrupt_real_mode(uint8_t int_no, struct rm_regs *out_regs, struct rm_regs *in_regs);

#endif // REALMODE_H