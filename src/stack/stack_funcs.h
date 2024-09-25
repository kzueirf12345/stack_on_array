#ifndef SRC_STACK_H
#define SRC_STACK_H

#include "stack_structs.h"


void stack_ctor(stack_t* const stack, const size_t start_capacity);
void stack_dtor(stack_t* const stack);

enum StackError stack_push(stack_t* stack, const stack_elem_t elem);

#endif /*SRC_STACK_H*/