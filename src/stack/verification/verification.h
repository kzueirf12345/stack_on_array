#ifndef SRC_VERIFICATION_H
#define SRC_VERIFICATION_H

#include <assert.h>
#include <stdio.h>

#include "../../logger/logger.h"
#include "../stack_structs.h"

#define STACK_MAX_SIZE_VALUE     1000000u
#define STACK_MAX_CAPACITY_VALUE 1000000u

enum StackError
{
    STACK_ERROR_SUCCESS               = 0,
    // STACK_ERROR_STACK_IS_NULL         = 1,
    STACK_ERROR_DATA_IS_NULL          = 1,
    STACK_ERROR_SIZE_GREATER_CAPACITY = 2,
    STACK_ERROR_SIZE_OVERFLOW         = 3,
    STACK_ERROR_CAPACITY_OVERFLOW     = 4,
    STACK_ERROR_UNKNOWN               = 5
};
static_assert(STACK_ERROR_SUCCESS == 0);


enum StackError stack_verify_func(const stack_t stack);

const char* stack_strerror(const enum StackError error);


#ifndef NDEBUG

#define STACK_VERIFY(stack)                                                                        \
    do                                                                                             \
    {                                                                                              \
        const enum StackError verify_error = stack_verify_func(stack);                             \
        lassert(!verify_error, "stack error: %s", stack_strerror(verify_error));                   \
    } while (0)

#else  /*NDEBUG*/

#define STACK_VERIFY(stack) do {} while (0)

#endif /*NDEBUG*/


#endif /*SRC_VERIFICATION_H*/