#ifndef SRC_STACK_STRUCTS_H
#define SRC_STACK_STRUCTS_H

#include <stdlib.h>


typedef int stack_elem_t; // REVIEW

typedef struct stack_t
{
    stack_elem_t* data;
    size_t size;
    size_t capacity;
} stack_t;


#endif /*SRC_STACK_STRUCTS_H*/