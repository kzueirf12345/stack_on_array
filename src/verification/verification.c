#include "verification.h"


static enum PtrState is_valid_ptr_(const void* ptr);

enum PtrState
{
    PTR_STATES_VALID   = 0,
    PTR_STATES_NULL    = 1,
    PTR_STATES_INVALID = 2,
    PTR_STATES_ERROR   = 3
};
static_assert(PTR_STATES_VALID == 0);

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
        CASE_ENUM_TO_STRING_(STACK_ERROR_STANDARD_ERRNO);
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
        CASE_ENUM_TO_STRING_(STACK_ERROR_DATA_CONTROL_HASH_NEQUAL);
#endif /*HASH_PROTECT*/
        CASE_ENUM_TO_STRING_(STACK_ERROR_UNKNOWN);

        default: 
            fprintf(stderr, "Unknown StackError enum, it's soooo bad\n");
            return NULL;
    }

    return NULL;
}
#undef CASE_ENUM_TO_STRING_


enum StackError data_to_lX_str(const void* const data, const size_t size, char* const * lX_str,
                               const size_t lX_str_size)
{
    lassert(data, "");
    lassert(size, "");
    lassert(lX_str, "");
    
    char temp_str[sizeof(uint64_t) * 4] = {};
    for (size_t offset = 0; offset < size; 
         offset += (size - offset >= sizeof(uint64_t) ? sizeof(uint64_t) : sizeof(uint8_t)))
    {
        if (size - offset >= sizeof(uint64_t))
        {
            if (snprintf(temp_str, sizeof(uint64_t) * 4, "%lX", 
                         *(const uint64_t*)((const char*)data + offset)) <= 0)
            {
                perror("Can't snprintf byte on temp_str");
                return STACK_ERROR_STANDARD_ERRNO;
            }
        }
        else
        {
            if (snprintf(temp_str, sizeof(uint8_t) * 4, "%lX", 
                         *(const uint8_t*)((const char*)data + offset)) <= 0)
            {
                perror("Can't snprintf byte on temp_str");
                return STACK_ERROR_STANDARD_ERRNO;
            }
        }

        if (!strncat(*lX_str, temp_str, lX_str_size))
        {
            perror("Can't stract lX_str and temp_str");
            return STACK_ERROR_STANDARD_ERRNO;
        }
    }

    return STACK_ERROR_SUCCESS;
}


#ifndef NDEBUG

IF_HASH(static uint64_t stack_hash_(const void* const elem, const size_t elem_size, 
                                    const size_t first_skip_size);)

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
        return STACK_ERROR_STANDARD_ERRNO;
    
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
        return STACK_ERROR_STANDARD_ERRNO;
    
    default:
        fprintf(stderr, "Unknown PtrState enum, it's soooo bad\n");
        return STACK_ERROR_UNKNOWN;
    }

    if (stack->elem_size > STACK_MAX_ELEM_SIZE)
        return STACK_ERROR_ELEM_SIZE_OVERFLOW;

    if (stack->size > STACK_MAX_SIZE)
        return STACK_ERROR_SIZE_OVERFLOW;

    if (stack->capacity > STACK_MAX_CAPACITY)
        return STACK_ERROR_CAPACITY_OVERFLOW;
    
    if (stack->elem_size == 0)
        return STACK_ERROR_ELEM_SIZE_IS_NULL;


    if (stack->size > stack->capacity)
        return STACK_ERROR_SIZE_GREATER_CAPACITY;

#ifdef PENGUIN_PROTECT

    const penguin_t PENGUIN_bump = PENGUIN_CONTROL;

    if (memcmp((char*)stack->data - 1 * PENGUIN_T_SIZE                , &PENGUIN_bump,
                PENGUIN_T_SIZE))
        return STACK_ERROR_DATA_PENGUIN_LEFT;
    if (memcmp((char*)stack->data + stack->elem_size * stack->capacity, &PENGUIN_bump, 
                PENGUIN_T_SIZE))
        return STACK_ERROR_DATA_PENGUIN_RIGHT;

#endif /*PENDUIN_PROTECT*/

#ifdef HASH_PROTECT

    if (stack->stack_hash
        != stack_hash_(stack, STACK_T_SIZE, IF_PENGUIN(PENGUIN_T_SIZE) + 2 * sizeof(uint64_t)))
        return STACK_ERROR_STACK_CONTROL_HASH_NEQUAL;
    
    if (stack->data_hash != stack_hash_(stack->data, stack->capacity * stack->elem_size, 0))
        return STACK_ERROR_DATA_CONTROL_HASH_NEQUAL;

#endif/*HASH_PROTECT*/
    
    
    if (errno)
        return STACK_ERROR_STANDARD_ERRNO;

    return STACK_ERROR_SUCCESS;
}

