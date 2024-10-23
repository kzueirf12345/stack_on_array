#ifndef SRC_STACK_TEST_H
#define SRC_STACK_TEST_H

#include "../stack_funcs.h"

#ifdef STACK_TEST_MODE

enum StackError stack_test(const size_t actions_count, const size_t elem_size);

#endif /*STACK_TEST_MODE*/

#endif /*SRC_STACK_TEST_H*/