#include "../../../include/arch/x86/gdt/gdt.h"
#include "../../../include/types.h"

#define GDT_ENTRIES 5

struct gdt_entry gdt_entries[GDT_ENTRIES] = {
    {0, 0, 0, 0, 0, 0}, // Null segment
    {0, 0, 0, 0, 0, 0}, // Code segment
    {0, 0, 0, 0, 0, 0}, // Data segment
    {0, 0, 0, 0, 0, 0}, // User code segment
    {0, 0, 0, 0, 0, 0}  // User data segment
};
struct gdt_ptr gdt_pointer;

extern void gdt_flush(void);

void gdt_install(void)
{
    // Initialize GDT entries and pointer
    gdt_pointer.limit = sizeof(struct gdt_entry) * 5 - 1; // Size of GDT entries
    gdt_pointer.base = (uintptr_t)&gdt_entries;           // Base address of GDT entries

    // Set up GDT entries (this is a simplified example)
    create_gdt_entry(0, 0, 0, 0, 0);                         // Null segment
    create_gdt_entry(1, 0x00000000, 0xFFFFFFFF, 0x9A, 0xCF); // Code segment
    create_gdt_entry(2, 0x00000000, 0xFFFFFFFF, 0x92, 0xCF); // Data segment
    create_gdt_entry(3, 0x00000000, 0xFFFFFFFF, 0xFA, 0xCF); // User code segment
    create_gdt_entry(4, 0x00000000, 0xFFFFFFFF, 0xF2, 0xCF); // User data segment

    // reload the GDT
    gdt_flush();
    // terminal_writestring("GDT installed successfully!\n");
}

void create_gdt_entry(int index, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity)
{
    if (index < 0 || index >= GDT_ENTRIES)
        return; // Invalid index

    gdt_entries[index].base_low = base & 0xFFFF;
    gdt_entries[index].base_middle = (base >> 16) & 0xFF;
    gdt_entries[index].base_high = (base >> 24) & 0xFF;

    gdt_entries[index].limit_low = limit & 0xFFFF;
    gdt_entries[index].granularity = (limit >> 16) & 0x0F;
    gdt_entries[index].granularity |= granularity & 0xF0; // Combine granularity bits
    gdt_entries[index].access = access;
}