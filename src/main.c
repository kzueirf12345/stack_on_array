#include <stdio.h>

#include "stack/stack_funcs.h"

int main()
{
    if (logger_ctor())
    {
        fprintf(stderr, "Can't init logger\n");
        return EXIT_FAILURE;
    }
    if (logger_set_level_details(LOG_LEVEL_DETAILS_ZERO))
    {
        fprintf(stderr, "Can't set logger level details\n");
        return EXIT_FAILURE;
    }

    //----------------------------------------------------------

    

    //----------------------------------------------------------

    if (logger_dtor())
    {
        fprintf(stderr, "Can't destroy logger\n");
        return EXIT_FAILURE;
    }
    return 0;
}