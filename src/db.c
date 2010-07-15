#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "pit.h"
#include "db.h"
#include "activity.h"
#include "user.h"

#define PITFILE "/tmp/.pit"

void pit_db_load() {
    FILE *pitfile = fopen(PITFILE, "r");

    if (pitfile) {
        projects   = pit_table_load(pitfile);
        tasks      = pit_table_load(pitfile);
        notes      = pit_table_load(pitfile);
        activities = pit_table_load(pitfile);
        users      = pit_table_load(pitfile);
        fclose(pitfile);
    } else {
        pit_db_initialize();
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
    FILE *pitfile = fopen(PITFILE, "wb");

    if (!pitfile) {
        die_with_errno(PITFILE);
    } else {
        pit_table_save(pitfile, projects);
        pit_table_save(pitfile, tasks);
        pit_table_save(pitfile, notes);
        pit_table_save(pitfile, activities);
        pit_table_save(pitfile, users);

        fclose(pitfile);
    }
}
