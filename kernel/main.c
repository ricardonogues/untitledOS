#include "include/drivers/vga/vga.h"
#include "include/drivers/tty/tty.h"
#include "libc/string.h"

void kernel_main(void) __attribute__((section(".text.kernel_main")));

void kernel_main(void)
{
    terminal_initialize();
    terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK));
    terminal_write("Hello, World!\n", strlen("Hello, World!\n"));
    terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
    terminal_write("Welcome to the kernel!\n", strlen("Welcome to the kernel!\n"));

    while (1)
    {
        // Kernel main loop
    }
}