#include <errno.h>

#include "verification.h"

#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))

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


const char* stack_strerror(const enum StackError error)
{
    
#define CASE_ENUM_TO_STRING_(error) case error: return #error

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

#undef CASE_ENUM_TO_STRING

    return NULL;
}

#ifndef NDEBUG

//TODO - mprotect
enum StackError stack_dumb_func(stack_t* const stack, const char* file, const char* func,
                                const int line)
{
#define LOGG_AND_FPRINTF_(format, ...)                                                              \
        do {                                                                                        \
        logg(LOG_LEVEL_DETAILS_DUMB, format, ##__VA_ARGS__);                                        \
        if (fprintf(stderr, format "\n", ##__VA_ARGS__) < 0)                                        \
            return STACK_ERROR_STANDART_ERRNO;                                                      \
        } while(0)

    LOGG_AND_FPRINTF_("==STACK DUMB==");

#define NULL_STR_TO_UNKNOWN_(str) if (!str) str = "UNKNOWN"

    NULL_STR_TO_UNKNOWN_(file);
    NULL_STR_TO_UNKNOWN_(func);

    if (!stack)
    {
        LOGG_AND_FPRINTF_("stack_t [NULL] at %s:%d (%s())", file, line, func);
        fprintf(stderr, "\n");
        return STACK_ERROR_SUCCESS;
    }

    NULL_STR_TO_UNKNOWN_(stack->name);
    NULL_STR_TO_UNKNOWN_(stack->file_burn);
    NULL_STR_TO_UNKNOWN_(stack->func_burn);

#undef NULL_STR_TO_UNKNOWN_

    LOGG_AND_FPRINTF_("stack_t %s[%p] at %s:%d (%s()) bUUUrn at %s:%d (%s())",
                     stack->name, stack,
                     file, line, func,
                     stack->file_burn, stack->line_burn, stack->func_burn);

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

    size_t ind_count = min(stack->size, min(stack->capacity, 100));


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

#undef LOGG_AND_FPRINTF_
}

#endif /*NDEBUG*/