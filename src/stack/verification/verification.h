#ifndef SRC_VERIFICATION_H
#define SRC_VERIFICATION_H

#include <assert.h>

#include "../../logger/logger.h"
#include "../stack_structs.h"

#define STACK_MAX_SIZE_VALUE     1000000u
#define STACK_MAX_CAPACITY_VALUE 1000000u

enum StackError
{
    STACK_ERROR_SUCCESS               = 0,
    STACK_ERROR_STACK_IS_NULL         = 1,
    STACK_ERROR_DATA_IS_NULL          = 2,
    STACK_ERROR_SIZE_GREATER_CAPACITY = 3,
    STACK_ERROR_SIZE_OVERFLOW         = 4,
    STACK_ERROR_CAPACITY_OVERFLOW     = 5,
    STACK_ERROR_UNKNOWN               = 6
};
static_assert(STACK_ERROR_SUCCESS == 0);


enum StackError verify(stack_t* const stack);


#endif /*SRC_VERIFICATION_H*/