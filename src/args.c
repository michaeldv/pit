#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
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

int pit_arg_number(char **arg, char *required)
{
    int number = 0;

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

static char *adjust_alpha_date(char **arg, char *format)
{
    char *pch = strstr(*arg, ", ");

    if (pch) {
        pch += 2;
        if (isdigit(*pch) && isdigit(*(pch + 1)) && !isdigit(*(pch + 2))) {
            inline_replace(format, "%Y", "%y");     /* Two digit year as in Oct 10, 92 19:30 */
        }
    } else {
        inline_replace(format, ", %Y", "");         /* No year as in Oct 10 19:30 */
    }

    return format;
}

static char *adjust_slash_date(char **arg, char *format)
{
    char *first = strchr(*arg, '/');
    char *last = strrchr(*arg, '/');
    if (first == last++) {
        inline_replace(format, "/%Y", "");          /* Single slash, i.e. no year as in 1/10 19:30 */
    } else {
        if (isdigit(*last) && isdigit(*(last + 1)) && !isdigit(*(last + 2))) {
            inline_replace(format, "%Y", "%y");     /* Two digit year as in 1/10/92 19:30 */
        }
    }

    return format;
}

static char *adjust_time(char **arg, char *format)
{
    if (strstr(*arg, "am") || strstr(*arg, "pm")) {
        inline_replace(format, "%H", "%I");
        strcat(format, "%p");
    }
    return format;
}

time_t pit_arg_date(char **arg, char *required)
{
    char format[32];
    time_t now = time(NULL);
    time_t seconds = (time_t)0;
    struct tm *ptm = localtime(&now);
    struct tm tm = { 0 };

    if (required && (!*arg || pit_arg_is_option(arg))) {
        die("missing %s", required);
    } else if (!strcmp(*arg, "none")) {                                 /* Drop dat value */
        return -1;
    } else {
        bool alpha_date = isalpha(**arg);
        bool slash_date = (strchr(*arg, '/') != NULL);

        if (alpha_date && islower(**arg)) {
            **arg = toupper(**arg);
        }

        if (alpha_date || slash_date) {                                 /* Date is present */
            if (strchr(*arg, ':')) {
                if (alpha_date) {
                    strcpy(format, "%b %d, %Y %H:%M");                  /* Oct 10, 1992 19:30 */
                } else {
                    strcpy(format, "%m/%d/%Y %H:%M");                   /* 10/10/1992 19:30 */
                }
            } else {
                if (strlen(*arg) >= 12) {
                    if (alpha_date) {
                        strcpy(format, "%b %d, %Y %H");                 /* Oct 10, 1992 19 */
                    } else {
                        strcpy(format, "%m/%d/%Y %H");                  /* 10/10/1992 19 */
                    }
                } else {                                                /* Date with no time */
                    if (alpha_date) {
                        strcpy(format, "%b %d, %Y");                    /* Oct 10, 1992 */
                    } else {
                        strcpy(format, "%m/%d/%Y");                     /* 10/10/1992 */
                    }
                }
            }
            if (alpha_date) {                                           /* Replace %Y with %y for two-digit year... */
                adjust_alpha_date(arg, format);                         /* or remove %Y if no year was specified */
            } else {
                adjust_slash_date(arg, format);
            }
        } else {                                                        /* No date, time only */
            if (strchr(*arg, ':')) {
                strcpy(format, "%H:%M");                                /* 19:30 */
            } else {
                strcpy(format, "%H");                                   /* 19 */
            }
        }
        adjust_time(arg, format);                                       /* Replace %H with %I%p for am/pm time */

        /* Ready to roll :-) */
        // printf("format: %s\n", format);
        if (strptime(*arg, format, &tm)) {
            // printf("then: %s\n", asctime(&tm));
            if (!tm.tm_mday) tm.tm_mday  = ptm->tm_mday;
            if (!tm.tm_mon)  tm.tm_mon   = ptm->tm_mon;
            if (!tm.tm_year) tm.tm_year  = ptm->tm_year;
            tm.tm_isdst = -1;
            // printf(" now: %s\n", asctime(ptm));
            // printf(" adj: %s\n", asctime(&tm));
            seconds = mktime(&tm);
            // printf("ctime: %s", ctime(&seconds));
            if (seconds == (time_t)-1) {
                perish("invalid date");
            }
        } else {
            die("invalid %s: %s", required, *arg);
        }
    }

    return seconds;
}

time_t pit_arg_time(char **arg, char *required)
{
    int seconds = 0;

    if (required && (!*arg || pit_arg_is_option(arg))) {
        die("missing %s", required);
    } else if (!strcmp(*arg, "none")) {     /* Drop time value */
        return -1;
    } else {                                /* Suppored time formats are :MM, HH:MM, and HH */
        char *colon = strchr(*arg, ':');
        if (colon == *arg) {                /* :MM - minutes only */
            seconds = atoi(++colon) * 60;
        } else if (colon) {                 /* HH:MM - hours and minutes */
            *colon = '\0';
            seconds = atoi(*arg) * 3600 + atoi(++colon) * 60;
        } else {                            /* HH - hours only */
            seconds = atoi(*arg) * 3600;
        }
    }

    if (!seconds) {
        die("invalid time format: %s", *arg);
    }

    return seconds;
}
