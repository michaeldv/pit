#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "pit.h"

int pit_arg_is_option(char **arg)
{
    return *arg && (**arg == '-' || **arg == '/') && strlen(*arg) == 2;
}

int pit_arg_option(char **arg)
{
    if (pit_arg_is_option(arg)) {
        return *(*arg + 1);
    } else {
        die("invalid option");
        return 0;
    }
}

char *pit_arg_string(char **arg, char *required)
{
    if (required && (!*arg || pit_arg_is_option(arg))) {
        die("missing %s", required);
    }

    return *arg;
}

ulong pit_arg_number(char **arg, char *required)
{
    ulong number = 0L;

    if (required && (!*arg || pit_arg_is_option(arg))) {
        die("missing %s", required);
    }
    if (*arg) {
        number = atol(*arg);
        if (number == 0 && **arg != '0' && required) {
            die("invalid %s value: %s", required, *arg);
        }
    }

    return number;
}

// char *strptime(const char *restrict buf, const char *restrict format, struct tm *restrict tm);
// time_t mktime ( struct tm * timeptr );
time_t pit_arg_time(char **arg, char *required)
{
    time_t seconds = (time_t)-1;
    struct tm tm;


    if (required && (!*arg || pit_arg_is_option(arg))) {
        die("missing %s", required);
    } else {
        memset(&tm, 0, sizeof(tm));
        if (strptime(*arg, "%m/%d/%Y", &tm)) {
            seconds = mktime(&tm);
        }

        if (strchr(*arg, ',')) {                                        /* Date present. */
            if (strchr(*arg, ':')) {                                    /* Date and HH:MM. */
                if (!strstr(*arg, "am") || !strstr(*arg, "pm")) {       /* Date and HH:MM am/pm. */
                    /* Oct 10, 1992 7:30pm */
                } else {
                    /* Oct 10, 1992 19:30 */
                }
            } else {
                if (strlen(*arg) > 12) {                                /* Date and HH. */
                    if (!strstr(*arg, "am") || !strstr(*arg, "pm")) {   /* Date and HH am/pm. */
                        /* Oct 10, 1992 7pm */
                    } else {
                        /* Oct 10, 1992 19 */                           /* HH */
                    }
                } else {                                                /* Date with no time. */
                    /* Oct 10, 1992 19 */
                }
            }
        } else {                                                        /* No date, time only. */
            if (strchr(*arg, ':')) {                                    /* HH:MM. */
                if (!strstr(*arg, "am") || !strstr(*arg, "pm")) {       /* HH:MM am/pm. */
                    /* 7:30pm */
                } else {
                    /* 19:30 */
                }
            } else {
                if (!strstr(*arg, "am") || !strstr(*arg, "pm")) {       /* HH am/pm. */
                    /* 7pm */
                } else {
                    /* 19 */                                            /* HH */
                }
            }
        }

        if (seconds == (time_t)-1) {
            die("invalid %s: %s", required, *arg);
        }
    }

    return seconds;
}