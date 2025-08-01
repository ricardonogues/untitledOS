#ifndef IDT_H
#define IDT_H

#include "../../../../include/types.h"

#define IDT_ENTRIES 256

struct idt_entry
{
    uint16_t offset_low;
    uint16_t selector;
    uint8_t zero;
    uint8_t type_attr;
    uint16_t offset_high;
} __attribute__((packed));

struct idt_ptr
{
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

void idt_install(void);
void idt_load(struct idt_ptr *idt_ptr);
void idt_set_entry(int index, uint32_t offset, uint16_t selector, uint8_t type_attr);

#endif // IDT_H