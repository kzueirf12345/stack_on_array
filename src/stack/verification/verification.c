#include <errno.h>

#include "verification.h"

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

enum StackError stack_verify_func(const stack_t* const stack)
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
        CASE_ENUM_TO_STRING_(STACK_ERROR_SIZE_GREATER_CAPACITY);
        CASE_ENUM_TO_STRING_(STACK_ERROR_SIZE_OVERFLOW);
        CASE_ENUM_TO_STRING_(STACK_ERROR_CAPACITY_OVERFLOW);
        CASE_ENUM_TO_STRING_(STACK_ERROR_STACK_IS_NULL);
        CASE_ENUM_TO_STRING_(STACK_ERROR_STANDART_ERRNO);
        CASE_ENUM_TO_STRING_(STACK_ERROR_UNKNOWN);

        default: 
            fprintf(stderr, "unknown STACK_ERROR enum, it's soooo bad\n");
            return NULL;
    }

    return NULL;
}
#undef CASE_ENUM_TO_STRING_


#ifndef NDEBUG

//TODO - mprotect
// NOTE - fuck this shit
#define LOGG_AND_FPRINTF_(format, ...)                                                              \
        do {                                                                                        \
        logg(LOG_LEVEL_DETAILS_DUMB, format, ##__VA_ARGS__);                                        \
        if (fprintf(stderr, format "\n", ##__VA_ARGS__) < 0)                                        \
            return STACK_ERROR_STANDART_ERRNO;                                                      \
        } while(0)

#define INIT_CONST_BUF_CHECK_POISON_(buf_name, check_val, poison_val)                               \
        const typeof(check_val) buf_name = !(check_val) ? (poison_val) : (check_val)

enum StackError stack_dumb_func(const stack_t* const stack, place_in_code_t place_in_code)
{
    LOGG_AND_FPRINTF_("==STACK DUMB==");

    INIT_CONST_BUF_CHECK_POISON_(file_buf,  place_in_code.file, "UNKNOWN");
    INIT_CONST_BUF_CHECK_POISON_(func_buf,  place_in_code.func, "UNKNOWN");
    const int                    line_buf = place_in_code.line <= 0
                                            ? CODE_LINE_POISON
                                            : place_in_code.line;

    if (!stack)
    {
        LOGG_AND_FPRINTF_("stack_t [NULL] at %s:%d (%s())", file_buf, line_buf, func_buf);
        fprintf(stderr, "\n");
        return STACK_ERROR_SUCCESS;
    }

    INIT_CONST_BUF_CHECK_POISON_(stack_name_buf     ,  stack->name           , "UNKNOWN");
    INIT_CONST_BUF_CHECK_POISON_(stack_file_burn_buf,  stack->place_burn.file, "UNKNOWN");
    INIT_CONST_BUF_CHECK_POISON_(stack_func_burn_buf,  stack->place_burn.func, "UNKNOWN");
    const int                    stack_line_burn_buf = stack->place_burn.line <= 0
                                                       ? CODE_LINE_POISON
                                                       : stack->place_burn.line;


    LOGG_AND_FPRINTF_("stack_t %s[%p] at %s:%d (%s()) bUUUrn at %s:%d (%s())",
                     stack_name_buf, stack,
                     file_buf, line_buf, func_buf,
                     stack_file_burn_buf, stack_line_burn_buf, stack_func_burn_buf);

    LOGG_AND_FPRINTF_("{");
    LOGG_AND_FPRINTF_("\tsize     = %zu", stack->size);
    LOGG_AND_FPRINTF_("\tcapacity = %zu", stack->capacity);

    if (!stack->data)
    {
        LOGG_AND_FPRINTF_("\tdata[NULL]");
        LOGG_AND_FPRINTF_("}");
        fprintf(stderr, "\n");
        return STACK_ERROR_SUCCESS;
    }

    LOGG_AND_FPRINTF_("\tdata[%p]", stack->data);
    LOGG_AND_FPRINTF_("\t{");

    size_t ind_count = MIN(stack->size, MIN(stack->capacity, 100));

    for (size_t ind = 0; ind < ind_count; ++ind)
    {
        if (ind < stack->size)
            LOGG_AND_FPRINTF_("\t\t*[%-3zu] = %d", ind, stack->data[ind]);
        else
            LOGG_AND_FPRINTF_("\t\t [%-3zu] = %d", ind, stack->data[ind]);
    }

    LOGG_AND_FPRINTF_("\t}");   

    LOGG_AND_FPRINTF_("}");    
    fprintf(stderr, "\n");
    return STACK_ERROR_SUCCESS;
}
#undef LOGG_AND_FPRINTF_
#undef INIT_CONST_BUF_CHECK_POISON_

#endif /*NDEBUG*/