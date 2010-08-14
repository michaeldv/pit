#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "pit.h"

static void action_list()
{
    PPager ppager;

    pit_db_load();
    if (actions->number_of_records > 0) {
        ppager = pit_pager_initialize(PAGER_ACTION, 0, actions->number_of_records);
        for_each_action(pa) {
            pit_pager_print(ppager, (char *)pa);
        }
        pit_pager_flush(ppager);
    }
}

void pit_action(int id, char *subject, char *message)
{
    static Action action = { 0 };

    action.subject_id = id;
    strncpy(action.subject, subject, sizeof(action.subject) - 1);
    strncpy(action.username, current_user(), sizeof(action.username) - 1);
    strncpy(action.message, message, sizeof(action.message) - 1);

    pit_table_insert(actions, (char *)&action);
    if (strcmp(subject, "pit")) puts(message);
}

void pit_log(char *argv[])
{
    action_list();
}