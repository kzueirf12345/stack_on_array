#include <errno.h>

#include "verification.h"

enum StackError stack_verify_func(const stack_t stack)
{
    if (!stack.data)
        return STACK_ERROR_DATA_IS_NULL;

    if (stack.size > STACK_MAX_SIZE_VALUE)
        return STACK_ERROR_SIZE_OVERFLOW;

    if (stack.capacity > STACK_MAX_CAPACITY_VALUE)
        return STACK_ERROR_CAPACITY_OVERFLOW;

    if (stack.size > stack.capacity)
        return STACK_ERROR_SIZE_GREATER_CAPACITY;

    if (errno)
        return STACK_ERROR_UNKNOWN;


    return STACK_ERROR_SUCCESS;
}

const char* stack_strerror(const enum StackError error)
{
    
#define CASE_ENUM_TO_STRING_(error) \
    case error: return #error

    switch(error)
    {
        CASE_ENUM_TO_STRING_(STACK_ERROR_SUCCESS);
        CASE_ENUM_TO_STRING_(STACK_ERROR_DATA_IS_NULL);
        CASE_ENUM_TO_STRING_(STACK_ERROR_SIZE_GREATER_CAPACITY);
        CASE_ENUM_TO_STRING_(STACK_ERROR_SIZE_OVERFLOW);
        CASE_ENUM_TO_STRING_(STACK_ERROR_CAPACITY_OVERFLOW);
        CASE_ENUM_TO_STRING_(STACK_ERROR_UNKNOWN);

        default: 
            fprintf(stderr, "unknown STACK_ERROR enum, it's soooo bad\n");
            return NULL;
    }

#undef CASE_ENUM_TO_STRING

    return NULL;
}