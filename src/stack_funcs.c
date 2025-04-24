#include "stack_funcs.h"


IF_HASH(static uint64_t stack_hash_(const void* const elem, const size_t elem_size);)
IF_HASH(static void     stack_rehash_(stack_t* const stack);)


// //I STEAL IT I STEAL IT I STEAL IT I STEAL IT I STEAL IT I STEAL IT I STEAL IT I STEAL IT I STEAL IT
// // https://stackoverflow.com/questions/33010010/how-to-generate-random-64-bit-unsigned-integer-in-c
// #define IMAX_BITS_(m) ((m)/((m)%255+1) / 255%255*8 + 7-86/((m)%255+12))
// #define RAND_MAX_WIDTH_ IMAX_BITS_(RAND_MAX)
// static_assert((RAND_MAX & (RAND_MAX + 1u)) == 0, "RAND_MAX not a Mersenne number");

// static stack_key_t rand64_(void) {
//   stack_key_t r = 0;
//   for (int i = 0; i < 64; i += RAND_MAX_WIDTH_) {
//     r <<= RAND_MAX_WIDTH_;
//     r ^= (unsigned) rand();
//   }
//   return r;
// }
// //I STEAL IT I STEAL IT I STEAL IT I STEAL IT I STEAL IT I STEAL IT I STEAL IT I STEAL IT I STEAL IT

// static stack_key_t STACK_KEY = 0;

enum StackError stack_ctor_NOT_USE_(stack_key_t* const stack_num, const size_t elem_size, 
                                    const size_t start_capacity, const char* const name,
                                    const place_in_code_t place_in_code)
{
    lassert(stack_num, "");
    lassert(name, "");
    lassert(place_in_code.file, "");
    lassert(place_in_code.func, "");
    lassert(place_in_code.line, "");   
    (void)name;
    (void)place_in_code; 

    stack_t* const stack = calloc(1, STACK_T_SIZE);
    if (!stack)
    {
        perror("Can't calloc stack");
        return STACK_ERROR_STANDARD_ERRNO;
    }
    *stack_num = (stack_key_t)stack;

#ifndef NDEBUG
    stack->name = name;
    stack->place_burn.file = place_in_code.file;
    stack->place_burn.func = place_in_code.func;
    stack->place_burn.line = place_in_code.line;
#endif /*NDEBUG*/
#ifdef PENGUIN_PROTECT
    stack->PENGUIN_LEFT_  = PENGUIN_CONTROL;
    stack->PENGUIN_RIGHT_ = PENGUIN_CONTROL;
#endif /*PENGUIN_CONTROL*/

    if (elem_size == 0)
        return STACK_ERROR_ELEM_SIZE_IS_NULL;
    if (elem_size > STACK_MAX_ELEM_SIZE)
        return STACK_ERROR_ELEM_SIZE_OVERFLOW;
    if (start_capacity > STACK_MAX_CAPACITY)
        return STACK_ERROR_CAPACITY_OVERFLOW;

    stack->elem_size = elem_size;
    stack->capacity = start_capacity;
    stack->size = 0;
    stack->data = calloc(stack->elem_size * stack->capacity IF_PENGUIN(+ 2 * PENGUIN_T_SIZE),
                         sizeof(char));

    if (!stack->data && (stack->capacity IF_PENGUIN(+ 2 * PENGUIN_T_SIZE)) != 0)
    {
        perror("Can't calloc stack->data");
        return STACK_ERROR_STANDARD_ERRNO;
    }

#ifdef PENGUIN_PROTECT
    const penguin_t PENGUIN_bump = PENGUIN_CONTROL;
    if (!memcpy((char*)stack->data, &PENGUIN_bump, PENGUIN_T_SIZE))
    {
        perror("Can't memcpy left PENGUIN");
        return STACK_ERROR_STANDARD_ERRNO;
    }
    stack->data = (char*)stack->data + 1 * PENGUIN_T_SIZE;
    if (!memcpy((char*)stack->data + stack->capacity * stack->elem_size, &PENGUIN_bump, 
                PENGUIN_T_SIZE))
    {
        perror("Can't memcpy right PENGUIN");
        return STACK_ERROR_STANDARD_ERRNO;
    }
#endif /*PENGUIN_PROTECT*/

    IF_HASH(stack_rehash_(stack);)
    
    STACK_VERIFY(stack, NULL);
    return STACK_ERROR_SUCCESS;
}

