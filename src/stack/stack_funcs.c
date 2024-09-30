#include <memory.h>
#include <sys/mman.h> 

#include "stack_funcs.h"
#include "verification/verification.h"


#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

enum StackError update_stack_data_check_(stack_t* const stack);
enum StackError update_stack_check_(stack_t* const stack);

enum StackError stack_ctor(stack_t* const stack, const size_t elem_size, const size_t start_capacity)
{
    lassert(stack    , "");
    lassert(elem_size, "");
#ifdef PENGUIN_PROTECT
    lassert(stack->PENGUIN_LEFT_  == PENGUIN_CONTROL, "PENGUIN_LEFT_: %lX" , stack->PENGUIN_LEFT_);
    lassert(stack->PENGUIN_RIGHT_ == PENGUIN_CONTROL, "PENGUIN_RIGHT_: %lX", stack->PENGUIN_RIGHT_);
#endif /*PENGUIN_PROTECT*/

    stack->elem_size = elem_size;
    stack->capacity = start_capacity;
    stack->size = 0;
    stack->data = calloc(1, stack->elem_size * stack->capacity IF_PENGUIN(+ 2 * PENGUIN_T_SIZE));
    
#ifdef PENGUIN_PROTECT
    const PENGUIN_TYPE PENGUIN_bump = PENGUIN_CONTROL;
    if (!memcpy((char*)stack->data, &PENGUIN_bump, PENGUIN_T_SIZE))
    {
        perror("Can't memcpy left PENGUIN");
        return STACK_ERROR_STANDART_ERRNO;
    }
    stack->data = (char*)stack->data + 1 * PENGUIN_T_SIZE;
    if (!memcpy((char*)stack->data + stack->capacity * stack->elem_size, &PENGUIN_bump, 
                PENGUIN_T_SIZE))
    {
        perror("Can't memcpy right PENGUIN");
        return STACK_ERROR_STANDART_ERRNO;
    }
#endif /*PENGUIN_PROTECT*/

#ifdef HASH_PROTECT     
    stack->stack_check = calloc(1, STACK_T_SIZE);
    if (!stack->stack_check)
    {
        perror("Can't calloc stack_check");
        return STACK_ERROR_STANDART_ERRNO;
    }

    stack->data_check = calloc(1, stack->capacity IF_PENGUIN(+ 2 * PENGUIN_T_SIZE));
    if (!stack->data_check)
    {
        perror("Can't calloc stack->data_check");
        return STACK_ERROR_STANDART_ERRNO;
    }

#ifdef PENGUIN_PROTECT
    if (!memcpy((char*)stack->data_check, &PENGUIN_bump, PENGUIN_T_SIZE))
    {
        perror("Can't memcpy left PENGUIN CHECK");
        return STACK_ERROR_STANDART_ERRNO;
    }
    stack->data_check = (char*)stack->data_check + 1 * PENGUIN_T_SIZE;
    if (!memcpy((char*)stack->data_check + stack->capacity * stack->elem_size, 
                &PENGUIN_bump, PENGUIN_T_SIZE))
    {
        perror("Can't memcpy right PENGUIN CHECK");
        return STACK_ERROR_STANDART_ERRNO;
    }
#endif /*PENGUIN_PROTECT*/

    enum StackError update_stack_check_error = update_stack_check_(stack);
    if (update_stack_check_error != STACK_ERROR_SUCCESS)
    {
        fprintf(stderr, "Can't update stack_check\n");
        return update_stack_check_error;
    }

    enum StackError update_stack_data_check_error = update_stack_data_check_(stack);
    if (update_stack_data_check_error != STACK_ERROR_SUCCESS)
    {
        fprintf(stderr, "Can't update stack_data_check\n");
        return update_stack_data_check_error;
    }
#endif /*HASH_PROTECT*/
    
    STACK_VERIFY(stack);
    return STACK_ERROR_SUCCESS;
}

