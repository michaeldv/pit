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
        char *penv = getenv("PITFILE");
        strcpy(file_name, expand_path(penv ? penv : PITFILE, file_name));
    }

    return file_name;
}

static void read_header(FILE *file)
{
    Header hd;

    if (fread(&hd, sizeof(hd), 1, file)) {
        if (hd.signature[0] != 0x50 || hd.signature[1] != 0x49 || hd.signature[2] != 0x54) {
            die("invalid pit file");
        }
        if (hd.schema_version != 0x01) {
            die("invalid pit file version");
        }
    } else {
        die("couldn't read header");
    }
}

static void write_header(FILE *file)
{
    Header hd;

    hd.signature[0] = 0x50; hd.signature[1] = 0x49; hd.signature[2] = 0x54;
    hd.schema_version = 0x01;
    memset(&hd.reserved, 0, sizeof(hd.reserved));

    if (!fwrite(&hd, sizeof(hd), 1, file)) {
        die("couldn't write header");
    }
}

void pit_init(char *argv[]) {
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
}

void pit_db_load() {
    char *file_name = pit_file_name();
    FILE *file = fopen(file_name, "r");

    if (!file) {
        perish(file_name);
    } else {
        read_header(file);
        projects = pit_table_load(file);
        tasks    = pit_table_load(file);
        notes    = pit_table_load(file);
        actions  = pit_table_load(file);
        fclose(file);
    }
}

void pit_db_initialize() {
    projects = pit_table_initialize(sizeof(Project),  TABLE_HAS_ID | TABLE_HAS_TIMESTAMPS);
    tasks    = pit_table_initialize(sizeof(Task),     TABLE_HAS_ID | TABLE_HAS_TIMESTAMPS);
    notes    = pit_table_initialize(sizeof(Note),     TABLE_HAS_ID | TABLE_HAS_TIMESTAMPS);
    actions  = pit_table_initialize(sizeof(Action), TABLE_HAS_CREATED_AT);

    pit_action(0, "pit", "Initialized pit");

    pit_db_save();
}

void pit_db_save() {
    char *file_name = pit_file_name();
    FILE *file = fopen(file_name, "wb");

    if (!file) {
        perish(file_name);
    } else {
        write_header(file);
        pit_table_save(file, projects);
        pit_table_save(file, tasks);
        pit_table_save(file, notes);
        pit_table_save(file, actions);
        fclose(file);
    }
}
