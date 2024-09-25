#include <memory.h>

#include "stack_funcs.h"
#include "verification/verification.h"

void stack_ctor(stack_t* const stack, const size_t start_capacity)
{
    lassert(stack, "");

    stack->capacity = start_capacity;
    stack->size = 0;
    stack->data = calloc(start_capacity, sizeof(stack_elem_t));
    
    STACK_VERIFY(stack);
}

void stack_dtor(stack_t* const stack)
{
    STACK_VERIFY(stack);

    stack->capacity = 0;
    stack->size = 0;
    free(stack->data); stack->data = NULL;
}

static enum StackError stack_resize_(stack_t* stack);

enum StackError stack_push(stack_t* stack, const stack_elem_t elem)
{
    STACK_VERIFY(stack);

    const enum StackError stack_resize_error = stack_resize_(stack);
    if (stack_resize_error != STACK_ERROR_SUCCESS)
    {
        fprintf(stderr, "Can't stack resize\n");
        return stack_resize_error;
    }

    stack->data[stack->size++] = elem;

    STACK_VERIFY(stack);
    return STACK_ERROR_SUCCESS;
}

static void* recalloc_(void* ptrmem, const size_t old_number, const size_t number,
                       const size_t size);

static enum StackError stack_resize_(stack_t* stack)
{
    STACK_VERIFY(stack);

    if (stack->size == stack->capacity)
    {
        if (!recalloc_(stack->data, stack->capacity, stack->capacity * 2, sizeof(*stack->data)))
        {
            perror("Can't recalloc_");
            return STACK_ERROR_STANDART_ERRNO;
        }
        stack->capacity <<= 1;
    }

    STACK_VERIFY(stack);
    return STACK_ERROR_SUCCESS;
}

static void* recalloc_(void* ptrmem, const size_t old_number, const size_t number, 
                       const size_t size)
{
    lassert(ptrmem, "");
    lassert(old_number < number, ""); // TODO - recalloc down

    if (!realloc(ptrmem, number * size))
    {
        perror("Can't realloc in recalloc_");
        return NULL;
    }

    if (!memset((char*)ptrmem + old_number * size, 0, (number - old_number) * size))
    {
        perror("Can't memset in recalloc_");
        return NULL;
    }

    return ptrmem;
}