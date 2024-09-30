#ifndef SRC_VERIFICATION_H
#define SRC_VERIFICATION_H

#include <assert.h>
#include <stdio.h>

#include "../../logger/logger.h"
#include "../stack_structs.h"


#define STACK_MAX_SIZE_VALUE      1000000u
#define STACK_MAX_CAPACITY_VALUE  1000000u
#define STACK_MAX_ELEM_SIZE_VALUE 1000000u

enum StackError
{
    STACK_ERROR_SUCCESS                   = 0,
    STACK_ERROR_DATA_IS_NULL              = 1,
    STACK_ERROR_SIZE_GREATER_CAPACITY     = 2,
    STACK_ERROR_SIZE_OVERFLOW             = 3,
    STACK_ERROR_CAPACITY_OVERFLOW         = 4,
    STACK_ERROR_STACK_IS_NULL             = 5,
    STACK_ERROR_STANDART_ERRNO            = 6,
    STACK_ERROR_DATA_IS_INVALID           = 7,
    STACK_ERROR_STACK_IS_INVALID          = 8,
    STACK_ERROR_ELEM_SIZE_OVERFLOW        = 9,
    STACK_ERROR_ELEM_SIZE_IS_NULL         = 10,
#ifdef   PENGUIN_PROTECT
    STACK_ERROR_STACK_PENGUIN_LEFT        = 11,
    STACK_ERROR_STACK_PENGUIN_RIGHT       = 12,
    STACK_ERROR_DATA_PENGUIN_LEFT         = 13,
    STACK_ERROR_DATA_PENGUIN_RIGHT        = 14,
#endif /*PENGUIN_PROTECT*/
#ifdef   HASH_PROTECT
    STACK_ERROR_STACK_CONTROL_HASH_NEQUAL = 15,
    STACK_ERROR_STACK_CHECK_IS_NULL       = 16,
    STACK_ERROR_DATA_CONTROL_HASH_NEQUAL  = 17,
    STACK_ERROR_DATA_CHECK_IS_NULL        = 18,
#ifdef   PENGUIN_PROTECT
    STACK_ERROR_STACK_CHECK_PENGUIN_LEFT  = 19,
    STACK_ERROR_STACK_CHECK_PENGUIN_RIGHT = 20,
    STACK_ERROR_DATA_CHECK_PENGUIN_LEFT   = 21,
    STACK_ERROR_DATA_CHECK_PENGUIN_RIGHT  = 22,
#endif /*PENGUIN_PROTECT*/
#endif /*HASH_PROTECT*/
    STACK_ERROR_UNKNOWN                   = 23
};
static_assert(STACK_ERROR_SUCCESS == 0);


const char* stack_strerror(const enum StackError error);


#ifndef NDEBUG

enum StackError stack_verify_func(const stack_t* const stack);

enum StackError stack_dumb_func(const stack_t* const stack, place_in_code_t place_in_code);


#define STACK_VERIFY(stack)                                                                         \
    do                                                                                              \
    {                                                                                               \
        const enum StackError verify_error = stack_verify_func(stack);                              \
        if (verify_error)                                                                           \
        {                                                                                           \
            stack_dumb_func(stack, (place_in_code_t)                                                \
                                   { .file = __FILE__, .func = __func__, .line = __LINE__ });       \
            lassert(false, "stack error: %s", stack_strerror(verify_error));                        \
        }                                                                                           \
    } while (0)

#else  /*NDEBUG*/

#define STACK_VERIFY(stack) do {} while (0)

#endif /*NDEBUG*/


#endif /*SRC_VERIFICATION_H*/