#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "pit.h"

char *pit_current_user() {
    return getlogin();
}