#include <stdio.h>

#include "logger/logger.h"
#include "stack/stack_funcs.h"
#include "stack/verification/verification.h"


//TODO check ded's texlib for crossplatform log (__file__ and other)

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
    stack_ctor(&stack, 10);

    const enum StackError stack_push_error = stack_push(&stack, 123);
    if (stack_push_error != STACK_ERROR_SUCCESS) // REVIEW I don't like this handle-method
    {
        if (fprintf(stderr, "Can't stack push. STACK_ERROR: %s", stack_strerror(stack_push_error))
            <= 0)
        {
            fprintf(stderr, "Can't fprintf stack_error\n");
        }
        return -1;
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