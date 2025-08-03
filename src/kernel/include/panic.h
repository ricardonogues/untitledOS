#ifndef PANIC_H
#define PANIC_H

#include <stddef.h>
#include <stdint.h>

void kernel_panic(const char *message);

#endif