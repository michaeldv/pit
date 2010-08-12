#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
#include "pit.h"

char *str2str(char *str) {
    return strcpy(malloc(strlen(str) + 1), str); /* Cheap strdup() */
}

char *mem2str(char *mem, int len) {
    char *str = malloc(len + 1);
    memcpy(str, mem, len);
    str[len] = '\0';

    return str;
}

bool is_zero(char *mem, int len) {
    char *pch = mem;

    while(pch - mem < len) {
        if (*pch++) return FALSE;
    }
    return TRUE;
}

void printa(char *msg[]) {
    while(*msg) puts(*msg++);
}

char *current_user() {
    static char *username = NULL;

    if (!username) {
        struct passwd *pws = getpwuid(geteuid());
        if (!pws) {
            perish("no username?!");
        } else {
            username = pws->pw_name;
        }
    }
    return username;
}

char *home_dir(char *username, int len) {
    char *str = mem2str(username, len);
    struct passwd *pw = getpwnam(str);
    free(str);

    return (pw ? pw->pw_dir : NULL);
}

char *expand_path(char *path, char *expanded) {
    if (!path || *path != '~') {
        return path;
    } else {
        char *next = path + 1;
        if (*next == '/') { /* Path without the username, i.e. ~/file */
            strcpy(expanded, getenv("HOME"));
            strcat(expanded, next);
        } else {            /* Path with the username, i.e. ~username/file */
            char *slash = strchr(next, '/');
            if (!slash) {
                slash = next + strlen(next);
            }
            char *home = home_dir(next, slash - next);
            if (!home) {    /* Ex. non-existent username. */
                perish(path);
            } else {
                strcpy(expanded, home);
                strcat(expanded, slash);
            }
        }
    }

    return expanded;
}

char *format_date(time_t date)
{
    static char str[32];
    struct tm *ptm;

    ptm = localtime(&date);
    if (!ptm->tm_hour && !ptm->tm_min && !ptm->tm_sec) {
        strftime(str, sizeof(str), "%b %d, %Y", ptm);
    } else {
        strftime(str, sizeof(str), "%b %d, %Y %H:%M", ptm);
    }
    return str;
}

char *format_time(time_t time)
{
    static char str[10];
    int hh = time / 3600;
    int mm = (time - hh * 3600) / 60;

    sprintf(str, "%d:%02d", hh, mm);
    return str;
}

char *format_timestamp(time_t timestamp)
{
    static char str[32];
    struct tm *ptm = localtime(&timestamp);

    strftime(str, sizeof(str), "%b %d, %Y at %H:%M", ptm);

    return str;
}

char *inline_replace(char *this, char *old, char *new)
{
    char *start = this;
    char *next = this;
    int new_length = strlen(new);
    int old_length = strlen(old);

    if (this && old && new && (old_length > 0) && (old_length >= new_length)) {
        while (*this) {
            if (strncmp(this, old, old_length)) {
                *next++ = *this++;                  /* Not a start of old thising, copy character unchanged. */
            } else {
                strncpy(next, new, new_length);     /* Found the old string, replace it with the new one. */
                next += new_length;                 /* Adjust pointers to move beyond replaced string. */
                this += old_length;
            }
        }
        *next = '\0';
    }
    return start;
}

#ifdef TEST
int main(int argc, char *argv[]) {
    printf("your username: %s\n", current_user());
    printf("your (cached) username: %s\n", current_user());
    return 1;
}
#endif
