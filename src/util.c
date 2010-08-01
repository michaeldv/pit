#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
#include "pit.h"

char *mem2str(char *mem, int len) {
    char *str = malloc(len + 1);
    memcpy(str, mem, len);
    str[len] = '\0';

    return str;
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

#ifdef TEST
int main(int argc, char *argv[]) {
    printf("your username: %s\n", current_user());
    printf("your (cached) username: %s\n", current_user());
    return 1;
}
#endif
