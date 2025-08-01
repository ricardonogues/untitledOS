#ifndef IDT_TEST_H
#define IDT_TEST_H

void test_idt_installation(void);
void test_divide_by_zero(void);
void test_invalid_opcode(void);
void test_software_interrupt(void);
void test_timer_interrupt(void);
void run_all_idt_tests(void);

#endif // IDT_TEST_H