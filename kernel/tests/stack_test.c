#include "./stack_test.h"
#include "../include/drivers/tty/tty.h"

void test_stack_protection(void)
{
    char buffer[10];

    // This should trigger stack protection
    for (int i = 0; i < 20; i++)
    {
        buffer[i] = 'A'; // Writes past buffer end
    }

    // This line should never execute if protection works
    terminal_write("ERROR: Stack protection failed!\n", 32);
}