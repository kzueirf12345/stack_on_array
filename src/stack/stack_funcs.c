#include "stack_funcs.h"


IF_HASH(static uint64_t stack_hash_(const void* const elem, const size_t elem_size, 
                                    const size_t first_skip_size);)


enum StackError stack_ctor(stack_t* const stack, const size_t elem_size, 
                           const size_t start_capacity)
{
    lassert(stack    , "");
    lassert(elem_size, "");
    IF_PENGUIN(lassert(stack->PENGUIN_LEFT_  == PENGUIN_CONTROL, "");)
    IF_PENGUIN(lassert(stack->PENGUIN_RIGHT_ == PENGUIN_CONTROL, "");)

    stack->elem_size = elem_size;
    stack->capacity = start_capacity;
    stack->size = 0;
    stack->data = calloc(stack->elem_size * stack->capacity IF_PENGUIN(+ 2 * PENGUIN_T_SIZE),
                         sizeof(char));

#ifdef PENGUIN_PROTECT
    const penguin_t PENGUIN_bump = PENGUIN_CONTROL;
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
    stack->data_hash  = stack_hash_(stack->data, stack->capacity * stack->elem_size, 0);
    stack->stack_hash = stack_hash_(stack, STACK_T_SIZE, 
                                    IF_PENGUIN(PENGUIN_T_SIZE) + 2 * sizeof(uint64_t));
#endif /*HASH_PROTECT*/
    
    STACK_VERIFY(stack, NULL);
    return STACK_ERROR_SUCCESS;
}

void stack_dtor(stack_t* const stack)
{
    STACK_VERIFY(stack, NULL);

    stack->capacity = 0;
    stack->size = 0;
    stack->elem_size = 0;
    IF_PENGUIN(stack->data = (char*)stack->data - 1 * PENGUIN_T_SIZE;)
    free(stack->data); stack->data = NULL;
}

//=====================================================

static enum StackError stack_resize_(stack_t* stack);

enum StackError stack_push(stack_t* const stack, const void* const elem)
{
    STACK_VERIFY(stack, NULL);
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
    stack->data_hash  = stack_hash_(stack->data, stack->capacity * stack->elem_size, 0);
    stack->stack_hash = stack_hash_(stack, STACK_T_SIZE, 
                                    IF_PENGUIN(PENGUIN_T_SIZE) + 2 * sizeof(uint64_t));
#endif /*HASH_PROTECT*/

    STACK_VERIFY(stack, NULL);
    return STACK_ERROR_SUCCESS;
}

enum StackError stack_pop(stack_t* const stack, void* const elem)
{
    STACK_VERIFY(stack, NULL);
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
    stack->data_hash  = stack_hash_(stack->data, stack->capacity * stack->elem_size, 0);
    stack->stack_hash = stack_hash_(stack, STACK_T_SIZE, 
                                    IF_PENGUIN(PENGUIN_T_SIZE) + 2 * sizeof(uint64_t));
#endif /*HASH_PROTECT*/

    const enum StackError stack_resize_error = stack_resize_(stack);
    if (stack_resize_error != STACK_ERROR_SUCCESS)
    {
        fprintf(stderr, "Can't stack resize\n");
        return stack_resize_error;
    }

#ifdef HASH_PROTECT
    stack->data_hash  = stack_hash_(stack->data, stack->capacity * stack->elem_size, 0);
    stack->stack_hash = stack_hash_(stack, STACK_T_SIZE, 
                                    IF_PENGUIN(PENGUIN_T_SIZE) + 2 * sizeof(uint64_t));
#endif /*HASH_PROTECT*/

    STACK_VERIFY(stack, NULL);
    return STACK_ERROR_SUCCESS;
}

enum StackError stack_back(const stack_t stack, void* const elem)
{
    STACK_VERIFY(&stack, NULL);
    lassert(stack.size > 0, "");
    lassert(elem          , "");

    if(!memcpy(elem, (char*)stack.data + (stack.size - 1) * stack.elem_size, stack.elem_size))
    {
        perror("Can't stack_back elem with memcpy");
        return STACK_ERROR_STANDART_ERRNO;
    }

    STACK_VERIFY(&stack, NULL);
    return STACK_ERROR_SUCCESS;
}

bool stack_is_empty(const stack_t stack)
{
    STACK_VERIFY(&stack, NULL);

    return stack.size == 0;
}

//=====================================================

static enum StackError stack_resize_data_(const stack_t* const stack, void** data, const size_t new_capacity);

static enum StackError stack_resize_(stack_t* stack)
{
    STACK_VERIFY(stack, NULL);

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
        enum StackError stack_resize_data_error 
                      = stack_resize_data_(stack, &stack->data, new_capacity);
        if (stack_resize_data_error != STACK_ERROR_SUCCESS)
        {
            fprintf(stderr, "Can't stack_resize_data_\n");
            return stack_resize_data_error;
        }

        stack->capacity = new_capacity;

#ifdef HASH_PROTECT
    stack->data_hash  = stack_hash_(stack->data, stack->capacity * stack->elem_size, 0);
    stack->stack_hash = stack_hash_(stack, STACK_T_SIZE, 
                                    IF_PENGUIN(PENGUIN_T_SIZE) + 2 * sizeof(uint64_t));
#endif /*HASH_PROTECT*/
    }

    STACK_VERIFY(stack, NULL);
    return STACK_ERROR_SUCCESS;
}


static void* recalloc_(void* ptrmem, const size_t old_number, const size_t old_size,
                                     const size_t     number, const size_t     size);

static enum StackError stack_resize_data_(const stack_t* const stack, void** data, const size_t new_capacity)
{
    lassert(stack, "");
    lassert(data, "");
    lassert(*data, "");
    lassert(new_capacity, "");
    
    IF_PENGUIN(*data = (char*)*data - 1 * PENGUIN_T_SIZE;)
    void* temp_data
        = recalloc_(*data, 
                    1, stack->capacity * stack->elem_size IF_PENGUIN(+ 2 * PENGUIN_T_SIZE),
                    1, new_capacity    * stack->elem_size IF_PENGUIN(+ 2 * PENGUIN_T_SIZE));
    if (!temp_data)
    {
        fprintf(stderr, "Can't recalloc_");
        return STACK_ERROR_STANDART_ERRNO;
    }
    *data = temp_data; temp_data = NULL;

#ifdef PENGUIN_PROTECT
    *data = (char*)*data + 1 * PENGUIN_T_SIZE;
    const penguin_t PENGUIN_bump = PENGUIN_CONTROL;

    if (!memcpy((char*)*data + new_capacity * stack->elem_size, &PENGUIN_bump, 
                PENGUIN_T_SIZE))
    {
        perror("Can't memcpy right PENGUIN");
        return STACK_ERROR_STANDART_ERRNO;
    }
#endif /*PENGUIN_PROTECT*/

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