#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "pit.h"

static int note_find_current(int id, PNote *ppn)
{
    if (id) {
        *ppn = (PNote)pit_table_find(notes, id);
        if (!*ppn) die("could not find note %d", id);
    } else {
        *ppn = (PNote)pit_table_current(notes);
        if (!*ppn) die("could not find current note");
    }
    return *ppn ? (*(PNote *)ppn)->id : 0;
}

static void note_log_create(PTask pt, PNote pn, POptions po)
{
    Action a = { pt->project_id, pt->id, pn->id, { 0 } };
    
    sprintf(a.message, "created note %d: %s (task %d)", pn->id, po->note.message, pn->task_id);
    pit_action(&a);
}

static void note_log_update(PTask pt, PNote pn, POptions po)
{
    Action a = { pt->project_id, pt->id, pn->id, { 0 } };

    sprintf(a.message, "updated note %d: (message: %s, task %d)", pn->id, pn->message, pn->task_id);
    pit_action(&a);
}

static void note_log_delete(int project_id, int task_id, int id, char *message)
{
    Action a = { project_id, task_id, id, { 0 } };

    sprintf(a.message, "deleted note %d: %s (task %d)", id, message, task_id);
    pit_action(&a);
}

static void note_create(POptions po)
{
    pit_db_load();
    PTask pt = (PTask)pit_table_current(tasks);

    if (!pt) {
        die("no task selected");
    } else {
        Note n = { 0 }, *pn;

        n.task_id = pt->id;
        strncpy(n.message,  po->note.message, sizeof(n.message)  - 1);
        strncpy(n.username, current_user(),   sizeof(n.username) - 1);

        pn = (PNote)pit_table_insert(notes, (char *)&n);
        pit_table_mark(notes, pn->id);
        pt->number_of_notes++;
        note_log_create(pt, pn, po);
        pit_db_save();
    }
}

static void note_update(int id, POptions po)
{
    PNote pn;

    pit_db_load();
    id = note_find_current(id, &pn);

    strncpy(pn->message,  po->note.message, sizeof(pn->message)  - 1);
    strncpy(pn->username, current_user(),   sizeof(pn->username) - 1);
    pit_table_mark(notes, pn->id);

    note_log_update((PTask)pit_table_find(tasks, pn->task_id), pn, po);
    pit_db_save();
}

/*
** A note could be deleted as standalone entity or as part of cascading task
** or project delete.
*/
void pit_note_delete(int id, PTask pt)
{
    PNote pn;
    bool standalone = (pt == NULL);

    if (standalone) pit_db_load();
    id = note_find_current(id, &pn);
    if (standalone) pt = (PTask)pit_table_find(tasks, pn->task_id);

    if (pt) {
        char *deleted_message = str2str(pn->message);

        pn = (PNote)pit_table_delete(notes, id);
        if (pn) {
            pit_table_mark(notes, 0); /* TODO: find better current note candidate. */
            note_log_delete(pt->project_id, pt->id, id, deleted_message);
            if (standalone) {
                pt->number_of_notes--;
                pit_db_save();
            }
            free(deleted_message);
        } else {
            free(deleted_message);
            die("could not delete note %d", id);
        }
    } else {
        die("could not find task for note %d", id);
    }
}

void pit_note_list(PTask pt)
{
    if (!notes) pit_db_load();

    if (notes->number_of_records > 0) {
        PPager ppager = pit_pager_initialize(PAGER_NOTE, pt ? 4 : 0, notes->number_of_records);
        if (!pt) pt = (PTask)pit_table_current(tasks);
        for_each_note(pn) {
            if (pt && pn->task_id != pt->id)
                continue;
            pit_pager_print(ppager, (char *)pn);
        }
        pit_pager_flush(ppager);
    }
}

/*
** CREATING NOTES:
**   pit note -c message
**
** EDITING NOTES:
**   pit note -e [number] message
**
** DELETING NOTES:
**   pit note -d [number]
**
** LISTING NOTES:
**   pit note
*/
void pit_note(char *argv[])
{
    char **arg = &argv[1];
    int number = 0;
    Options opt = {{ 0 }};

    if (!*arg) {
        pit_note_list(NULL);
    } else {
        switch(pit_arg_option(arg)) {
        case 'c': /* pit note -c message */
            opt.note.message = pit_arg_string(++arg, "note message");
            note_create(&opt);
            break;
        case 'e': /* pit note -e [number] message */
            number = pit_arg_number(++arg, NULL);
            if (!number) --arg;
            opt.note.message = pit_arg_string(++arg, "note message");
            note_update(number, &opt);
            break;
        case 'd': /* pit note -d [number] */
            number = pit_arg_number(++arg, NULL);
            pit_note_delete(number, NULL);
            break;
        default:
            die("invalid note option: %s", *arg);
        }
    }
}
