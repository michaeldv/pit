/*
** Copyright (c) 2010 Michael Dvorkin
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the Simplified BSD License (also
** known as the "2-Clause License" or "FreeBSD License".)
**
** This program is distributed in the hope that it will be useful,
** but without any warranty; without even the implied warranty of
** merchantability or fitness for a particular purpose.
*/
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

static bool read_and_validate_header(FILE *file)
{
    header = (PHeader)calloc(1, sizeof(Header));

    if (fread(header, sizeof(Header), 1, file)) {
        if (header->signature[0] != 0x50 || header->signature[1] != 0x49 || header->signature[2] != 0x54) {
            printf("pit: invalid pit file (%s)\n", pit_file_name());
            return FALSE;
        }
        if (header->schema_version != PIT_SCHEMA_VERSION) {
            printf("pit: invalid pit file version (%d)\n", header->schema_version);
            return FALSE;
        }
    } else {
        puts("pit: error reading pit file header");
        return FALSE;
    }

    return TRUE;
}

static void write_header(FILE *file)
{
    if (!header) header = (PHeader)calloc(1, sizeof(Header));

    header->signature[0] = 0x50; header->signature[1] = 0x49; header->signature[2] = 0x54;
    header->schema_version = PIT_SCHEMA_VERSION;
    if (!header->created_at) {
        header->created_at = time(NULL);
        strncpy(header->created_by, current_user(), sizeof(header->created_by) - 1);
    }
    header->updated_at = time(NULL);
    strncpy(header->updated_by, current_user(), sizeof(header->updated_by) - 1);

    if (!fwrite(header, sizeof(Header), 1, file)) {
        die("couldn't write header");
    }
}

void pit_init(char *argv[]) {
    char **arg = &argv[1];
    char *file_name = pit_file_name();
    int reply = 'Y';

    if (!access(file_name, F_OK)) {
        if (!*arg || strcmp(*arg, "-f")) { /* Do not prompt user if forced init (-f). */
            printf("%s already exists, do you want to override it [y/N]: ", file_name);
            reply = getchar();
        }
    }

    if (reply == 'y' || reply == 'Y') {
        pit_db_initialize();
        printf("Created empty %s\n", file_name);
    }
}

void pit_info(char *argv[])
{
    pit_db_load();
    printf("Pit version:     %s\n", PIT_VERSION);
    printf("Pit file name:   %s\n", pit_file_name());
    printf("Created by:      %s on %s\n", header->created_by, format_timestamp(header->created_at)); 
    printf("Last updated by: %s on %s\n", header->updated_by, format_timestamp(header->updated_at)); 
    printf("Schema version:  %d\n", header->schema_version);
    printf("Projects:        %d\n", projects->number_of_records); 
    printf("Tasks:           %d\n", tasks->number_of_records); 
    printf("Notes:           %d\n", notes->number_of_records); 
    printf("Log entries:     %d\n", actions->number_of_records); 
}

void pit_db_load() {
    char *file_name = pit_file_name();
    FILE *file = fopen(file_name, "r");

    if (!file) {
        perish(file_name);
    } else {
        if (read_and_validate_header(file)) {
            projects = pit_table_load(file);
            tasks    = pit_table_load(file);
            notes    = pit_table_load(file);
            actions  = pit_table_load(file);
            fclose(file);
        } else {
            fclose(file);
            die(NULL);
        }
    }
}

void pit_db_initialize() {
    Action a = { 0 };

    projects = pit_table_initialize(sizeof(Project), TABLE_HAS_ID | TABLE_HAS_TIMESTAMPS);
    tasks    = pit_table_initialize(sizeof(Task),    TABLE_HAS_ID | TABLE_HAS_TIMESTAMPS);
    notes    = pit_table_initialize(sizeof(Note),    TABLE_HAS_ID | TABLE_HAS_TIMESTAMPS);
    actions  = pit_table_initialize(sizeof(Action),  TABLE_HAS_CREATED_AT);

    strcpy(a.message, "Initialized pit");
    pit_action(&a);
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
