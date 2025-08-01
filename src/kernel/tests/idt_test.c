#include "idt_test.h"
#include "../include/drivers/tty/tty.h"
#include "../include/drivers/vga/vga.h"
#include "../include/arch/x86/idt/idt.h"
#include "../include/arch/x86/ports.h"
#include "../include/arch/x86/interrupts.h"

extern void interrupt_handler_0x80(void);

static volatile int interrupt_received = 0;
static volatile int timer_ticks = 0;

// Test handler for software interrupt
void test_interrupt_handler(struct cpu_state *cpu, struct stack_state *stack)
{
    interrupt_received = 1;
    terminal_writestring("Software interrupt handler called!\n");
}

// Test handler for timer interrupt
void timer_interrupt_handler(struct cpu_state *cpu, struct stack_state *stack)
{
    timer_ticks++;
    if (timer_ticks % 100 == 0)
    { // Print every 100 ticks to avoid spam
        terminal_writestring("Timer tick: ");
        // Simple number printing
        char num_str[16];
        int temp = timer_ticks / 100;
        int i = 0;
        if (temp == 0)
        {
            num_str[i++] = '0';
        }
        else
        {
            while (temp > 0)
            {
                num_str[i++] = '0' + (temp % 10);
                temp /= 10;
            }
            // Reverse
            for (int j = 0; j < i / 2; j++)
            {
                char tmp = num_str[j];
                num_str[j] = num_str[i - 1 - j];
                num_str[i - 1 - j] = tmp;
            }
        }
        num_str[i] = '\0';
        terminal_writestring(num_str);
        terminal_writestring("\n");
    }
}

void test_idt_installation(void)
{
    terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK));
    terminal_writestring("\n=== Testing IDT Installation ===\n");
    terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));

    // Check if IDT is loaded by reading IDTR
    struct idt_ptr current_idt;
    asm volatile("sidt %0" : "=m"(current_idt));

    terminal_writestring("IDT Base: 0x");
    // Print hex address (simplified)
    for (int i = 28; i >= 0; i -= 4)
    {
        uint8_t nibble = (current_idt.base >> i) & 0xF;
        char hex_char = (nibble < 10) ? ('0' + nibble) : ('A' + nibble - 10);
        terminal_putchar(hex_char);
    }
    terminal_writestring("\n");

    terminal_writestring("IDT Limit: ");
    char limit_str[16];
    int temp = current_idt.limit;
    int i = 0;
    while (temp > 0)
    {
        limit_str[i++] = '0' + (temp % 10);
        temp /= 10;
    }
    // Reverse
    for (int j = 0; j < i / 2; j++)
    {
        char tmp = limit_str[j];
        limit_str[j] = limit_str[i - 1 - j];
        limit_str[i - 1 - j] = tmp;
    }
    limit_str[i] = '\0';
    terminal_writestring(limit_str);
    terminal_writestring("\n");

    if (current_idt.limit > 0)
    {
        terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
        terminal_writestring("IDT appears to be loaded!\n");
    }
    else
    {
        terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK));
        terminal_writestring("IDT not properly loaded!\n");
    }
    terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
}

void test_software_interrupt(void)
{
    terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK));
    terminal_writestring("\n=== Testing Software Interrupt (INT 0x80) ===\n");
    terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));

    // Register our test handler for interrupt 0x80
    interrupt_handler_register(0x80, test_interrupt_handler);

    // Set up IDT entry for interrupt 0x80
    idt_set_entry(0x80, (uint32_t)interrupt_handler_0x80, 0x08, 0x8E);

    interrupt_received = 0;
    terminal_writestring("Triggering software interrupt...\n");

    // Trigger software interrupt
    asm volatile("int $0x80");

    if (interrupt_received)
    {
        terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
        terminal_writestring("Software interrupt test PASSED!\n");
    }
    else
    {
        terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK));
        terminal_writestring("Software interrupt test FAILED!\n");
    }
    terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
}

