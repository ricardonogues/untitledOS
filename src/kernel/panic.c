#include "include/drivers/vga/vga.h"
#include "include/drivers/tty/tty.h"

void kernel_panic(const char *message)
{
    terminal_initialize();
    terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK));
    terminal_writestring("Kernel panic: ");
    terminal_writestring(message);
    terminal_writestring("\n");
    while (1)
    {
        asm volatile("hlt"); // Halt the CPU
    }
}