void stack_dtor(stack_t* const stack)
{
    STACK_VERIFY(stack);

    stack->capacity = 0;
    stack->size = 0;
    stack->elem_size = 0;
    IF_PENGUIN(stack->data = (char*)stack->data - 1 * PENGUIN_T_SIZE;)
    free(stack->data); stack->data = NULL;
#ifdef HASH_PROTECT
    free(stack->stack_check); stack->stack_check = NULL;
    IF_PENGUIN(stack->data_check = (char*)stack->data_check - 1 * PENGUIN_T_SIZE;)
    free(stack->data_check); stack->data_check = NULL;
#endif /*HASH_PROTECT*/
}



static enum StackError stack_resize_(stack_t* stack);

enum StackError stack_push(stack_t* const stack, const void* const elem)
{
    STACK_VERIFY(stack);
    lassert(elem, "");

    const enum StackError stack_resize_error = stack_resize_(stack);
    if (stack_resize_error != STACK_ERROR_SUCCESS)
    {
        fprintf(stderr, "Can't stack resize\n");
        return stack_resize_error;
    }

    if (!memcpy((char*)stack->data + stack->size * stack->elem_size, elem, stack->elem_size))
    {
        perror("Can't push elem with memcpy");
        return STACK_ERROR_STANDART_ERRNO;
    }
    ++stack->size;

#ifdef HASH_PROTECT
    enum StackError update_stack_check_error = update_stack_check_(stack);
    if (update_stack_check_error != STACK_ERROR_SUCCESS)
    {
        fprintf(stderr, "Can't update stack_check\n");
        return update_stack_check_error;
    }

    enum StackError update_stack_data_check_error = update_stack_data_check_(stack);
    if (update_stack_data_check_error != STACK_ERROR_SUCCESS)
    {
        fprintf(stderr, "Can't update stack_data_check\n");
        return update_stack_data_check_error;
    }
#endif /*HASH_PROTECT*/

    STACK_VERIFY(stack);
    return STACK_ERROR_SUCCESS;
}


static void* recalloc_(void* ptrmem, const size_t old_number, const size_t old_size,
                                     const size_t     number, const size_t     size);

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
        IF_PENGUIN(stack->data = (char*)stack->data - 1 * PENGUIN_T_SIZE;)
        void* temp_data
            = recalloc_(stack->data, 
                        1, stack->capacity * stack->elem_size IF_PENGUIN(+ 2 * PENGUIN_T_SIZE),
                        1, new_capacity    * stack->elem_size IF_PENGUIN(+ 2 * PENGUIN_T_SIZE));
        if (!temp_data)
        {
            fprintf(stderr, "Can't recalloc_");
            return STACK_ERROR_STANDART_ERRNO;
        }
        stack->data = temp_data; temp_data = NULL;

#ifdef PENGUIN_PROTECT
        stack->data = (char*)stack->data + 1 * PENGUIN_T_SIZE;
        const PENGUIN_TYPE PENGUIN_bump = PENGUIN_CONTROL;
        if (!memcpy((char*)stack->data + new_capacity * stack->elem_size, &PENGUIN_bump, 
                    PENGUIN_T_SIZE))
        {
            perror("Can't memcpy right PENGUIN");
            return STACK_ERROR_STANDART_ERRNO;
        }
#endif /*PENGUIN_PROTECT*/

#ifdef HASH_PROTECT

        IF_PENGUIN(stack->data_check = (char*)stack->data_check - 1 * PENGUIN_T_SIZE;)
        temp_data
            = recalloc_(stack->data_check, 
                        1, stack->capacity * stack->elem_size IF_PENGUIN(+ 2 * PENGUIN_T_SIZE),
                        1, new_capacity    * stack->elem_size IF_PENGUIN(+ 2 * PENGUIN_T_SIZE));
        if (!temp_data)
        {
            fprintf(stderr, "Can't recalloc_ CHECK");
            return STACK_ERROR_STANDART_ERRNO;
        }
        stack->data_check = temp_data; temp_data = NULL;

#ifdef PENGUIN_PROTECT
        stack->data_check = (char*)stack->data_check + 1 * PENGUIN_T_SIZE;
        if (!memcpy((char*)stack->data_check + new_capacity * stack->elem_size, &PENGUIN_bump, 
                    PENGUIN_T_SIZE))
        {
            perror("Can't memcpy right PENGUIN CHECK");
            return STACK_ERROR_STANDART_ERRNO;
        }
