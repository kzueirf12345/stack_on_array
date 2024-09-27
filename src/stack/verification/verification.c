#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
// #include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

#include "verification.h"


#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))


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
        CASE_ENUM_TO_STRING_(STACK_ERROR_DATA_IS_INVALID);
        CASE_ENUM_TO_STRING_(STACK_ERROR_SIZE_GREATER_CAPACITY);
        CASE_ENUM_TO_STRING_(STACK_ERROR_SIZE_OVERFLOW);
        CASE_ENUM_TO_STRING_(STACK_ERROR_CAPACITY_OVERFLOW);
        CASE_ENUM_TO_STRING_(STACK_ERROR_STACK_IS_NULL);
        CASE_ENUM_TO_STRING_(STACK_ERROR_STACK_IS_INVALID);
        CASE_ENUM_TO_STRING_(STACK_ERROR_STANDART_ERRNO);
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
    const int                 line_buf = place_in_code.line <= 0
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
    LOGG_AND_FPRINTF_("\tsize     = %zu", stack->size);
    LOGG_AND_FPRINTF_("\tcapacity = %zu", stack->capacity);

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

#endif /*NDEBUG*/