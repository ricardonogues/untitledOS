#include "include/drivers/vga/vga.h"
#include "include/drivers/tty/tty.h"
#include "libc/string.h"
#include "libc/stack_protector.h"
#include "arch/x86/gdt.h"
// #include "tests/stack_test.h"

void kernel_main(void) __attribute__((section(".text.kernel_main")));

void kernel_main(void)
{
    terminal_initialize();
    terminal_writestring("Terminal initialized.\n");

    stack_protector_init();
    terminal_writestring("Stack protector initialized.\n");

    // Run stack protection test
    // test_stack_protection();

    gdt_install();
    terminal_writestring("GDT installed successfully.\n");

    while (1)
    {
        // Kernel main loop
    }
}