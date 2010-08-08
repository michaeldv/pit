#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "pit.h"

static void project_list(char *name, char *status);
static void project_show(ulong id);
static void project_create(char *name, char *status);
static void project_update(ulong id, char *name, char *status);
static void project_delete(ulong id);
static bool project_already_exist(char *name);
static void project_find_current(PProject *ppp, ulong *pid);
static void project_log_create(PProject pp, char *name, char *status);
static void project_log_update(PProject pp, char *name, char *status);
static void project_log_delete(ulong id, char *name, ulong number_of_tasks);
static void project_parse_options(char **arg, char **name, char **status);

/*
** CREATING PROJECTS:
**   pit project -c name [-s status]
**
** EDITING PROJECTS:
**   pit project -e [number] [-n name] [-s status]
**
** DELETING PROJECTS:
**   pit project -d [number]
**
** VIEWING PROJECT:
**   pit project [[-q] number]
**
** LISTING PROJECTS:
**   pit project -q [number | [-n name] [-s status]]
*/

static void project_list(char *name, char *status)
{
    PPager ppager;

    pit_db_load();
    if (projects->number_of_records > 0) {
        ppager = pit_pager_initialize(PAGER_PROJECT, projects->number_of_records);
        for_each_project(pp) {
            pit_pager_print(ppager, (uchar *)pp);
        }
        pit_pager_flush(ppager);
    }
}

static void project_create(char *name, char *status)
{
    pit_db_load();

    if (project_already_exist(name)) {
        die("project with the same name already exists");
    } else {
        Project p, *pp;

        memset(&p, 0, sizeof(p));

        if (!status) status = "active";
        printf("creating project [%s], status [%s]\n", name, status);

        strncpy(p.name, name, sizeof(p.name) - 1);
        strncpy(p.status, status, sizeof(p.status) - 1);
        strncpy(p.username, current_user(), sizeof(p.username) - 1);

        pp = (PProject)pit_table_insert(projects, (uchar *)&p);
        pit_table_mark(projects, pp->id);
        pit_db_save();
    }
}

static void project_show(ulong id)
{
    PProject pp;

    pit_db_load();
    pp = (PProject)pit_table_find(projects, id);
    if (pp) {
        printf("%lu: %s (%s, %lu task%s)\n",
            pp->id, pp->name, pp->status, pp->number_of_tasks, (pp->number_of_tasks != 1 ? "s" : ""));
        if (pp->number_of_tasks > 0) {
            puts("Tasks:");
            for_each_task(pt) {
                printf("  %c %lu: %s (%lu notes)\n", (pt->id == tasks->current ? '*' : ' '), pt->id, pt->name, pt->number_of_notes);
            }
        }
        pit_table_mark(projects, pp->id);
        pit_db_save();
    } else {
        die("could not find the project");
    }
}

static void project_update(ulong id, char *name, char *status)
{
    PProject pp;

    pit_db_load();
    project_find_current(&pp, &id);

    if (name) strncpy(pp->name, name, sizeof(pp->name) - 1);
    if (status) strncpy(pp->status, status, sizeof(pp->status) - 1);
    pit_table_mark(projects, pp->id);

    project_log_update(pp, name, status);
    pit_db_save();
}

static void project_delete(ulong id)
{
    PProject pp;

    pit_db_load();
    project_find_current(&pp, &id);
    /*
    ** Delete project tasks.
    */
    if (pp->number_of_tasks > 0) {
        for_each_task(pt) {
            if (pt->project_id == id) {
                pit_task_delete(pt->id, pp);
                --pt; /* Make the task pointer stay since it now points to the next task. */
            }
        }
    }
    /*
    ** Ready to delete the project itself. But first preserve the
    ** name and number of tasks since we need these bits for logging.
    */
    char *deleted_name = str2str(pp->name);
    ulong deleted_number_of_tasks = pp->number_of_tasks;

    pp = (PProject)pit_table_delete(projects, id);
    if (pp) {
        pit_table_mark(projects, 0); /* TODO: find better current project candidate. */
        project_log_delete(id, deleted_name, deleted_number_of_tasks);
        pit_db_save();
    } else {
        die("could not delete the project");
    }
}

