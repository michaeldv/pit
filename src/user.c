#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "pit.h"
#include "user.h"

char *pit_current_user() {
    return getlogin();
}