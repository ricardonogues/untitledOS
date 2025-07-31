#ifndef STACK_PROTECTOR_H
#define STACK_PROTECTOR_H

#include "../include/types.h"

void stack_protector_init(void);
void update_stack_guard(uintptr_t new_value);
uintptr_t get_stack_guard(void);

#endif