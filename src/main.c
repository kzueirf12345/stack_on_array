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

#ifdef TEST_MODE

    const size_t COUNT_TEST_ACTIONS = 10;
    const size_t TEST_ELEM_SIZE = 8;
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