void stack_dtor(stack_key_t* const stack_num)
{
    lassert(stack_num, "");
    stack_t* stack = (stack_t*)(*stack_num);
    STACK_VERIFY(stack, NULL);

#ifndef NDEBUG
    stack->capacity = 0;
    stack->size = 0;
    stack->elem_size = 0;
#endif /*NDEBUG*/
    
    IF_PENGUIN(stack->data = (char*)stack->data - 1 * PENGUIN_T_SIZE;)
    free(stack->data); IF_DEBUG(stack->data = NULL;)
    free(stack);       IF_DEBUG(stack       = NULL;)
    
    *stack_num = 0;
}

//=====================================================

static enum StackError stack_resize_(stack_t* stack);

enum StackError stack_push(const stack_key_t* const stack_num, const void* const elem)
{
    lassert(stack_num, "");
    stack_t* const stack = (stack_t* const)(*stack_num);
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
        return STACK_ERROR_STANDARD_ERRNO;
    }
    ++stack->size;

    IF_HASH(stack_rehash_(stack);)

    STACK_VERIFY(stack, NULL);
    return STACK_ERROR_SUCCESS;
}

enum StackError stack_pop (const stack_key_t* const stack_num, void* const elem)
{
    lassert(stack_num, "");
    stack_t* const stack = (stack_t* const)(*stack_num);
    STACK_VERIFY(stack, NULL);

    lassert(stack->size > 0, "");
    // lassert(elem           , "");

    if (elem &&
        !memcpy(elem, (char*)stack->data + (stack->size - 1) * stack->elem_size, stack->elem_size))
    {
        perror("Can't push back stack elem with memcpy");
        return STACK_ERROR_STANDARD_ERRNO;
    }

    if (!memset((char*)stack->data + (stack->size - 1) * stack->elem_size, 0, stack->elem_size))
    {
        perror("Can't memset zero back elem in stack pop");
        return STACK_ERROR_STANDARD_ERRNO;
    }
    --stack->size;

    IF_HASH(stack_rehash_(stack);)

    const enum StackError stack_resize_error = stack_resize_(stack);
    if (stack_resize_error != STACK_ERROR_SUCCESS)
    {
        fprintf(stderr, "Can't stack resize\n");
        return stack_resize_error;
    }

    IF_HASH(stack_rehash_(stack);)

    STACK_VERIFY(stack, NULL);
    return STACK_ERROR_SUCCESS;
}

enum StackError stack_back(const stack_key_t stack_num, void* const elem)
{
    lassert(stack_num, "");
    stack_t* const stack = (stack_t* const)(stack_num);
    STACK_VERIFY(stack, NULL);

    lassert(stack->size > 0, "");
    lassert(elem          , "");

    if(!memcpy(elem, (char*)stack->data + (stack->size - 1) * stack->elem_size, stack->elem_size))
    {
        perror("Can't stack_back elem with memcpy");
        return STACK_ERROR_STANDARD_ERRNO;
    }

    STACK_VERIFY(stack, NULL);
    return STACK_ERROR_SUCCESS;
}

void* stack_begin(const stack_key_t stack_num)
{
    lassert(stack_num, "");
    stack_t* const stack = (stack_t* const)(stack_num);
    STACK_VERIFY(stack, NULL);

    return stack->data;
}

void* stack_get(const stack_key_t stack_num, const size_t ind)
{
    lassert(stack_num, "");
    stack_t* const stack = (stack_t* const)(stack_num);
    STACK_VERIFY(stack, NULL);

    return (char*)stack->data + stack->elem_size * ind;
}

void* stack_find(const stack_key_t stack_num, const void* const elem, stack_cmp cmp) 
{
    lassert(stack_num, "");
    stack_t* const stack = (stack_t* const)(stack_num);
    STACK_VERIFY(stack, NULL);
    lassert(elem, "");

    for (size_t ind = 0; ind < stack->size; ++ind)
    {
        if (!cmp)
        {
            if (memcmp((char*)stack->data + stack->elem_size * ind, elem, stack->elem_size) == 0)
            {
                return (char*)stack->data + stack->elem_size * ind;
            }
        }
        else
        {
            if (cmp((char*)stack->data + stack->elem_size * ind, elem) == 0)
            {
                return (char*)stack->data + stack->elem_size * ind;
            }
        }
    }
    return NULL;
}