#ifdef HASH_PROTECT
static uint64_t stack_hash_(const void* const elem, const size_t elem_size, 
                            const size_t first_skip_size)
{
    lassert(elem, "");
    lassert(first_skip_size <= elem_size, "");

    uint64_t hash_val = 0;
    for (size_t offset = first_skip_size;
         offset + sizeof(uint64_t) <= elem_size; 
         offset += sizeof(uint64_t))
    {
        hash_val = hash_val * 33 + *(const uint64_t*)((const char*)elem + offset);                                                                                                                          /*vova loh*/
    }

    const size_t remainder = elem_size % sizeof(uint64_t);
    for (size_t offset = elem_size - remainder; offset < elem_size; ++offset)
    {
        hash_val = hash_val * 33 + *(const uint8_t*)((const char*)elem + offset);
    }

    return hash_val;
}
#endif /*HASH_PROTECT*/


static const char* handle_invalid_ptr_(const void* const check_ptr);

#define LOGG_AND_FPRINTF_(format, ...)                                                              \
        do {                                                                                        \
        logg(LOG_LEVEL_DETAILS_DUMB, format, ##__VA_ARGS__);                                        \
        fprintf(stderr, format "\n", ##__VA_ARGS__);                                                \
        } while(0)
void stack_dumb_func(const stack_t* const stack, place_in_code_t place_in_code,
                     enum StackError (*elem_to_str)(const void* const elem, const size_t elem_size,
                                                    char* const * str, const size_t mx_str_size))
{
    LOGG_AND_FPRINTF_("==STACK DUMB==");

    const char* stack_buf = handle_invalid_ptr_(stack);
    const char* file_buf  = handle_invalid_ptr_(place_in_code.file);
    const char* func_buf  = handle_invalid_ptr_(place_in_code.func);
    file_buf =  file_buf  ? file_buf :          place_in_code.file;
    func_buf =  func_buf  ? func_buf :          place_in_code.func;
    const int   line_buf  =                     place_in_code.line <= 0
                                                ? CODE_LINE_POISON
                                                : place_in_code.line;

    if (stack_buf)
    {
        LOGG_AND_FPRINTF_("stack_t [%s] at %s:%d (%s())", stack_buf, file_buf, line_buf, func_buf);
        fprintf(stderr, "\n");
        return;
    }

    const char*           stack_name_buf      = handle_invalid_ptr_(stack->name           );
    const char*           stack_file_burn_buf = handle_invalid_ptr_(stack->place_burn.file);
    const char*           stack_func_burn_buf = handle_invalid_ptr_(stack->place_burn.func);
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

    const char* stack_data_buf = handle_invalid_ptr_(stack->data);
    if (stack_data_buf)
    {
        LOGG_AND_FPRINTF_("\tdata[%s]", stack_data_buf);
        LOGG_AND_FPRINTF_("}");
        fprintf(stderr, "\n");
        return;
    }

    LOGG_AND_FPRINTF_("\tdata[%p]", stack->data);
    LOGG_AND_FPRINTF_("\t{");

    size_t ind_count = MIN(stack->size, MIN(stack->capacity, 100));

    for (size_t ind = 0; ind < ind_count; ++ind)
    {
        char* str_elem = calloc(4, stack->elem_size);
        if (!str_elem)
        {
            LOGG_AND_FPRINTF_("\t\tERROR");
            break;
        }

        if (!elem_to_str)
            data_to_lX_str((char*)stack->data + ind * stack->elem_size, stack->elem_size, &str_elem,
                           4 * stack->elem_size);
        else
            elem_to_str   ((char*)stack->data + ind * stack->elem_size, stack->elem_size, &str_elem,
                           4 * stack->elem_size);

        const char* str_elem_buf = handle_invalid_ptr_(str_elem);

        if (ind < stack->size)
            LOGG_AND_FPRINTF_("\t\t*[%-3zu] = %s", ind, str_elem_buf ? str_elem_buf : str_elem);
        else
            LOGG_AND_FPRINTF_("\t\t [%-3zu] = %s", ind, str_elem_buf ? str_elem_buf : str_elem);
            
        free(str_elem); str_elem = NULL;
    }

    LOGG_AND_FPRINTF_("\t}");   

    LOGG_AND_FPRINTF_("}");    
    fprintf(stderr, "\n");
}
#undef LOGG_AND_FPRINTF_

static const char* handle_invalid_ptr_(const void* const check_ptr)
{
    switch (is_valid_ptr_(check_ptr))
    {
    case PTR_STATES_VALID:
        return NULL;
    case PTR_STATES_NULL:
        return "NULL";
    case PTR_STATES_INVALID:
        return "INVALID";
    case PTR_STATES_ERROR:
        return "ERROR";
    default:
        return "UNKNOWN";
    }

    return "MIPT SHIT";
}

#endif /*NDEBUG*/


static enum PtrState is_valid_ptr_(const void* ptr)
{
    if (errno)
        return PTR_STATES_ERROR;

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

