#ifndef SRC_STACK_TEST_H
#define SRC_STACK_TEST_H

#define HASH_PROTECT
#define PENGUIN_PROTECT
#include "../stack_funcs.h"

#ifdef TEST_MODE

enum StackError stack_test(const size_t actions_count, const size_t elem_size);

static enum StackError randn_(void* const data, const size_t size);
static enum StackError data_as_lX_str_(const void* const data, const size_t size, 
                                       char* const * lx_str);

static enum StackError handle_pop_(stack_t* const stack_t, const size_t elem_size, 
                                    const size_t action_ind);
static enum StackError handle_push_(stack_t* const stack, const size_t elem_size,
                                    const size_t action_ind, char** push_history);

enum StackError stack_test(const size_t actions_count, const size_t elem_size)
{
    lassert(actions_count, "No work - no result!");

    logg(LOG_LEVEL_DETAILS_DUMB, "==START TEST==");
    logg(LOG_LEVEL_DETAILS_DUMB, "actions_count = %zu", actions_count);

    enum StackError error_handler = STACK_ERROR_SUCCESS;

    stack_t stack = { STACK_INIT(stack) };
    error_handler = stack_ctor(&stack, elem_size, 0);
    if (error_handler)
    {
        fprintf(stderr, "Can't stack_ctor\n");
        return error_handler;
    }

    char* push_history = calloc(actions_count, 2 * elem_size + 1);
    if (!push_history)
    {
        perror("Can't calloc push_history");
        return STACK_ERROR_STANDART_ERRNO;
    }

    size_t count_pushes = 0;
    size_t possible_pop_count = 0;
    for (size_t action_ind = 1; action_ind <= actions_count; ++action_ind)
    {
        const bool do_pop = (possible_pop_count && (rand() % 2));
        if (do_pop)
        {
            --possible_pop_count;
            
            error_handler = handle_pop_(&stack, elem_size, action_ind);
            if (error_handler)
            {
                fprintf(stderr, "Can't handle_pop_\n");
                return error_handler;
            }
        }
        else
        {
            ++possible_pop_count;
            ++count_pushes;

            error_handler = handle_push_(&stack, elem_size, action_ind, &push_history);
            if (error_handler)
            {
                fprintf(stderr, "Can't handle_push_\n");
                return error_handler;
            }
        }

    }
    
    logg(LOG_LEVEL_DETAILS_DUMB, "PUSH_HISTORY: %s", push_history);
    free(push_history); push_history = NULL;

    stack_dtor(&stack);

    return STACK_ERROR_SUCCESS;
}

static enum StackError randn_(void* const data, const size_t size)
{
    lassert(data, "");
    lassert(size, "");

    for (size_t byte_ind = 0; byte_ind < size; ++byte_ind)
    {
        uint8_t byte = (uint8_t)(rand() % (1 << 8));
        if (!memcpy((char*)data + byte_ind, &byte, 1))
        {
            perror("Can't memcpy byte in data");
            return STACK_ERROR_STANDART_ERRNO;
        }
    }
    return STACK_ERROR_SUCCESS;
}

static enum StackError handle_pop_(stack_t* const stack, const size_t elem_size, 
                                    const size_t action_ind)
{
    STACK_VERIFY(stack);
    lassert(elem_size, "");

    enum StackError error_handler = STACK_ERROR_SUCCESS;

    void* pop_elem = calloc(1, elem_size);
    if (!pop_elem)
    {
        perror("Can't calloc pop_elem");
        return STACK_ERROR_STANDART_ERRNO;
    }

    error_handler = stack_pop(stack, pop_elem);
    if (error_handler)
    {
        fprintf(stderr, "Can't stack_pop\n");
        return error_handler;
    }
    
    char* pop_elem_str = calloc(elem_size, 4);
    if (!pop_elem_str)
    {
        perror("Can't calloc pop_elem_str");
        return STACK_ERROR_STANDART_ERRNO;
    }

    error_handler = data_as_lX_str_(pop_elem, elem_size, &pop_elem_str);
    if (error_handler)
    {
        fprintf(stderr, "Can't data_as_lX_str_");
        return error_handler;
    }
    logg(LOG_LEVEL_DETAILS_DUMB, "Actions №%-3zu. POP:  0x%s", action_ind,  pop_elem_str);

    free(pop_elem_str); pop_elem_str = NULL;
    free(pop_elem); pop_elem = NULL;

    STACK_VERIFY(stack);
    return error_handler;
}

static enum StackError handle_push_(stack_t* const stack, const size_t elem_size,
                                    const size_t action_ind, char** push_history)
{
    STACK_VERIFY(stack);
    lassert(elem_size, "");

    enum StackError error_handler = STACK_ERROR_SUCCESS;

    void* push_elem = calloc(1, elem_size);
    error_handler = randn_(push_elem, elem_size);
    if (error_handler)
    {
        fprintf(stderr, "Can't randn_ push_elem\n");
        return error_handler;
    }
    
    error_handler = stack_push(stack, push_elem);
    if (error_handler)
    {
        fprintf(stderr, "Can't stack_push\n");
        return error_handler;
    }

    char* push_elem_str = calloc(1, 4 * elem_size + 1);
    if (!push_elem_str)
    {
        perror("Can't calloc push_elem_str");
        return STACK_ERROR_STANDART_ERRNO;
    }

    error_handler = data_as_lX_str_(push_elem, elem_size, &push_elem_str);
    if (error_handler)
    {
        fprintf(stderr, "Can't data_as_lX_str_");
        return error_handler;
    }
    logg(LOG_LEVEL_DETAILS_DUMB, "Actions №%-3zu. PUSH: 0x%s", action_ind,  push_elem_str);

    
    if (!strcat(strcat(strcat(*push_history, "0x"), push_elem_str), " "))
    {
        perror("Can't memcpu push_elem to push_history");
        return STACK_ERROR_STANDART_ERRNO;
    }

    free(push_elem_str); push_elem_str = NULL;
    free(push_elem); push_elem = NULL;

    return error_handler;
}

static enum StackError data_as_lX_str_(const void* const data, const size_t size, 
                                       char* const * lx_str)
{
    lassert(data, "");
    lassert(size, "");
    lassert(lx_str, "");

    
    char temp_str[sizeof(uint64_t) * 4] = {};
    for (size_t offset = 0; offset + sizeof(uint64_t) <= size; offset += sizeof(uint64_t))
    {
        if (snprintf(temp_str, sizeof(uint64_t) * 4, "%lX", 
                     *(const uint64_t*)((const char*)data + offset)) <= 0)
        {
            perror("Can't snprintf byte on temp_str");
            return STACK_ERROR_STANDART_ERRNO;
        }

        if (!strcat(*lx_str, temp_str))
        {
            perror("Can't stract lx_str and temp_str");
            return STACK_ERROR_STANDART_ERRNO;
        }
    }

    for (size_t offset = size - size % sizeof(uint64_t); offset < size; 
         ++offset)
    {
        if (snprintf(temp_str, sizeof(uint8_t) * 4, "%lX", 
                     *(const uint8_t*)((const char*)data + offset)) <= 0)
        {
            perror("Can't snprintf byte on temp_str");
            return STACK_ERROR_STANDART_ERRNO;
        }

        if (!strcat(*lx_str, temp_str))
        {
            perror("Can't stract lx_str and temp_str");
            return STACK_ERROR_STANDART_ERRNO;
        }
    }

    return STACK_ERROR_SUCCESS;
}

#endif /*TEST_MODE*/

#endif /*SRC_STACK_TEST_H*/