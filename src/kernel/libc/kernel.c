#include "../include/drivers/vga/vga.h"
#include "../include/drivers/tty/tty.h"
#include "kernel.h"

__attribute__((__noreturn__)) void abort(void)
{
    terminal_initialize();
    terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK));
    terminal_writestring("Kernel panic: abort called!\n");
    while (1)
    {
        asm volatile("hlt");
    }
}
