#ifndef SRC_STACK_STRUCTS_H
#define SRC_STACK_STRUCTS_H

#include <stdlib.h>

#include "../logger/logger.h"

typedef int stack_elem_t;

typedef struct stack_t
{
#ifndef NDEBUG
    const char* const name;
    place_in_code_t place_burn;
#endif /*NDEBUG*/

    stack_elem_t* data;
    size_t size;
    size_t capacity;
} stack_t;


#ifndef NDEBUG

#define STACK_INIT(name)                                                                            \
        #name, (place_in_code_t){ .file = __FILE__, .func = __func__, .line = __LINE__ }

#else  /*NDEBUG*/

#define STACK_INIT(name)

#endif /*NDEBUG*/

#endif /*SRC_STACK_STRUCTS_H*/