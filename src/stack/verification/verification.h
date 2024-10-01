#ifndef SRC_VERIFICATION_H
#define SRC_VERIFICATION_H

#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
// #include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>

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


//=================================================================================================


#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

IF_HASH(uint64_t stack_hash_(const void* const elem, size_t elem_size, size_t first_skip_size);)


enum PtrState
{
    PTR_STATES_VALID   = 0,
    PTR_STATES_NULL    = 1,
    PTR_STATES_INVALID = 2,
    PTR_STATES_ERROR   = 3
};
static_assert(PTR_STATES_VALID == 0);

static enum PtrState is_valid_ptr_(const void* ptr);

enum StackError stack_verify_func(const stack_t* const stack)
{
    switch (is_valid_ptr_(stack))
    {
    case PTR_STATES_VALID:
        break;
    case PTR_STATES_NULL:
        return STACK_ERROR_STACK_IS_NULL;
    case PTR_STATES_INVALID:
        return STACK_ERROR_STACK_IS_INVALID;
    case PTR_STATES_ERROR:
        return STACK_ERROR_STANDART_ERRNO;
    
    default:
        fprintf(stderr, "Unknown PtrState enum, it's soooo bad\n");
        return STACK_ERROR_UNKNOWN;
    }

#ifdef PENGUIN_PROTECT

    if (stack->PENGUIN_LEFT_  != PENGUIN_CONTROL)
        return STACK_ERROR_STACK_PENGUIN_LEFT;
    if (stack->PENGUIN_RIGHT_ != PENGUIN_CONTROL)
        return STACK_ERROR_STACK_PENGUIN_RIGHT;

#endif /*PENDUIN_PROTECT*/

    switch (is_valid_ptr_(stack->data))
    {
    case PTR_STATES_VALID:
        break;
    case PTR_STATES_NULL:
        return STACK_ERROR_STACK_IS_NULL;
    case PTR_STATES_INVALID:
        return STACK_ERROR_STACK_IS_INVALID;
    case PTR_STATES_ERROR:
        return STACK_ERROR_STANDART_ERRNO;
    
    default:
        fprintf(stderr, "Unknown PtrState enum, it's soooo bad\n");
        return STACK_ERROR_UNKNOWN;
    }

    if (stack->elem_size > STACK_MAX_ELEM_SIZE_VALUE)
        return STACK_ERROR_ELEM_SIZE_OVERFLOW;

    if (stack->size > STACK_MAX_SIZE_VALUE)
        return STACK_ERROR_SIZE_OVERFLOW;

    if (stack->capacity > STACK_MAX_CAPACITY_VALUE)
        return STACK_ERROR_CAPACITY_OVERFLOW;
    
    if (stack->elem_size == 0)
        return STACK_ERROR_ELEM_SIZE_IS_NULL;


    if (stack->size > stack->capacity)
        return STACK_ERROR_SIZE_GREATER_CAPACITY;

#ifdef PENGUIN_PROTECT

    const PENGUIN_TYPE PENGUIN_bump = PENGUIN_CONTROL;

    if (memcmp((char*)stack->data - 1 * PENGUIN_T_SIZE                , &PENGUIN_bump,
                PENGUIN_T_SIZE))
        return STACK_ERROR_DATA_PENGUIN_LEFT;
    if (memcmp((char*)stack->data + stack->elem_size * stack->capacity, &PENGUIN_bump, 
                PENGUIN_T_SIZE))
        return STACK_ERROR_DATA_PENGUIN_RIGHT;

#endif /*PENDUIN_PROTECT*/


#ifdef HASH_PROTECT

    if (!stack->stack_check)
        return STACK_ERROR_STACK_CHECK_IS_NULL;

#ifdef PENGUIN_PROTECT
    if (stack->stack_check->PENGUIN_LEFT_  != PENGUIN_CONTROL)
        return STACK_ERROR_STACK_CHECK_PENGUIN_LEFT;
    if (stack->stack_check->PENGUIN_RIGHT_ != PENGUIN_CONTROL)
        return STACK_ERROR_STACK_CHECK_PENGUIN_RIGHT;
#endif /*PENDUIN_PROTECT*/

