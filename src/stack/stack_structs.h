#ifndef SRC_STACK_STRUCTS_H
#define SRC_STACK_STRUCTS_H

#include <stdlib.h>
#include <stdint.h>

#include "../config.h"
#include "../logger/logger.h"

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#ifndef NDEBUG
#define IF_DEBUG(...) __VA_ARGS__
#define IF_ELSE_DEBUG(smth, other_smth) smth
#else /*NDEBUG*/
#define IF_DEBUG(...)
#define IF_ELSE_DEBUG(smth, other_smth) other_smth
#endif /*NDEBUG*/


#ifdef NDEBUG
#undef PENGUIN_PROTECT
#undef HASH_PROTECT
#endif /*NDEBUG*/


#define PENGUIN_CONTROL (0xBAADC0FEDEADBABEu)
typedef uint64_t penguin_t;
#define PENGUIN_T_SIZE sizeof(penguin_t)

#ifdef  PENGUIN_PROTECT
#define IF_PENGUIN(...) __VA_ARGS__
#define IF_ELSE_PENGUIN(smth, other_smth) smth
#else /*PENGUIN_PROTECT*/
#define IF_PENGUIN(...) /*YOU aren't PENGUIN*/
#define IF_ELSE_PENGUIN(smth, other_smth) other_smth
#endif/*PENGUIN_PROTECT*/


#ifdef  HASH_PROTECT
#define IF_HASH(...) __VA_ARGS__
#define IF_ELSE_HASH(smth, other_smth) smth
#else /*HASH_PROTECT*/
#define IF_HASH(...)
#define IF_ELSE_HASH(smth, other_smth) other_smth
#endif/*HASH_PROTECT*/


typedef struct stack_t
{
#ifdef PENGUIN_PROTECT
    penguin_t PENGUIN_LEFT_;
#endif /*PENGUIN_PROTECT*/


#ifdef HASH_PROTECT
    uint64_t stack_hash;
    uint64_t data_hash;
#endif /*HASH_PROTECT*/

#ifndef NDEBUG
    const char* name;
    place_in_code_t place_burn;
#endif /*NDEBUG*/

    void* data;
    size_t elem_size;
    size_t size;
    size_t capacity;


#ifdef PENGUIN_PROTECT
    penguin_t PENGUIN_RIGHT_;
#endif /*PENGUIN_PROTECT*/
} stack_t;

#define STACK_T_SIZE sizeof(stack_t)


#endif /*SRC_STACK_STRUCTS_H*/