#endif /*PENGUIN_PROTECT*/
#endif /*HASH_PROTECT*/

        stack->capacity = new_capacity;

#ifdef HASH_PROTECT
        enum StackError update_stack_check_error = update_stack_check_(stack);
        if (update_stack_check_error != STACK_ERROR_SUCCESS)
        {
            fprintf(stderr, "Can't update stack_check\n");
            return update_stack_check_error;
        }
#endif /*HASH_PROTECT*/
    }

    STACK_VERIFY(stack);
    return STACK_ERROR_SUCCESS;
}


static void* recalloc_(void* ptrmem, const size_t old_number, const size_t old_size,
                                     const size_t     number, const size_t     size)
{
    lassert(ptrmem, "");
    lassert(number, "");
    lassert(size  , "");

    if (number * size == old_number * old_size)
        return ptrmem;

    if (!(ptrmem = realloc(ptrmem, number * size)))
    {
        perror("Can't realloc in recalloc_");
        return NULL;
    }

    if (number * size > old_number * old_size
        && !memset((char*)ptrmem + old_number * old_size, 0, number * size - old_number * old_size))
    {
        perror("Can't memset in recalloc_");
        return NULL;
    }

    return ptrmem;
}


enum StackError stack_pop(stack_t* const stack, void* const elem)
{
    STACK_VERIFY(stack);
    lassert(stack->size > 0, "");
    lassert(elem           , "");

    if (elem &&
        !memcpy(elem, (char*)stack->data + (stack->size - 1) * stack->elem_size, stack->elem_size))
    {
        perror("Can't push back stack elem with memcpy");
        return STACK_ERROR_STANDART_ERRNO;
    }

    if (!memset((char*)stack->data + (stack->size - 1) * stack->elem_size, 0, stack->elem_size))
    {
        perror("Can't memset zero back elem in stack pop");
        return STACK_ERROR_STANDART_ERRNO;
    }
    --stack->size;

#ifdef HASH_PROTECT
    --stack->stack_check->size;

    enum StackError update_stack_data_check_error = update_stack_data_check_(stack);
    if (update_stack_data_check_error != STACK_ERROR_SUCCESS)
    {
        fprintf(stderr, "Can't update stack_data_check\n");
        return update_stack_data_check_error;
    }
#endif /*HASH_PROTECT*/

    const enum StackError stack_resize_error = stack_resize_(stack);
    if (stack_resize_error != STACK_ERROR_SUCCESS)
    {
        fprintf(stderr, "Can't stack resize\n");
        return stack_resize_error;
    }

#ifdef HASH_PROTECT
    enum StackError update_stack_check_error1 = update_stack_check_(stack);
    if (update_stack_check_error1 != STACK_ERROR_SUCCESS)
    {
        fprintf(stderr, "Can't update stack_check\n");
        return update_stack_check_error1;
    }
#endif /*HASH_PROTECT*/

    STACK_VERIFY(stack);
    return STACK_ERROR_SUCCESS;
}


enum StackError update_stack_data_check_(stack_t* const stack)
{
    if (!memcpy(stack->data_check, stack->data, stack->capacity * stack->elem_size))
    {
        perror("Can't memcpy stack->data in stack->data_check");
        return STACK_ERROR_STANDART_ERRNO;
    }
    return STACK_ERROR_SUCCESS;
}

enum StackError update_stack_check_(stack_t* const stack)
{
    if (!memcpy(stack->stack_check, stack, STACK_T_SIZE))
    {
        perror("Can't memcpy stack in stack_check");
        return STACK_ERROR_STANDART_ERRNO;
    }
    return STACK_ERROR_SUCCESS;
}



enum StackError stack_back(const stack_t stack, void* const elem)
{
    STACK_VERIFY(&stack);
    lassert(stack.size > 0, "");
    lassert(elem          , "");

    if(!memcpy(elem, (char*)stack.data + (stack.size - 1) * stack.elem_size, stack.elem_size))
    {
        perror("Can't stack_back elem with memcpy");
        return STACK_ERROR_STANDART_ERRNO;
    }

    STACK_VERIFY(&stack);
    return STACK_ERROR_SUCCESS;
}