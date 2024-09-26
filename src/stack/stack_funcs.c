#include <memory.h>

#include "stack_funcs.h"
#include "verification/verification.h"


#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))


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

    size_t new_capacity = 0;
    if (stack->size == stack->capacity)
    {
        new_capacity = stack->capacity * 2;
        new_capacity = MAX(1, new_capacity);
    }
    else
    {
        size_t size_to_down_resize = stack->capacity / 4;
        size_to_down_resize = MAX(1, size_to_down_resize);
        if (stack->size <= size_to_down_resize)
            new_capacity = size_to_down_resize;
    }

    if (new_capacity)
    {
        stack_elem_t* temp_data = recalloc_(stack->data, stack->capacity, new_capacity,
                                            sizeof(*stack->data));
        if (!temp_data)
        {
            perror("Can't recalloc_");
            return STACK_ERROR_STANDART_ERRNO;
        }
        stack->data = temp_data; temp_data = NULL;
        stack->capacity = new_capacity;
    }

    STACK_VERIFY(stack);
    return STACK_ERROR_SUCCESS;
}

static void* recalloc_(void* ptrmem, const size_t old_number, const size_t number, 
                       const size_t size)
{
    lassert(ptrmem, "");
    lassert(number, "");

    if (number == old_number)
        return ptrmem;

    if (!(ptrmem = realloc(ptrmem, number * size)))
    {
        perror("Can't realloc in recalloc_");
        return NULL;
    }

    if (number > old_number
        && !memset((char*)ptrmem + old_number * size, 0, (number - old_number) * size))
    {
        perror("Can't memset in recalloc_");
        return NULL;
    }

    return ptrmem;
}


enum StackError stack_pop(stack_t* stack, stack_elem_t* const elem)
{
    STACK_VERIFY(stack);
    lassert(stack->size > 0, "");

    if (elem) *elem = stack->data[stack->size - 1];

    stack->data[--stack->size] = 0;

    const enum StackError stack_resize_error = stack_resize_(stack);
    if (stack_resize_error != STACK_ERROR_SUCCESS)
    {
        fprintf(stderr, "Can't stack resize\n");
        return stack_resize_error;
    }

    STACK_VERIFY(stack);
    return STACK_ERROR_SUCCESS;
}


stack_elem_t stack_back(const stack_t stack)
{
    STACK_VERIFY(&stack);
    lassert(stack.size > 0, "");

    return stack.data[stack.size - 1];
}