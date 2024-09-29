#ifndef SRC_STACK_STRUCTS_H
#define SRC_STACK_STRUCTS_H

#include <stdlib.h>
#include <stdint.h>

#include "../logger/logger.h"


#ifndef NDEBUG
#define PENGUIN_PROTECT
#endif /*NDEBUG*/

#define PENGUIN_CONTROL (0xBAADC0FEDEADBABEu)
#define PENGUIN_TYPE uint64_t
#define PENGUIN_SIZE sizeof(PENGUIN_TYPE)

#ifdef  PENGUIN_PROTECT
#define IF_PENGUIN(...) __VA_ARGS__
#define IF_ELSE_PENGUIN(smth, other_smth) smth
#else /*PENGUIN_PROTECT*/
#define IF_PENGUIN(...) /*YOU aren't PENGUIN*/
#define IF_ELSE_PENGUIN(smth, other_smth) other_smth
#endif/*PENGUIN_PROTECT*/



typedef struct stack_t
{
#ifdef PENGUIN_PROTECT
    const PENGUIN_TYPE PENGUIN_LEFT_;
#endif /*PENGUIN_PROTECT*/


#ifndef NDEBUG
    const char* const name;
    place_in_code_t place_burn;
#endif /*NDEBUG*/

    void* data;
    size_t elem_size;
    // IF_PENGUIN(size_t user_elem_size;)
    size_t size;
    size_t capacity;


#ifdef PENGUIN_PROTECT
    const PENGUIN_TYPE PENGUIN_RIGHT_;
#endif /*PENGUIN_PROTECT*/
} stack_t;


#ifndef NDEBUG
#ifndef  PENGUIN_PROTECT

#define STACK_INIT(name)                                                                            \
        #name, (place_in_code_t){ .file = __FILE__, .func = __func__, .line = __LINE__ }

#else  /*PENGUIN_PROTECT*/

#define STACK_INIT(name)                                                                            \
        .PENGUIN_LEFT_ = PENGUIN_CONTROL,                                                           \
        #name, (place_in_code_t){ .file = __FILE__, .func = __func__, .line = __LINE__, },          \
        .PENGUIN_RIGHT_ = PENGUIN_CONTROL

#endif /*PENGUIN_PROTECT*/
#else  /*NDEBUG*/

#define STACK_INIT(name)

#endif /*NDEBUG*/


#endif /*SRC_STACK_STRUCTS_H*/