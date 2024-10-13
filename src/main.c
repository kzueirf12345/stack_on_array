#include "stack/test/test.h"

// TODO - README

#define error_handle(call_func, ...)                                                                \
    do{                                                                                             \
    error_handler = call_func;                                                                      \
    if (error_handler)                                                                              \
    {                                                                                               \
        fprintf(stderr, "Can't " #call_func ". Error: %s\n", stack_strerror(error_handler));        \
        __VA_ARGS__;                                                                                \
        return EXIT_FAILURE;                                                                        \
    }                                                                                               \
    }while(0)

int main()
{
    enum StackError error_handler = STACK_ERROR_SUCCESS;
    if (logger_ctor())
    {
        fprintf(stderr, "Can't init logger\n");
        return EXIT_FAILURE;
    }
    if (logger_set_level_details(LOG_LEVEL_DETAILS_ALL))
    {
        fprintf(stderr, "Can't set logger level details\n");
        return EXIT_FAILURE;
    }

    //----------------------------------------------------------


#ifdef TEST_MODE

    const size_t COUNT_TEST_ACTIONS = 10;
    const size_t TEST_ELEM_SIZE = 50;
    error_handler = stack_test(COUNT_TEST_ACTIONS, TEST_ELEM_SIZE);
    if (error_handler)
    {
        fprintf(stderr, "Can't stack_test. Error: %s\n", strerror(error_handler));
        return EXIT_FAILURE;
    }
    
#else /*TEST_MODE*/

    stack_key_t stack = 0;
    error_handle(STACK_CTOR(&stack, sizeof(size_t), 8), stack_dtor(&stack), logger_dtor());
    size_t elem = 123;
    error_handle(stack_push(&stack, &elem));
    size_t back_elem = 0;
    error_handle(stack_back(&stack, &back_elem), stack_dtor(&stack), logger_dtor());
    printf("elem: %zu\n", back_elem);
    stack_dtor(&stack);

#endif /*TEST_MODE*/

    //----------------------------------------------------------

    if (logger_dtor())
    {
        fprintf(stderr, "Can't destroy logger\n");
        return EXIT_FAILURE;
    }
    return 0;
}