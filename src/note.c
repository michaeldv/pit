#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "pit.h"

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

static int note_find_current(int id, PProject *ppp)
{
    // if (id) {
    //     *ppp = (PProject)pit_table_find(notes, id);
    //     if (!*ppp) die("could not find note %d", id);
    // } else {
    //     *ppp = (PProject)pit_table_current(notes);
    //     if (!*ppp) die("could not find current note");
    // }
    // return *ppp ? (*(PProject *)ppp)->id : 0;
    return 0;
}

static void note_log_create(PNote pn, POptions po)
{
    char str[256];
    
    sprintf(str, "created note %d: %s (task %d)", pn->id, po->note.message, pn->task_id);
    pit_action(pn->id, "note", str);
}

static void note_log_update(PNote pn, POptions po)
{
    // char str[256];
    // bool empty = TRUE;
    // 
    // sprintf(str, "updated note %d:", pn->id);
    // if (po->note.name) {
    //     sprintf(str + strlen(str), " (name: %s", po->note.name);
    //     empty = FALSE;
    // } else {
    //     sprintf(str + strlen(str), " %s (", pn->name);
    // }
    // if (po->note.status) {
    //     sprintf(str + strlen(str), "%sstatus: %s)", (empty ? "" : ", "), po->note.status);
    // }
    // strcat(str, ")");
    // pit_action(pn->id, "note", str);
}

static void note_log_delete(int id, char *name, int number_of_tasks)
{
    // char str[256];
    // 
    // sprintf(str, "deleted note %d: %s", id, name);
    // if (number_of_tasks > 0) {
    //     sprintf(str + strlen(str), " with %d task%s", number_of_tasks, (number_of_tasks == 1 ? "" : "s"));
    // }
    // pit_action(id, "note", str);
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
        note_log_create(pn, po);
        pit_db_save();
    }
}

static void note_update(int id, POptions po)
{
    puts("note_update");
    // PProject pp;
    // 
    // pit_db_load();
    // id = note_find_current(id, &pp);
    // 
    // if (po->note.name)   strncpy(pp->name,   po->note.name,   sizeof(pp->name)   - 1);
    // if (po->note.status) strncpy(pp->status, po->note.status, sizeof(pp->status) - 1);
    // pit_table_mark(notes, pp->id);
    // 
    // note_log_update(pp, po);
    // pit_db_save();
}

void pit_note_delete(int id, PTask pt)
{
    puts("note_delete");
    // PProject pp;
    // 
    // pit_db_load();
    // id = note_find_current(id, &pp);
    // /*
    // ** Delete note tasks.
    // */
    // if (pp->number_of_tasks > 0) {
    //     for_each_task(pt) {
    //         if (pt->note_id == id) {
    //             pit_task_delete(pt->id, pp);
    //             --pt; /* Make the task pointer stay since it now points to the next task. */
    //         }
    //     }
    // }
    // /*
    // ** Ready to delete the note itself. But first preserve the
    // ** name and number of tasks since we need these bits for logging.
    // */
    // char *deleted_name = str2str(pp->name);
    // int deleted_number_of_tasks = pp->number_of_tasks;
    // 
    // pp = (PProject)pit_table_delete(notes, id);
    // if (pp) {
    //     pit_table_mark(notes, 0); /* TODO: find better current note candidate. */
    //     note_log_delete(id, deleted_name, deleted_number_of_tasks);
    //     pit_db_save();
    // } else {
    //     die("could not delete the note");
    // }
}

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
            if (is_zero((char *)&opt.note, sizeof(opt.note))) {
                die("nothing to update");
            } else {
                note_update(number, &opt);
            }
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
