#ifndef SRC_VERIFICATION_H
#define SRC_VERIFICATION_H

#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>

#include "../stack_structs.h"


#define STACK_MAX_SIZE      1000000u
#define STACK_MAX_CAPACITY  1000000u
#define STACK_MAX_ELEM_SIZE 1000000u

enum StackError
{
    STACK_ERROR_SUCCESS                   = 0,
    STACK_ERROR_DATA_IS_NULL              = 1,
    STACK_ERROR_SIZE_GREATER_CAPACITY     = 2,
    STACK_ERROR_SIZE_OVERFLOW             = 3,
    STACK_ERROR_CAPACITY_OVERFLOW         = 4,
    STACK_ERROR_STACK_IS_NULL             = 5,
    STACK_ERROR_STANDARD_ERRNO            = 6,
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
    STACK_ERROR_DATA_CONTROL_HASH_NEQUAL  = 17,
#endif /*HASH_PROTECT*/
    STACK_ERROR_UNKNOWN                   = 23
};
static_assert(STACK_ERROR_SUCCESS == 0);


const char* stack_strerror(const enum StackError error);

#define stack_error_handle(call_func, ...)                                                          \
    do {                                                                                            \
        stack_error_handler = call_func;                                                            \
        if (stack_error_handler)                                                                    \
        {                                                                                           \
            fprintf(stderr, "Can't " #call_func". Stack error: %s\n",                               \
                            stack_strerror(input_error_handler));                                   \
            __VA_ARGS__                                                                             \
            return stack_error_handler;                                                             \
        }                                                                                           \
    } while(0)
    

enum StackError data_to_lX_str(const void* const data, const size_t size, char* const * lX_str,
                               const size_t lX_str_size);


#ifndef NDEBUG

enum StackError stack_verify_func(const stack_t* const stack);

void stack_dumb_func(const stack_t* const stack, place_in_code_t place_in_code,
                     enum StackError (*elem_to_str)(const void* const elem, const size_t elem_size,
                                                    char* const * str, const size_t mx_str_size));


#define STACK_VERIFY(stack, elem_to_str)                                                            \
    do                                                                                              \
    {                                                                                               \
        const enum StackError verify_error = stack_verify_func(stack);                              \
        if (verify_error)                                                                           \
        {                                                                                           \
            stack_dumb_func(stack, (place_in_code_t)                                                \
                                   { .file = __FILE__, .func = __func__, .line = __LINE__ },        \
                            elem_to_str);                                                           \
            lassert(false, "stack error: %s", stack_strerror(verify_error));                        \
        }                                                                                           \
    } while (0)

#else  /*NDEBUG*/

#define STACK_VERIFY(stack) do {} while (0)

#endif /*NDEBUG*/


#endif /*SRC_VERIFICATION_H*/