    if (stack_hash_(stack             , STACK_T_SIZE, 
                    IF_PENGUIN(PENGUIN_T_SIZE) + sizeof(stack_t*) + sizeof(stack->data_check)) 
     != stack_hash_(stack->stack_check, STACK_T_SIZE,
                    IF_PENGUIN(PENGUIN_T_SIZE) + sizeof(stack_t*) + sizeof(stack->data_check)))
        return STACK_ERROR_STACK_CONTROL_HASH_NEQUAL;
    

    if (!stack->data_check)
        return STACK_ERROR_DATA_CHECK_IS_NULL;

#ifdef PENGUIN_PROTECT
    if (memcmp((char*)stack->data_check - 1 * PENGUIN_T_SIZE                , &PENGUIN_bump,
                PENGUIN_T_SIZE))
        return STACK_ERROR_DATA_CHECK_PENGUIN_LEFT;
    if (memcmp((char*)stack->data_check + stack->elem_size * stack->capacity, &PENGUIN_bump, 
                PENGUIN_T_SIZE))
        return STACK_ERROR_DATA_CHECK_PENGUIN_RIGHT;
#endif /*PENDUIN_PROTECT*/

    // fprintf(stderr, "size: %zu\n", stack->capacity * stack->elem_size);
    if (stack_hash_(stack->data      , stack->capacity * stack->elem_size, 0) !=
        stack_hash_(stack->data_check, stack->capacity * stack->elem_size, 0))
        return STACK_ERROR_DATA_CONTROL_HASH_NEQUAL;

#endif/*HASH_PROTECT*/
    
    
    if (errno)
        return STACK_ERROR_STANDART_ERRNO;

    return STACK_ERROR_SUCCESS;
}

#define CASE_ENUM_TO_STRING_(error) case error: return #error
const char* stack_strerror(const enum StackError error)
{
    switch(error)
    {
        CASE_ENUM_TO_STRING_(STACK_ERROR_SUCCESS);
        CASE_ENUM_TO_STRING_(STACK_ERROR_DATA_IS_NULL);
        CASE_ENUM_TO_STRING_(STACK_ERROR_DATA_IS_INVALID);
        CASE_ENUM_TO_STRING_(STACK_ERROR_SIZE_GREATER_CAPACITY);
        CASE_ENUM_TO_STRING_(STACK_ERROR_SIZE_OVERFLOW);
        CASE_ENUM_TO_STRING_(STACK_ERROR_CAPACITY_OVERFLOW);
        CASE_ENUM_TO_STRING_(STACK_ERROR_STACK_IS_NULL);
        CASE_ENUM_TO_STRING_(STACK_ERROR_STACK_IS_INVALID);
        CASE_ENUM_TO_STRING_(STACK_ERROR_STANDART_ERRNO);
        CASE_ENUM_TO_STRING_(STACK_ERROR_ELEM_SIZE_OVERFLOW);
        CASE_ENUM_TO_STRING_(STACK_ERROR_ELEM_SIZE_IS_NULL);
#ifdef PENGUIN_PROTECT
        CASE_ENUM_TO_STRING_(STACK_ERROR_STACK_PENGUIN_LEFT);
        CASE_ENUM_TO_STRING_(STACK_ERROR_STACK_PENGUIN_RIGHT);
        CASE_ENUM_TO_STRING_(STACK_ERROR_DATA_PENGUIN_LEFT);
        CASE_ENUM_TO_STRING_(STACK_ERROR_DATA_PENGUIN_RIGHT);
#endif /*PENGUIN_PROTECT*/
#ifdef HASH_PROTECT
        CASE_ENUM_TO_STRING_(STACK_ERROR_STACK_CONTROL_HASH_NEQUAL);
        CASE_ENUM_TO_STRING_(STACK_ERROR_STACK_CHECK_IS_NULL);
        CASE_ENUM_TO_STRING_(STACK_ERROR_DATA_CONTROL_HASH_NEQUAL);
        CASE_ENUM_TO_STRING_(STACK_ERROR_DATA_CHECK_IS_NULL);
#ifdef PENGUIN_PROTECT
        CASE_ENUM_TO_STRING_(STACK_ERROR_STACK_CHECK_PENGUIN_LEFT);
        CASE_ENUM_TO_STRING_(STACK_ERROR_STACK_CHECK_PENGUIN_RIGHT);
        CASE_ENUM_TO_STRING_(STACK_ERROR_DATA_CHECK_PENGUIN_LEFT);
        CASE_ENUM_TO_STRING_(STACK_ERROR_DATA_CHECK_PENGUIN_RIGHT);
#endif /*PENGUIN_PROTECT*/
#endif /*HASH_PROTECT*/
        CASE_ENUM_TO_STRING_(STACK_ERROR_UNKNOWN);

        default: 
            fprintf(stderr, "Unknown StackError enum, it's soooo bad\n");
            return NULL;
    }

    return NULL;
}
#undef CASE_ENUM_TO_STRING_