#define STACK_ERROR_HANDLE_(call_func, ...)                                                         \
    do {                                                                                            \
        const enum StackError stack_error_handler = call_func;                                      \
        if (stack_error_handler)                                                                    \
        {                                                                                           \
            fprintf(stderr, "Can't " #call_func". Stack error: %s\n",                               \
                            stack_strerror(stack_error_handler));                                   \
            __VA_ARGS__                                                                             \
            return SIZE_MAX;                                                                        \
        }                                                                                           \
    } while(0)

size_t stack_find_push(const stack_key_t* const stack_num, const void* const elem)
{
    lassert(stack_num, "");
    stack_t* const stack = (stack_t* const)(*stack_num);
    STACK_VERIFY(stack, NULL);
    lassert(elem, "");

    void* ans = NULL;
    if ((ans = stack_find(*stack_num, elem, NULL)))
    {
        return (size_t)((char*)ans - (char*)stack->data) / stack->elem_size;
    }

    STACK_ERROR_HANDLE_(stack_push(stack_num, elem));

    return stack->size - 1;
}
#undef STACK_ERROR_HANDLE_

bool stack_is_empty(const stack_key_t stack_num)
{
    lassert(stack_num, "");
    stack_t* const stack = (stack_t* const)(stack_num);
    STACK_VERIFY(stack, NULL);

    return stack->size == 0;
}

size_t stack_size(const stack_key_t stack_num)
{
    lassert(stack_num, "");
    stack_t* const stack = (stack_t* const)(stack_num);
    STACK_VERIFY(stack, NULL);

    return stack->size;
}

//=====================================================

static enum StackError stack_resize_data_(const stack_t* const stack, void** data, 
                                          const size_t new_capacity);

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
        size_to_down_resize = MAX(1, size_to_down_resize) + 1;
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

        IF_HASH(stack_rehash_(stack);)
    }

    STACK_VERIFY(stack, NULL);
    return STACK_ERROR_SUCCESS;
}


static void* recalloc_(void* ptrmem, const size_t old_number, const size_t old_size,
                                     const size_t     number, const size_t     size);

static enum StackError stack_resize_data_(const stack_t* const stack, void** data, 
                                          const size_t new_capacity)
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
        return STACK_ERROR_STANDARD_ERRNO;
    }
    *data = temp_data; temp_data = NULL;

#ifdef PENGUIN_PROTECT
    *data = (char*)*data + 1 * PENGUIN_T_SIZE;
    const penguin_t PENGUIN_bump = PENGUIN_CONTROL;

    if (!memcpy((char*)*data + new_capacity * stack->elem_size, &PENGUIN_bump, 
                PENGUIN_T_SIZE))
    {
        perror("Can't memcpy right PENGUIN");
        return STACK_ERROR_STANDARD_ERRNO;
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

//====================================================

#ifndef NDEBUG

void stack_dumb_NOT_USE_(const stack_key_t stack_num, place_in_code_t place_in_code,
                         enum StackError (*elem_to_str)(const void* const elem, 
                                                        const size_t elem_size,
                                                        char* const * str, 
                                                        const size_t mx_str_size))
{
    stack_t* const stack = (stack_t* const)(stack_num);

    stack_dumb_func_NOT_USE_(stack, place_in_code, elem_to_str);
}

#endif /*NDEBUG*/

//====================================================

#ifdef HASH_PROTECT
static uint64_t stack_hash_(const void* const elem, const size_t elem_size)
{
    lassert(elem, "");

    uint64_t hash_val = 0;
    for (size_t offset = 0; offset + sizeof(uint64_t) <= elem_size; offset += sizeof(uint64_t))
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

static void stack_rehash_(stack_t* const stack)
{
    lassert(stack, "");
    lassert(stack->data, "");

    stack->data_hash  = stack_hash_(stack->data, stack->capacity * stack->elem_size);
    stack->stack_hash = stack_hash_((char*)stack 
                                        + (IF_PENGUIN(PENGUIN_T_SIZE) + 2 * sizeof(uint64_t)), 
                                    STACK_T_SIZE 
                                        - (IF_PENGUIN(PENGUIN_T_SIZE) + 2 * sizeof(uint64_t)));
}
#endif /*HASH_PROTECT*/