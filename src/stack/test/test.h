#ifndef SRC_STACK_TEST_H
#define SRC_STACK_TEST_H

#define HASH_PROTECT
#define PENGUIN_PROTECT
#include "../stack_funcs.h"

#ifdef TEST_MODE

enum StackError stack_test(const size_t actions_count, const size_t elem_size);

#endif /*TEST_MODE*/

#endif /*SRC_STACK_TEST_H*/