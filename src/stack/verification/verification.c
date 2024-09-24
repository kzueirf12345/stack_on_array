#include <errno.h>

#include "verification.h"

enum StackError verify(stack_t* const stack)
{
    if (!stack)
        return STACK_ERROR_STACK_IS_NULL;
    
    if (!stack->data)
        return STACK_ERROR_DATA_IS_NULL;

    if (stack->size > STACK_MAX_SIZE_VALUE)
        return STACK_ERROR_SIZE_OVERFLOW;

    if (stack->capacity > STACK_MAX_CAPACITY_VALUE)
        return STACK_ERROR_CAPACITY_OVERFLOW;

    if (stack->size > stack->capacity)
        return STACK_ERROR_SIZE_GREATER_CAPACITY;

    if (errno)
        return STACK_ERROR_UNKNOWN;


    return STACK_ERROR_SUCCESS;
}