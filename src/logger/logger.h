#ifndef SRC_LOGGER_H
#define SRC_LOGGER_H

#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>


enum LogCode
{
    LOG_CODE_SUCCES  = 0,
    LOG_CODE_FAILURE = 1
};
static_assert(LOG_CODE_SUCCES == 0);

enum LogLevelDetails
{
    LOG_LEVEL_DETAILS_ZERO    = 0b0000u,
    LOG_LEVEL_DETAILS_INFO    = 0b0001u,
    LOG_LEVEL_DETAILS_ERROR   = 0b0010u,
    LOG_LEVEL_DETAILS_LASSERT = 0b0100u,
    LOG_LEVEL_DETAILS_DUMB    = 0b1000u,
    LOG_LEVEL_DETAILS_ALL     = 0b1111u,
};


enum LogCode logger_ctor(void);
enum LogCode logger_dtor(void);

enum LogCode logger_set_level_details(const unsigned level_details);
enum LogCode logger_set_logout_file(const char* const filename);


enum LogCode internal_func_log(const char* const func_name, const int line_num, 
                               const char* const filename, enum LogLevelDetails level_details,
                               const bool check, const char* const check_str,
                               const char* const format, ...);

#define logg(log_level_details, format, ...)                                                        \
        do                                                                                          \
        {                                                                                           \
            if (internal_func_log(__func__, __LINE__, __FILE__, log_level_details, true, NULL,      \
                                  format, ##__VA_ARGS__) != LOG_CODE_SUCCES)                        \
                fprintf(stderr, "Can't log smth\n");                                                \
        } while (0)


#ifndef NDEBUG
// REVIEW - maybe add local assert macro or smth
#define lassert(check, ...)                                                                         \
        do                                                                                          \
        {                                                                                           \
            if(!(check))                                                                            \
            {                                                                                       \
                if (internal_func_log(__func__, __LINE__, __FILE__,                                 \
                                      LOG_LEVEL_DETAILS_LASSERT | LOG_LEVEL_DETAILS_ERROR,          \
                                      false, #check, ##__VA_ARGS__) != LOG_CODE_SUCCES)             \
                    fprintf(stderr, "Can't log smth\n");                                            \
                if (logger_dtor())                                                                  \
                    fprintf(stderr, "Can't destroy logger\n");                                      \
                assert(0);                                                                          \
            }                                                                                       \
        } while(0)

#define nlassert(check, ...)                                                                        \
        do                                                                                          \
        {                                                                                           \
            if(!(check))                                                                            \
            {                                                                                       \
                if (internal_func_log(__func__, __LINE__, __FILE__, LOG_LEVEL_DETAILS_LASSERT,      \
                                      false, #check, ##__VA_ARGS__) != LOG_CODE_SUCCES)             \
                    fprintf(stderr, "Can't log smth\n");                                            \
                if (logger_dtor())                                                                  \
                    fprintf(stderr, "Can't destroy logger\n");                                      \
                assert(0);                                                                          \
            }                                                                                       \
        } while(0) 

#else /*NDEBUG*/

#define  lassert(check, format, ...) do {} while(0)
#define nlassert(check, format, ...) do {} while(0)

#endif /*NDEBUG*/


#endif /*SRC_LOGGER_H*/