#ifndef SRC_STACK_H
#define SRC_STACK_H

#include <memory.h>
#include <sys/mman.h> 

#include "verification/verification.h"


enum StackError stack_ctor_NOT_USE_(uint64_t* const stack_num, const size_t elem_size, 
                                    const size_t start_capacity, const char* const name,
                                    const place_in_code_t place_in_code);

#ifndef NDEBUG

#define STACK_CTOR(stack_num, elem_size, start_capacity)                                            \
        stack_ctor_NOT_USE_(stack_num, elem_size, start_capacity, #stack_num,                       \
                            (place_in_code_t)                                                       \
                            { .file = __FILE__, .func = __func__, .line = __LINE__ });

#else  /*NDEBUG*/

#define STACK_CTOR(stack_num, elem_size, start_capacity)                                            \
        stack_ctor_NOT_USE_(stack_num, elem_size, start_capacity, #stack_num, NULL,                 \
                            (place_in_code_t){});

#endif  /*NDEBUG*/

void stack_dtor(uint64_t* const stack_num);

enum StackError stack_push(const uint64_t* const stack_num, const void* const elem);
enum StackError stack_pop (const uint64_t* const stack_num, void* const elem);
enum StackError stack_back(const uint64_t* const stack_num, void* const elem);

bool stack_is_empty(const uint64_t* const stack_num);


//==================================================================================================

#endif /*SRC_STACK_H*/