void test_timer_interrupt(void)
{
    terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK));
    terminal_writestring("\n=== Testing Timer Interrupt (IRQ0) ===\n");
    terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));

    // Register timer handler
    interrupt_handler_register(32, timer_interrupt_handler); // IRQ0 = interrupt 32

    terminal_writestring("Setting up PIT timer...\n");

    // Program the PIT (Programmable Interval Timer)
    // Channel 0, Mode 3 (square wave), binary counting
    outb(0x43, 0x36);

    // Set frequency to ~100 Hz (1193180 / 100 = 11931)
    uint16_t divisor = 11931;
    outb(0x40, divisor & 0xFF);        // Low byte
    outb(0x40, (divisor >> 8) & 0xFF); // High byte

    terminal_writestring("Timer configured. Waiting for interrupts...\n");
    terminal_writestring("(This will show timer ticks for ~3 seconds)\n");

    timer_ticks = 0;
    int initial_ticks = timer_ticks;

    // Wait for some timer ticks (rough timing)
    for (volatile int i = 0; i < 10000000; i++)
    {
        asm volatile("nop");
        if (timer_ticks > initial_ticks + 5)
            break; // Wait for at least 5 ticks
    }

    if (timer_ticks > initial_ticks)
    {
        terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
        terminal_writestring("Timer interrupt test PASSED!\n");
    }
    else
    {
        terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK));
        terminal_writestring("Timer interrupt test FAILED!\n");
    }
    terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
}

void test_divide_by_zero(void)
{
    terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK));
    terminal_writestring("\n=== Testing Exception Handling (Divide by Zero) ===\n");
    terminal_setcolor(vga_entry_color(VGA_COLOR_MAGENTA, VGA_COLOR_BLACK));
    terminal_writestring("WARNING: This test will trigger a divide by zero exception!\n");
    terminal_writestring("If IDT is working, you should see an exception message.\n");
    terminal_writestring("Press any key to continue or wait 3 seconds...\n");
    terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));

    // Simple delay
    for (volatile int i = 0; i < 50000000; i++)
    {
        asm volatile("nop");
    }

    terminal_writestring("Triggering divide by zero...\n");

    // This should trigger interrupt 0 (divide by zero exception)
    volatile int zero = 0;
    volatile int result = 42 / zero; // This will cause divide by zero

    // This line should never execute if the exception handler works
    terminal_writestring("ERROR: Exception handler did not work!\n");
    (void)result; // Suppress unused variable warning
}

void test_invalid_opcode(void)
{
    terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK));
    terminal_writestring("\n=== Testing Invalid Opcode Exception ===\n");
    terminal_setcolor(vga_entry_color(VGA_COLOR_MAGENTA, VGA_COLOR_BLACK));
    terminal_writestring("WARNING: This test will trigger an invalid opcode exception!\n");
    terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));

    // Simple delay
    for (volatile int i = 0; i < 30000000; i++)
    {
        asm volatile("nop");
    }

    terminal_writestring("Triggering invalid opcode...\n");

    // Execute an invalid opcode (undefined instruction)
    asm volatile(".byte 0x0F, 0x0B"); // UD2 instruction - guaranteed invalid

    // This line should never execute
    terminal_writestring("ERROR: Invalid opcode exception handler did not work!\n");
}

void run_all_idt_tests(void)
{
    terminal_setcolor(vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
    terminal_writestring("\n");
    terminal_writestring("========================================\n");
    terminal_writestring("       IDT FUNCTIONALITY TESTS\n");
    terminal_writestring("========================================\n");
    terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));

    // Test 1: Check IDT installation
    test_idt_installation();

    // Test 2: Software interrupt (safest test)
    test_software_interrupt();

    // Test 3: Timer interrupt (hardware interrupt)
    test_timer_interrupt();

    terminal_setcolor(vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
    terminal_writestring("\n========================================\n");
    terminal_writestring("   EXCEPTION TESTS (WILL HALT SYSTEM)\n");
    terminal_writestring("========================================\n");
    terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));

    terminal_writestring("\nChoose exception test (these will halt the system):\n");
    terminal_writestring("1. Divide by zero test\n");
    terminal_writestring("2. Invalid opcode test\n");
    terminal_writestring("3. Skip exception tests\n");
    terminal_writestring("\nWaiting 5 seconds, then running divide by zero test...\n");

    // Wait 5 seconds then run divide by zero test
    for (volatile int i = 0; i < 100000000; i++)
    {
        asm volatile("nop");
    }

    test_divide_by_zero();
}