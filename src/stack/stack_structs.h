#ifndef SRC_STACK_STRUCTS_H
#define SRC_STACK_STRUCTS_H

#include <stdlib.h>


typedef int stack_elem_t; // REVIEW

typedef struct stack_t
{
#ifndef NDEBUG
    const char* name;
    const char* file_burn;
    const char* func_burn;
    int line_burn;
#endif /*NDEBUG*/

    stack_elem_t* data;
    size_t size;
    size_t capacity;
} stack_t;


#ifndef NDEBUG

#define STACK_INIT(name) #name, __FILE__, __func__, __LINE__

#else  /*NDEBUG*/

#define STACK_INIT(name)

#endif /*NDEBUG*/

#endif /*SRC_STACK_STRUCTS_H*/