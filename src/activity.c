#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "pit.h"
#include "activity.h"

ulong  subject_id;              /* Reference to the specific Project, Task, or Note. */
char   subject[16];             /* Project, Task, or Note. */
char   message[255];            /* Log message. */
ulong  created_by;              /* Who added log message? */
time_t created_at;              /* When log message was added? */    

PActivity pit_add_activity(ulong id, char *subject, char *message, ulong user_id) {
    static Activity activity;

    memset(&activity, 0, sizeof(activity));

    activity.subject_id = id;
    activity.created_by = user_id;
    activity.created_at = time(NULL);
    strncpy(activity.subject, subject, sizeof(activity.subject) - 1);
    strncpy(activity.message, message, sizeof(activity.message) - 1);

    return &activity;
}