static bool project_already_exist(char *name)
{
    pit_db_load();
    for_each_project(pp) {
        if (!strcmp(pp->name, name)) {
            return TRUE;
        }
    }
    return FALSE;
}

static void project_find_current(PProject *ppp, ulong *pid)
{
    if (*pid) {
        *ppp = (PProject)pit_table_find(projects, *pid);
        if (!*ppp) die("could not find project %lu", *pid);
    } else {
        *ppp = (PProject)pit_table_current(projects);
        if (!*ppp) die("could not find current project");
        else *pid = (*(PProject *)ppp)->id;
    }
}

static void project_log_create(PProject pp, char *name, char *status)
{
    char str[256];

    sprintf(str, "created project %lu: %s (status: %s)", pp->id, name, status);
    puts(str);
    pit_action(pp->id, "project", str);
}

static void project_log_update(PProject pp, char *name, char *status)
{
    char str[256];
    bool empty = TRUE;

    sprintf(str, "updated project %lu:", pp->id);
    if (name) {
        sprintf(str + strlen(str), " (name: %s", name);
        empty = FALSE;
    } else {
        sprintf(str + strlen(str), " %s (", pp->name);
    }
    if (status) {
        sprintf(str + strlen(str), "%sstatus: %s)", (empty ? "" : ", "), status);
    }
    puts(str);
    pit_action(pp->id, "project", str);
}

static void project_log_delete(ulong id, char *name, ulong number_of_tasks)
{
    char str[256];
    
    sprintf(str, "deleted project %lu: %s", id, name);
    if (number_of_tasks > 0) {
        sprintf(str + strlen(str), " with %lu task%s", number_of_tasks, (number_of_tasks == 1 ? "" : "s"));
    }
    puts(str);
    pit_action(id, "project", str);
}

static void project_parse_options(char **arg, char **name, char **status)
{
    while(*++arg) {
        switch(pit_arg_option(arg)) {
        case 's':
            *status = pit_arg_string(++arg, "project status");
            break;
        case 'n':
            if (name) {
                *name = pit_arg_string(++arg, "project name");
                break;
            } /* else fall though */
        default:
            die("invalid project option: %s", *arg);
        }
    }
}

void pit_project(char *argv[])
{
    char **arg = &argv[1];
    char *name = NULL, *status = NULL;
    ulong number = 0L;

    if (!*arg) {
        project_list(name, status); /* Show all projects. */
    } else { /* pit project [number] */
        number = pit_arg_number(arg, NULL);
        if (number) {
            project_show(number);
        } else {
            switch(pit_arg_option(arg)) {
            case 'c': /* pit project -c name [-s status] */
                name = pit_arg_string(++arg, "project name");
                project_parse_options(arg, NULL, &status);
                project_create(name, status);
                break;
            case 'e': /* pit project -e [number] [-n name] [-s status] */
                number = pit_arg_number(++arg, NULL);
                if (!number) --arg;
                project_parse_options(arg, &name, &status);
                if (!name && !status) {
                    die("nothing to update");
                } else {
                    project_update(number, name, status);
                }
                break;
            case 'd': /* pit project -d [number] */
                number = pit_arg_number(++arg, NULL);
                project_delete(number);
                break;
            case 'q': /* pit project -q [number | [-n name] [-s status]] */
                number = pit_arg_number(++arg, NULL);
                if (number) {
                    project_show(number);
                } else {
                    project_parse_options(--arg, &name, &status);
                    if (!name && !status) {
                        project_show(0); /* Show current project if any. */
                    } else {
                        project_list(name, status);
                    }
                }
                break;
            default:
                die("invalid project option: %s", *arg);
            }
        }
    }
}
