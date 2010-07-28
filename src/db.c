#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "pit.h"

#define PITFILE "~/.pit"


static char *pit_file_name()
{
    static char file_name[128];

    if (!*file_name) {
        strcpy(file_name, expand_path(PITFILE, file_name));
    }

    return file_name;
}

int pit_init(char *argv[]) {
    char **arg = &argv[1];
    char *file_name = pit_file_name();
    int reply = 'Y';

    if (!access(file_name, F_OK)) {
        if (!*arg || strcmp(*arg, "-f")) { /* Do not prompt user if forced init (-f). */
            printf("%s already exist, do you want to override it [y/N]: ", file_name);
            reply = getchar();
        }
    }

    if (reply == 'y' || reply == 'Y') {
        pit_db_initialize();
        printf("Created empty %s\n", file_name);
    }
    return 1;
}

void pit_db_load() {
    char *file_name = pit_file_name();
    FILE *file = fopen(file_name, "r");

    if (!file) {
        perish(file_name);
    } else {
        projects   = pit_table_load(file);
        tasks      = pit_table_load(file);
        notes      = pit_table_load(file);
        activities = pit_table_load(file);
        users      = pit_table_load(file);
        fclose(file);
    }
}

void pit_db_initialize() {
    projects   = pit_table_initialize(sizeof(Project),  TABLE_HAS_ID | TABLE_HAS_TIMESTAMPS);
    tasks      = pit_table_initialize(sizeof(Task),     TABLE_HAS_ID | TABLE_HAS_TIMESTAMPS);
    notes      = pit_table_initialize(sizeof(Note),     TABLE_HAS_ID | TABLE_HAS_TIMESTAMPS);
    activities = pit_table_initialize(sizeof(Activity), TABLE_HAS_CREATED_AT);
    users      = pit_table_initialize(sizeof(User),     TABLE_HAS_ID);

    pit_table_insert(users,      (uchar *)pit_current_user());
    pit_table_insert(activities, (uchar *)pit_add_activity(0, "", "Initialized pit", 1));

    pit_db_save();
}

void pit_db_save() {
    char *file_name = pit_file_name();
    FILE *file = fopen(file_name, "wb");

    if (!file) {
        perish(file_name);
    } else {
        pit_table_save(file, projects);
        pit_table_save(file, tasks);
        pit_table_save(file, notes);
        pit_table_save(file, activities);
        pit_table_save(file, users);
        fclose(file);
    }
}
