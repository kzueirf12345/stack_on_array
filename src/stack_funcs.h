#ifndef SRC_STACK_H
#define SRC_STACK_H

#include <memory.h>
#include <sys/mman.h> 

#include "verification/verification.h"


enum StackError stack_ctor_NOT_USE_(stack_key_t* const stack_num, const size_t elem_size, 
                                    const size_t start_capacity, const char* const name,
                                    const place_in_code_t place_in_code);

#ifndef NDEBUG

#define STACK_CTOR(stack_num, elem_size, start_capacity)                                            \
        stack_ctor_NOT_USE_(stack_num, elem_size, start_capacity, #stack_num,                       \
                            (place_in_code_t)                                                       \
                            { .file = __FILE__, .func = __func__, .line = __LINE__ })

void stack_dumb_NOT_USE_(const stack_key_t stack_num, place_in_code_t place_in_code,
                         enum StackError (*elem_to_str)(const void* const elem, 
                                                        const size_t elem_size,
                                                        char* const * str, 
                                                        const size_t mx_str_size));

#define STACK_DUMB(stack_num, elem_to_str)                                                          \
        stack_dumb_NOT_USE_(stack_num, (place_in_code_t)                                            \
                                       { .file = __FILE__, .func = __func__, .line = __LINE__ },    \
                            elem_to_str);

#else  /*NDEBUG*/

#define STACK_CTOR(stack_num, elem_size, start_capacity)                                            \
        stack_ctor_NOT_USE_(stack_num, elem_size, start_capacity, #stack_num,                       \
                            (place_in_code_t){})

#define STACK_DUMB(stack_num, elem_to_str) do {} while()

#endif  /*NDEBUG*/

void stack_dtor(stack_key_t* const stack_num);

typedef int(*stack_cmp)(const void* const first, const void* const second);

enum StackError stack_push      (const stack_key_t* const stack_num, const void* const elem);
enum StackError stack_pop       (const stack_key_t* const stack_num, void* const elem);
enum StackError stack_back      (const stack_key_t stack_num, void* const elem);
void*           stack_begin     (const stack_key_t stack_num);
void*           stack_get       (const stack_key_t stack_num, const size_t ind);
void*           stack_find      (const stack_key_t stack_num, const void* const elem, stack_cmp cmp);
size_t          stack_find_push (const stack_key_t* const stack_num, const void* const elem);

bool stack_is_empty(const stack_key_t stack_num);
size_t stack_size  (const stack_key_t stack_num);

//==================================================================================================

#endif /*SRC_STACK_H*/