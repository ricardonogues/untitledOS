#include "include/drivers/vga/vga.h"
#include "include/drivers/tty/tty.h"
#include "libc/string.h"
#include "libc/stack_protector.h"
#include "include/arch/x86/gdt/gdt.h"
#include "include/arch/x86/interrupts.h"
#include "tests/idt_test.h"
// #include "tests/stack_test.h"

void kernel_main(void) __attribute__((section(".text.kernel_main")));

void kernel_main(void)
{
    terminal_writestring("Kernel starting...\n");
    terminal_initialize();
    terminal_writestring("Terminal initialized.\n");

    stack_protector_init();
    terminal_writestring("Stack protector initialized.\n");

    // Run stack protection test
    // test_stack_protection();

    gdt_install();
    terminal_writestring("GDT installed successfully.\n");

    interrupt_install();
    terminal_writestring("Interrupts installed successfully.\n");

    // Run IDT tests
    run_all_idt_tests();
    terminal_writestring("\nKernel initialization complete. Entering main loop...\n");

    while (1)
    {
        // Kernel main loop
    }
}