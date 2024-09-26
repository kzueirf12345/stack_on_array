#ifndef SRC_STACK_H
#define SRC_STACK_H

#include "stack_structs.h"


void stack_ctor(stack_t* const stack, const size_t start_capacity);
void stack_dtor(stack_t* const stack);

enum StackError stack_push(stack_t* stack, const stack_elem_t elem);
enum StackError stack_pop(stack_t* stack, stack_elem_t* const elem);
stack_elem_t stack_back(const stack_t stack);

#endif /*SRC_STACK_H*/