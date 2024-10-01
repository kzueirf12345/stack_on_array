#define TEST_MODE
#include "stack/test/test.h"

// TODO - README
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

    // stack_t stack = { STACK_INIT(stack) };
    // stack_ctor(&stack, sizeof(int), 0);
    // for (int stack_elem = 0; stack_elem < 100; ++stack_elem)
    // {
    //     const enum StackError stack_push_error = stack_push(&stack, &stack_elem);
    //     if (stack_push_error != STACK_ERROR_SUCCESS)
    //     {
    //         if (logger_dtor())
    //             fprintf(stderr, "Can't destroy logger\n");
    //         stack_dtor(&stack);
    //         fprintf(stderr, "Can't stack push. STACK_ERROR: %s", stack_strerror(stack_push_error));
    //         return EXIT_FAILURE;
    //     }
    // }
    // for (int stack_elem = 0; stack_elem < 100; ++stack_elem)
    // {
    //     fprintf(stderr, "%-3d. ", stack_elem);
    //     fprintf(stderr, "stack_data[]: %-3d ",
    //            *(int*)((char*)stack.data + (99 - (size_t)stack_elem) * stack.elem_size));
    //     int stack_back_elem = 0;
    //     const enum StackError stack_back_error = stack_back(stack, &stack_back_elem);
    //     if (stack_back_error != STACK_ERROR_SUCCESS)
    //     {
    //         if (logger_dtor())
    //                 fprintf(stderr, "Can't destroy logger\n");
    //         stack_dtor(&stack);
    //         fprintf(stderr, "Can't stack push. STACK_ERROR: %s", stack_strerror(stack_back_error));
    //         return EXIT_FAILURE;
    //     }
    //     fprintf(stderr, "stack_back: %-3d ", stack_back_elem);
    //     int stack_pop_elem = 0;
    //     const enum StackError stack_pop_error = stack_pop(&stack, &stack_pop_elem);
    //     if (stack_pop_error != STACK_ERROR_SUCCESS)
    //     {
    //         if (logger_dtor())
    //             fprintf(stderr, "Can't destroy logger\n");
    //         stack_dtor(&stack);
    //         fprintf(stderr, "Can't stack push. STACK_ERROR: %s", stack_strerror(stack_pop_error));
    //         return EXIT_FAILURE;
    //     }
    //     fprintf(stderr, "stack_pop: %-3d\n", stack_pop_elem);
    // }
    // stack_dtor(&stack);
    // stack_t stack1 = { STACK_INIT(stack1) };
    // stack_ctor(&stack1, sizeof(char), 0);
    // for (char stack_elem = 'a'; stack_elem <= 'z'; ++stack_elem)
    // {
    //     const enum StackError stack_push_error = stack_push(&stack1, &stack_elem);
    //     if (stack_push_error != STACK_ERROR_SUCCESS)
    //     {
    //         if (logger_dtor())
    //             fprintf(stderr, "Can't destroy logger\n");
    //         stack_dtor(&stack);
    //         fprintf(stderr, "Can't stack1 push. STACK_ERROR: %s", stack_strerror(stack_push_error));
    //         return EXIT_FAILURE;
    //     }
    // }
    // for (char stack_elem = 'a'; stack_elem <= 'z'; ++stack_elem)
    // {
    //     fprintf(stderr, "%c. ", stack_elem);
    //     fprintf(stderr, "stack_data[]: %c ",
    //            *((char*)stack1.data + ('z' - (size_t)stack_elem) * stack1.elem_size));
    //     char stack_back_elem = 0;
    //     const enum StackError stack_back_error = stack_back(stack1, &stack_back_elem);
    //     if (stack_back_error != STACK_ERROR_SUCCESS)
    //     {
    //         if (logger_dtor())
    //                 fprintf(stderr, "Can't destroy logger\n");
    //         stack_dtor(&stack1);
    //         fprintf(stderr, "Can't stack1 push. STACK_ERROR: %s", stack_strerror(stack_back_error));
    //         return EXIT_FAILURE;
    //     }
    //     fprintf(stderr, "stack_back: %c ", stack_back_elem);
    //     char stack_pop_elem = 0;
    //     const enum StackError stack_pop_error = stack_pop(&stack1, &stack_pop_elem);
    //     if (stack_pop_error != STACK_ERROR_SUCCESS)
    //     {
    //         if (logger_dtor())
    //             fprintf(stderr, "Can't destroy logger\n");
    //         stack_dtor(&stack1);
    //         fprintf(stderr, "Can't stack1 push. STACK_ERROR: %s", stack_strerror(stack_pop_error));
    //         return EXIT_FAILURE;
    //     }
    //     fprintf(stderr, "stack_pop: %c\n", stack_pop_elem);
    // }
    // stack_dtor(&stack1);

#ifdef TEST_MODE

    const size_t COUNT_TEST_ACTIONS = 10;
    const size_t TEST_ELEM_SIZE = 7;
    error_handler = stack_test(COUNT_TEST_ACTIONS, TEST_ELEM_SIZE);
    if (error_handler)
    {
        fprintf(stderr, "Can't stack_test. Error: %s\n", strerror(error_handler));
        return EXIT_FAILURE;
    }

#endif /*TEST_MODE*/

    //----------------------------------------------------------

    if (logger_dtor())
    {
        fprintf(stderr, "Can't destroy logger\n");
        return EXIT_FAILURE;
    }
    return 0;
}