#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <stdint.h>

struct cpu_state
{
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
} __attribute__((packed));

struct stack_state
{
    uint32_t error_code;
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
} __attribute__((packed));

void interrupt_handler(struct cpu_state *cpu, unsigned int interrupt, struct stack_state *stack);
void interrupt_install(void);
void interrupt_handler_register(uint8_t interrupt, void (*handler)(void));
void interrupt_handler_unregister(uint8_t interrupt);

#endif // INTERRUPTS_H