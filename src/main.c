#include <stdio.h>

#include "logger/logger.h"
#include "stack/stack_funcs.h"
#include "stack/verification/verification.h"


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
    stack_ctor(&stack, 0);

    for (stack_elem_t stack_elem = 0; stack_elem < 100; ++stack_elem)
    {
        const enum StackError stack_push_error = stack_push(&stack, stack_elem);
        if (stack_push_error != STACK_ERROR_SUCCESS)
        {
            if (logger_dtor())
                fprintf(stderr, "Can't destroy logger\n");
            stack_dtor(&stack);
            fprintf(stderr, "Can't stack push. STACK_ERROR: %s", stack_strerror(stack_push_error));
            return EXIT_FAILURE;
        }
    }

    // stack.data = NULL;

    for (stack_elem_t stack_elem = 0; stack_elem < 100; ++stack_elem)
    {
        printf("%-3d. ", stack_elem);
        printf("stack_back: %-3d ", stack_back(stack));

        stack_elem_t stack_pop_elem = 0;
        const enum StackError stack_pop_error = stack_pop(&stack, &stack_pop_elem);
        if (stack_pop_error != STACK_ERROR_SUCCESS)
        {
            if (logger_dtor())
                fprintf(stderr, "Can't destroy logger\n");
            stack_dtor(&stack);
            fprintf(stderr, "Can't stack push. STACK_ERROR: %s", stack_strerror(stack_pop_error));
            return EXIT_FAILURE;
        }
        printf("stack_pop: %-3d\n", stack_pop_elem);
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