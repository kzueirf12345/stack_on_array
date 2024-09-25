#ifndef SRC_STACK_H
#define SRC_STACK_H

#include "verification/verification.h"


void stack_ctor(stack_t* const stack, const size_t start_capacity);
void stack_dtor(stack_t* const stack);


#endif /*SRC_STACK_H*/