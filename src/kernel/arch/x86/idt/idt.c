#include "../../../include/arch/x86/idt/idt.h"

#include "../../../include/types.h"

#define IDT_ENTRIES 256

struct idt_entry idt[IDT_ENTRIES];
struct idt_ptr idt_ptr;

void idt_load(struct idt_ptr *idt_ptr)
{
    asm volatile("lidt %0" : : "m"(*idt_ptr));
}
void idt_install(void)
{
    // Initialize IDT pointer
    idt_ptr.limit = sizeof(struct idt_entry) * IDT_ENTRIES - 1;
    idt_ptr.base = (uintptr_t)&idt;

    // Clear IDT entries
    for (int i = 0; i < IDT_ENTRIES; i++)
    {
        idt[i].offset_low = 0;
        idt[i].selector = 0;
        idt[i].zero = 0;
        idt[i].type_attr = 0;
        idt[i].offset_high = 0;
    }

    // Load the IDT
    idt_load(&idt_ptr);
}
void idt_set_entry(int index, uint32_t offset, uint16_t selector, uint8_t type_attr)
{
    if (index < 0 || index >= IDT_ENTRIES)
        return; // Invalid index

    idt[index].offset_low = offset & 0xFFFF;
    idt[index].selector = selector;
    idt[index].zero = 0;
    idt[index].type_attr = type_attr;
    idt[index].offset_high = (offset >> 16) & 0xFFFF;
}