#include "../../include/arch/x86/interrupts.h"
#include "../../include/arch/x86/idt/idt.h"
#include "../../include/arch/x86/pic/pic.h"
#include "../../include/drivers/tty/tty.h"
#include "../../include/types.h"

static void (*interrupt_handlers[256])(struct cpu_state *cpu, struct stack_state *stack) = {0};

extern void interrupt_handler_0(void);
extern void interrupt_handler_1(void);
extern void interrupt_handler_2(void);
extern void interrupt_handler_3(void);
extern void interrupt_handler_4(void);
extern void interrupt_handler_5(void);
extern void interrupt_handler_6(void);
extern void interrupt_handler_7(void);
extern void interrupt_handler_8(void);
extern void interrupt_handler_9(void);
extern void interrupt_handler_10(void);
extern void interrupt_handler_11(void);
extern void interrupt_handler_12(void);
extern void interrupt_handler_13(void);
extern void interrupt_handler_14(void);
extern void interrupt_handler_15(void);
extern void interrupt_handler_16(void);
extern void interrupt_handler_17(void);
extern void interrupt_handler_18(void);
extern void interrupt_handler_19(void);
extern void interrupt_handler_20(void);
extern void interrupt_handler_21(void);
extern void interrupt_handler_32(void); // Timer
extern void interrupt_handler_33(void); // Keyboard
extern void interrupt_handler_0x80(void);

void interrupt_install(void)
{
    // Initialize the IDT
    idt_install();

    // Remap the PIC
    pic_remap(PIC1_OFFSET, PIC2_OFFSET);

    // Set up default handlers for interrupts
    for (int i = 0; i < 256; i++)
    {
        interrupt_handlers[i] = NULL; // No handler by default
    }

    idt_set_entry(0, (uint32_t)interrupt_handler_0, 0x08, 0x8E);   // Division by zero
    idt_set_entry(1, (uint32_t)interrupt_handler_1, 0x08, 0x8E);   // Debug
    idt_set_entry(2, (uint32_t)interrupt_handler_2, 0x08, 0x8E);   // NMI
    idt_set_entry(3, (uint32_t)interrupt_handler_3, 0x08, 0x8E);   // Breakpoint
    idt_set_entry(4, (uint32_t)interrupt_handler_4, 0x08, 0x8E);   // Overflow
    idt_set_entry(5, (uint32_t)interrupt_handler_5, 0x08, 0x8E);   // Bound Range Exceeded
    idt_set_entry(6, (uint32_t)interrupt_handler_6, 0x08, 0x8E);   // Invalid Opcode
    idt_set_entry(7, (uint32_t)interrupt_handler_7, 0x08, 0x8E);   // Device Not Available
    idt_set_entry(8, (uint32_t)interrupt_handler_8, 0x08, 0x8E);   // Double Fault
    idt_set_entry(9, (uint32_t)interrupt_handler_9, 0x08, 0x8E);   // Coprocessor Segment Overrun
    idt_set_entry(10, (uint32_t)interrupt_handler_10, 0x08, 0x8E); // Invalid TSS
    idt_set_entry(11, (uint32_t)interrupt_handler_11, 0x08, 0x8E); // Segment Not Present
    idt_set_entry(12, (uint32_t)interrupt_handler_12, 0x08, 0x8E); // Stack Fault
    idt_set_entry(13, (uint32_t)interrupt_handler_13, 0x08, 0x8E); // General Protection Fault
    idt_set_entry(14, (uint32_t)interrupt_handler_14, 0x08, 0x8E); // Page Fault
    idt_set_entry(15, (uint32_t)interrupt_handler_15, 0x08, 0x8E); // Reserved
    idt_set_entry(16, (uint32_t)interrupt_handler_16, 0x08, 0x8E); // Floating Point Error
    idt_set_entry(17, (uint32_t)interrupt_handler_17, 0x08, 0x8E); // Alignment Check
    idt_set_entry(18, (uint32_t)interrupt_handler_18, 0x08, 0x8E); // Machine Check
    idt_set_entry(19, (uint32_t)interrupt_handler_19, 0x08, 0x8E); // SIMD Floating Point Exception
    idt_set_entry(20, (uint32_t)interrupt_handler_20, 0x08, 0x8E); // Virtualization Exception
    idt_set_entry(21, (uint32_t)interrupt_handler_21, 0x08, 0x8E); // Control Protection Exception

    // Set up specific handlers if needed (e.g., for timer, keyboard, etc.)
    // Example: interrupt_handler_register(32, timer_interrupt_handler);
    idt_set_entry(32, (uint32_t)interrupt_handler_32, 0x08, 0x8E); // Timer (IRQ0)
    idt_set_entry(33, (uint32_t)interrupt_handler_33, 0x08, 0x8E); // Keyboard (IRQ1)

    // Set up software interrupt for testing
    idt_set_entry(0x80, (uint32_t)interrupt_handler_0x80, 0x08, 0x8E);

    // Enable interrupts
    asm volatile("sti");
}

void interrupt_handler(struct cpu_state *cpu, unsigned int interrupt, struct stack_state *stack)
{
    if (interrupt < 256 && interrupt_handlers[interrupt])
    {
        // Call the registered handler for this interrupt
        interrupt_handlers[interrupt](cpu, stack);
    }

    // Send EOI for hardware interrupts
    if (interrupt >= 32 && interrupt <= 47)
    {
        pic_send_eoi(interrupt);
    }

    // For now, halt on exceptions (you might want to handle this differently)
    if (interrupt < 32)
    {
        terminal_writestring("System halted due to exception.\n");
        asm volatile("cli; hlt");
    }
}

void interrupt_handler_register(uint8_t interrupt, void (*handler)(struct cpu_state *cpu, struct stack_state *stack))
{
    interrupt_handlers[interrupt] = handler;
}

void interrupt_handler_unregister(uint8_t interrupt)
{
    interrupt_handlers[interrupt] = NULL; // Unregister the handler
}