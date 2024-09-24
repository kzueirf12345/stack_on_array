#include "stack_funcs.h"

void stack_ctor(stack_t* const stack, const size_t start_capacity)
{
    assert(stack);

    stack->capacity = start_capacity;
    stack->size = 0;
    stack->data = calloc(start_capacity, sizeof(stack_elem_t));
    
    STACK_VERIFY(*stack);
}

void stack_dtor(stack_t* const stack)
{
    assert(stack);
    STACK_VERIFY(*stack);

    stack->capacity = 0;
    stack->size = 0;
    free(stack->data);
}
