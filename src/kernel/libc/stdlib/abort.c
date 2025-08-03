#include "../include/stdlib.h"

extern void kernel_panic(const char *message);

__attribute__((__noreturn__)) void abort(void)
{
    kernel_panic("abort called!");
}
