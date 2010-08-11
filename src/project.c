#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "pit.h"

static void project_list(POptions po);
static void project_show(int id);
static void project_create(POptions po);
static void project_update(int id, POptions po);
static void project_delete(int id);
static bool project_already_exist(char *name);
static int  project_find_current(int id, PProject *ppp);
static void project_log_create(PProject pp, POptions po);
static void project_log_update(PProject pp, POptions po);
static void project_log_delete(int id, char *name, int number_of_tasks);
static void project_parse_options(char **arg, POptions po);

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

static void project_list(POptions po)
{
    PPager ppager;

    pit_db_load();
    if (projects->number_of_records > 0) {
        ppager = pit_pager_initialize(PAGER_PROJECT, projects->number_of_records);
        for_each_project(pp) {
            pit_pager_print(ppager, (char *)pp);
        }
        pit_pager_flush(ppager);
    }
}

static void project_create(POptions po)
{
    pit_db_load();

    if (project_already_exist(po->project.name)) {
        die("project with the same name already exists");
    } else {
        Project p, *pp;

        memset(&p, 0, sizeof(p));

        if (!po->project.status) po->project.status = "active";

        strncpy(p.name,     po->project.name,   sizeof(p.name)     - 1);
        strncpy(p.status,   po->project.status, sizeof(p.status)   - 1);
        strncpy(p.username, current_user(),     sizeof(p.username) - 1);

        pp = (PProject)pit_table_insert(projects, (char *)&p);
        pit_table_mark(projects, pp->id);

        project_log_create(pp, po);
        pit_db_save();
    }
}

static void project_show(int id)
{
    PProject pp;

    pit_db_load();
    pp = (PProject)pit_table_find(projects, id);
    if (pp) {
        printf("%d: %s (%s, %d task%s)\n",
            pp->id, pp->name, pp->status, pp->number_of_tasks, (pp->number_of_tasks != 1 ? "s" : ""));
        if (pp->number_of_tasks > 0) {
            puts("Tasks:");
            for_each_task(pt) {
                printf("  %c %d: %s (%d notes)\n", (pt->id == tasks->current ? '*' : ' '), pt->id, pt->name, pt->number_of_notes);
            }
        }
        pit_table_mark(projects, pp->id);
        pit_db_save();
    } else {
        die("could not find the project");
    }
}

static void project_update(int id, POptions po)
{
    PProject pp;

    pit_db_load();
    id = project_find_current(id, &pp);

    if (po->project.name)   strncpy(pp->name,   po->project.name,   sizeof(pp->name)   - 1);
    if (po->project.status) strncpy(pp->status, po->project.status, sizeof(pp->status) - 1);
    pit_table_mark(projects, pp->id);

    project_log_update(pp, po);
    pit_db_save();
}

static void project_delete(int id)
{
    PProject pp;

    pit_db_load();
    id = project_find_current(id, &pp);
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
    int deleted_number_of_tasks = pp->number_of_tasks;

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

static int project_find_current(int id, PProject *ppp)
{
    if (id) {
        *ppp = (PProject)pit_table_find(projects, id);
        if (!*ppp) die("could not find project %d", id);
    } else {
        *ppp = (PProject)pit_table_current(projects);
        if (!*ppp) die("could not find current project");
    }
    return *ppp ? (*(PProject *)ppp)->id : 0;
}

static void project_log_create(PProject pp, POptions po)
{
    char str[256];

    sprintf(str, "created project %d: %s (status: %s)", pp->id, po->project.name, po->project.status);
    puts(str);
    pit_action(pp->id, "project", str);
}

static void project_log_update(PProject pp, POptions po)
{
    char str[256];
    bool empty = TRUE;

    sprintf(str, "updated project %d:", pp->id);
    if (po->project.name) {
        sprintf(str + strlen(str), " (name: %s", po->project.name);
        empty = FALSE;
    } else {
        sprintf(str + strlen(str), " %s (", pp->name);
    }
    if (po->project.status) {
        sprintf(str + strlen(str), "%sstatus: %s)", (empty ? "" : ", "), po->project.status);
    }
    strcat(str, ")");
    puts(str);
    pit_action(pp->id, "project", str);
}

static void project_log_delete(int id, char *name, int number_of_tasks)
{
    char str[256];
    
    sprintf(str, "deleted project %d: %s", id, name);
    if (number_of_tasks > 0) {
        sprintf(str + strlen(str), " with %d task%s", number_of_tasks, (number_of_tasks == 1 ? "" : "s"));
    }
    puts(str);
    pit_action(id, "project", str);
}

static void project_parse_options(char **arg, POptions po)
{
    while(*++arg) {
        switch(pit_arg_option(arg)) {
        case 'n':
            po->project.name = pit_arg_string(++arg, "project name");
            break;
        case 's':
            po->project.status = pit_arg_string(++arg, "project status");
            break;
        default:
            die("invalid project option: %s", *arg);
        }
    }
}

void pit_project(char *argv[])
{
    Options opt;
    char **arg = &argv[1];
    int number = 0;

    memset(&opt, 0, sizeof(opt));
    if (!*arg) {
        project_list(&opt); /* Show all projects. */
    } else { /* pit project [number] */
        number = pit_arg_number(arg, NULL);
        if (number) {
            project_show(number);
        } else {
            switch(pit_arg_option(arg)) {
            case 'c': /* pit project -c name [-s status] */
                opt.project.name = pit_arg_string(++arg, "project name");
                project_parse_options(arg, &opt);
                project_create(&opt);
                break;
            case 'e': /* pit project -e [number] [-n name] [-s status] */
                number = pit_arg_number(++arg, NULL);
                if (!number) --arg;
                project_parse_options(arg, &opt);
                if (zero((char *)&opt.project, sizeof(opt.project))) {
                    die("nothing to update");
                } else {
                    project_update(number, &opt);
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
                    project_parse_options(--arg, &opt);
                    if (zero((char *)&opt.project, sizeof(opt.project))) {
                        project_show(0); /* Show current project if any. */
                    } else {
                        project_list(&opt);
                    }
                }
                break;
            default:
                die("invalid project option: %s", *arg);
            }
        }
    }
}