#ifndef NDEBUG

// NOTE - fuck this shit
#define LOGG_AND_FPRINTF_(format, ...)                                                              \
        do {                                                                                        \
        logg(LOG_LEVEL_DETAILS_DUMB, format, ##__VA_ARGS__);                                        \
        if (fprintf(stderr, format "\n", ##__VA_ARGS__) < 0)                                        \
            return STACK_ERROR_STANDART_ERRNO;                                                      \
        } while(0)

#define INIT_CONST_BUF_CHECK_STR_(buf_name, check_str)                                              \
        const char* buf_name = NULL;                                                                \
        switch (is_valid_ptr_(check_str))                                                           \
        {                                                                                           \
        case PTR_STATES_VALID:                                                                      \
            buf_name = NULL;                                                                        \
            break;                                                                                  \
        case PTR_STATES_NULL:                                                                       \
            buf_name = "NULL";                                                                      \
            break;                                                                                  \
        case PTR_STATES_INVALID:                                                                    \
            buf_name = "INVALID";                                                                   \
            break;                                                                                  \
        case PTR_STATES_ERROR:                                                                      \
            buf_name = "ERROR";                                                                     \
            break;                                                                                  \
        default:                                                                                    \
            buf_name = "UNKNOWN";                                                                   \
            break;                                                                                  \
        }

enum StackError stack_dumb_func(const stack_t* const stack, place_in_code_t place_in_code)
{
    LOGG_AND_FPRINTF_("==STACK DUMB==");

    INIT_CONST_BUF_CHECK_STR_(stack_buf, stack);
    INIT_CONST_BUF_CHECK_STR_(file_buf , place_in_code.file);
    INIT_CONST_BUF_CHECK_STR_(func_buf , place_in_code.func);
    file_buf = file_buf ? file_buf : place_in_code.file;
    func_buf = func_buf ? func_buf : place_in_code.func;
    const int             line_buf = place_in_code.line <= 0
                                     ? CODE_LINE_POISON
                                     : place_in_code.line;

    if (stack_buf)
    {
        LOGG_AND_FPRINTF_("stack_t [%s] at %s:%d (%s())", stack_buf, file_buf, line_buf, func_buf);
        fprintf(stderr, "\n");
        return STACK_ERROR_STACK_IS_NULL;
    }

    INIT_CONST_BUF_CHECK_STR_(stack_name_buf     ,  stack->name           );
    INIT_CONST_BUF_CHECK_STR_(stack_file_burn_buf,  stack->place_burn.file);
    INIT_CONST_BUF_CHECK_STR_(stack_func_burn_buf,  stack->place_burn.func);
    stack_name_buf      = stack_name_buf      ? stack_name_buf      : stack->name;
    stack_file_burn_buf = stack_file_burn_buf ? stack_file_burn_buf : stack->place_burn.file;
    stack_func_burn_buf = stack_func_burn_buf ? stack_func_burn_buf : stack->place_burn.func;
    const int             stack_line_burn_buf = stack->place_burn.line <= 0
                                                ? CODE_LINE_POISON
                                                : stack->place_burn.line;


    LOGG_AND_FPRINTF_("stack_t %s[%p] at %s:%d (%s()) bUUUrn at %s:%d (%s())",
                     stack_name_buf, stack,
                     file_buf, line_buf, func_buf,
                     stack_file_burn_buf, stack_line_burn_buf, stack_func_burn_buf);

    LOGG_AND_FPRINTF_("{");
    LOGG_AND_FPRINTF_("\telem_size = %zu", stack->elem_size);
    LOGG_AND_FPRINTF_("\tsize      = %zu", stack->size);
    LOGG_AND_FPRINTF_("\tcapacity  = %zu", stack->capacity);

    INIT_CONST_BUF_CHECK_STR_(stack_data_buf, stack->data);
    if (stack_data_buf)
    {
        LOGG_AND_FPRINTF_("\tdata[%s]", stack_data_buf);
        LOGG_AND_FPRINTF_("}");
        fprintf(stderr, "\n");
        return STACK_ERROR_DATA_IS_NULL;
    }

    LOGG_AND_FPRINTF_("\tdata[%p]", stack->data);
    LOGG_AND_FPRINTF_("\t{");

    size_t ind_count = MIN(stack->size, MIN(stack->capacity, 100));

    for (size_t ind = 0; ind < ind_count; ++ind)
    {
        if (ind < stack->size)
            LOGG_AND_FPRINTF_("\t\t*[%-3zu] = %d",
                              ind, *(int*)((char*)stack->data + ind * stack->elem_size)); 
                              // TODO - check void* print, maybe define type with ifndef
        else
            LOGG_AND_FPRINTF_("\t\t [%-3zu] = %d",
                              ind, *(int*)((char*)stack->data + ind * stack->elem_size));
    }

    LOGG_AND_FPRINTF_("\t}");   

    LOGG_AND_FPRINTF_("}");    
    fprintf(stderr, "\n");
    return STACK_ERROR_SUCCESS;
}

#undef LOGG_AND_FPRINTF_
#undef INIT_CONST_BUF_CHECK_POISON_

#endif /*NDEBUG*/

static enum PtrState is_valid_ptr_(const void* ptr)
{
    if (!ptr) return PTR_STATES_NULL;

    int fd = open(ptr, 0, 0);

    if (fd == -1 && errno == EFAULT)
    {
        errno = 0;
        return PTR_STATES_INVALID;
    }
    else if (fd != -1 && close(fd) == -1)
        return PTR_STATES_ERROR;
    else 
        errno = 0;

    return PTR_STATES_VALID;
}

#ifdef HASH_PROTECT
uint64_t stack_hash_(const void* const elem, size_t elem_size, size_t first_skip_size)
{
    lassert(elem, "");
    lassert(first_skip_size <= elem_size, "");

    uint64_t hash_val = 0;
    for (size_t offset = first_skip_size;
         offset + sizeof(uint64_t) <= elem_size; 
         offset += sizeof(uint64_t))
    {
        hash_val = hash_val * 31 + *(const uint64_t*)((const char*)elem + offset);                                                                                                                          /*vova loh*/
    }

    const size_t remainder = elem_size % sizeof(uint64_t);
    for (size_t offset = elem_size - remainder; offset < elem_size; ++offset)
    {
        hash_val = hash_val * 31 + *(const uint8_t*)((const char*)elem + offset);
    }

    return hash_val;
}
#endif /*HASH_PROTECT*/


#endif /*SRC_VERIFICATION_H*/