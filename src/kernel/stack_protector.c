#include "./include/panic.h"
#include <stdint.h>
#include <stdlib.h>

// Global canary value - GCC expects this exact symbol name
uintptr_t __stack_chk_guard = 0;

// Initialize the stack guard (called during kernel boot)
void stack_protector_init(void)
{
    // Simple initialization - improve this later
    __stack_chk_guard = 0xDEADBEEF; // TODO: Use proper randomness
}

// Called when canary is corrupted - GCC expects this exact function name
void __stack_chk_fail(void)
{
    // For now, just panic - improve this later
    abort();
}

// Optional: Functions to manage canaries
void update_stack_guard(uintptr_t new_value)
{
    __stack_chk_guard = new_value;
}

uintptr_t get_stack_guard(void)
{
    return __stack_chk_guard;
}