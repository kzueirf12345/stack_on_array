#include <stdio.h>

// #define HASH_PROTECT
#include "logger/logger.h"
#include "stack/stack_funcs.h"
#include "stack/verification/verification.h"
// #undef PENGUIN_PROTECT // TODO - это нихуя не работает, надо переносить всё в ашник

// TODO - README
int main()
{
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

    stack_t stack = { STACK_INIT(stack) };
    // fprintf(stderr, "RIGHT: %lX", stack.PENGUIN_RIGHT_);
    stack_ctor(&stack, sizeof(int), 0);

    for (int stack_elem = 0; stack_elem < 10; ++stack_elem)
    {
        const enum StackError stack_push_error = stack_push(&stack, &stack_elem);
        if (stack_push_error != STACK_ERROR_SUCCESS)
        {
            if (logger_dtor())
                fprintf(stderr, "Can't destroy logger\n");
            stack_dtor(&stack);
            fprintf(stderr, "Can't stack push. STACK_ERROR: %s", stack_strerror(stack_push_error));
            return EXIT_FAILURE;
        }
    }

    for (int stack_elem = 0; stack_elem < 10; ++stack_elem)
    {
        fprintf(stderr, "%-3d. ", stack_elem);
        fprintf(stderr, "stack_data[]: %-3d ",
               *(int*)((char*)stack.data + (9 - (size_t)stack_elem) * stack.elem_size));

        int stack_back_elem = 0;
        const enum StackError stack_back_error = stack_back(stack, &stack_back_elem);
        if (stack_back_error != STACK_ERROR_SUCCESS)
        {
            if (logger_dtor())
                    fprintf(stderr, "Can't destroy logger\n");
            stack_dtor(&stack);
            fprintf(stderr, "Can't stack push. STACK_ERROR: %s", stack_strerror(stack_back_error));
            return EXIT_FAILURE;
        }
        fprintf(stderr, "stack_back: %-3d ", stack_back_elem);

        int stack_pop_elem = 0;
        const enum StackError stack_pop_error = stack_pop(&stack, &stack_pop_elem);
        if (stack_pop_error != STACK_ERROR_SUCCESS)
        {
            if (logger_dtor())
                fprintf(stderr, "Can't destroy logger\n");
            stack_dtor(&stack);
            fprintf(stderr, "Can't stack push. STACK_ERROR: %s", stack_strerror(stack_pop_error));
            return EXIT_FAILURE;
        }
        fprintf(stderr, "stack_pop: %-3d\n", stack_pop_elem);
    }

    stack_dtor(&stack);

    //----------------------------------------------------------

    if (logger_dtor())
    {
        fprintf(stderr, "Can't destroy logger\n");
        return EXIT_FAILURE;
    }
    return 0;
}