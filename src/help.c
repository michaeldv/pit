#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "pit.h"

static void usage()
{
    char *msg[] = {
        "usage: pit command [args]\n",
        "The commands are:",
        "   init       Create empty pit database or reinitialize an existing one",
        "   project    Create, search, and manage pit projects",
        "   task       Create, search, and manage pit tasks",
        "   note       Create, search, and manage pit notes",
        "   log        Show chronological pit activity log",
        "   info       Show summary information about your pit database",
        "   help       Show help information about pit",
        "   version    Show pit version number\n",
        "All commands might be shortened as long as they remain unambiguous. See 'pit help <command>' for more",
        "information on a specific command.\n", NULL
    };
    printa(msg);
}

static void help_project()
{
    char *msg[] = {
        "Pit project is basic entity used to group related tasks together. In pit project has name and status, neither",
        "of which have any semantical meaning and are just arbitrary strings.\n", 
        "Creating a project:\n",
        "   $ pit project -c name [-s status]\n",
        "Editing a project:\n",
        "   $ pit project -e [number] [-n name] [-s status]\n",
        "Deleting a project:\n",
        "   $ pit project -d [number]\n",
        "Viewing a project:\n",
        "   $ pit project [[-q] number]\n",
        "Searching projects:\n",
        "   $ pit project -q [number | [-n name] [-s status]]\n",
        "Examples:\n",
        "   $ pit project -c 'My Tasks'",
        "   created project 1: My Tasks (status: active)\n",
        "   $ pit project -c 'Feature Requests' -s backlog",
        "   created project 2: Feature Requests (status: backlog)\n",
        "   $ pit pro",
        "     1: (username) [active ] My Tasks         (0 tasks)",
        "   * 2: (username) [backlog] Feature Requests (0 tasks)\n",
        "   $ pit p 1 -e -n 'Task and Bugs' -s current",
        "   1: Task and Bugs (current, 0 tasks)\n",
        "   $ pit p",
        "     1: (username) [current] Task and Bugs    (0 tasks)",
        "   * 2: (username) [backlog] Feature Requests (0 tasks)\n",
        "   $ pit p -d",
        "   deleted project 2: Feature Requests\n",
        "   $ pit p",
        "   * 1: (username) [current] Task and Bugs (0 tasks)\n", NULL
    };
    printa(msg);
}

static void help_task()
{
    char *msg[] = {
        "In pit a task belongs to particular project. A task has name, status, priority, date, and time. All attributes",
        "except the task name are optional.\n",
        "Creating a task:\n",
        "   pit task -c name [-s status] [-p priority] [-d date] [-t time]\n",
        "Editing a task:\n",
        "   pit task -e [number] [-n name] [-s status] [-p priority] [-d date] [-t time]\n",
        "Moving a task:\n",
        "   pit task -m [number] -p number\n",
        "Deleting a task:\n",
        "   pit task -d [number]\n",
        "Viewing a task:\n",
        "   pit task [[-q] number]\n",
        "Searching tasks:\n",
        "   pit task -q [number | [-n name] [-s status] [-p priority] [-d date-from] [-D date-to] [-t time-min] [-T time-max]]\n",
        "Supported date formats:\n",
        "   none, 4/26, 4/26/2012, 4/26/12, '4/26 3pm', '4/26 19:30', '4/26/2012 3:15am', '4/26/12 17:00'",
        "   'Apr 26', 'Apr 26, 2012', 'Apr 26 3pm', 'Apr 26 19:30', 'Apr 26, 12 3:15am', 'Apr 26, 2012 17:00'\n",
        "Supported time formats:\n",
        "   none, 17, 17:00, 17:30, 5pm, 1:15am\n",
        "Examples:\n",
        "   $ pit task -c 'Hack this'",
        "   created task 1/1: Hack this (status: open, priority: normal)\n",
        "   $ pit task -c 'And hack that' -s new -p urgent -d 'Dec 31'",
        "   created task 2/1: And hack that (status: new, priority: urgent, date: Dec 31, 2010)\n",
        "   $ pit t",
        "     1: (username) [open] [normal]              Hack this     (0 notes)",
        "   * 2: (username) [new ] [urgent] Dec 31, 2010 And hack that (0 notes)\n",
        "   $ pit t -e 1 -s done -d 10/10 -t 4:30",
        "   updated task 1: Hack that (status: done, date: Oct 10, 2010, time: 4:30)\n",
        "   $ pit t",
        "     1: (username) [done] [normal] Oct 10, 2010 4:30 Hack this     (0 notes)",
        "   * 2: (username) [new]  [urgent] Dec 31, 2010      And hack that (0 notes)\n",
        "   $ pit t -d",
        "   deleted task 2: And hack that\n",
        "   $ pit t",
        "   1: (username) [done] [normal] Oct 10, 2010 4:30 Hack this (0 notes)\n", NULL
    };
    printa(msg);
}

static void help_note()
{
    puts("pit note is not implemented yet.");
}

static void help_log()
{
    char *msg[] = {
        "Show summary information about your pit database. This command is as simple as:\n",
        "   pit log\n", NULL
    };
    printa(msg);
}

static void help_init()
{
    char *msg[] = {
        "Create empty pit database or reinitialize an existing one. Default file name for the pit database",
        "is ~/.pit -- you can override the default by setting PITFILE environment variable.\n",
        "   $ pit init [-f]\n",
        "   -f   force initialization without prompt\n",
        "Example:\n",
        "   $ pit init",
        "   /home/user/.pit already exists, do you want to override it [y/N]: y",
        "   Created empty /home/user/.pit\n", NULL
    };
    printa(msg);
}

static void help_info()
{
    char *msg[] = {
        "Show summary information about your pit database. This command is as simple as:\n",
        "   pit info\n", NULL
    };
    printa(msg);
}

static void help_version()
{
    puts("No kidding :-)");
    pit_version();
}

void pit_help(char *argv[])
{
    char *command[] = { "project", "task", "note", "log", "init", "info", "help", "version" };
    void (*handler[])() = { help_project, help_task, help_note, help_log, help_init, help_info, usage, help_version };

    if (!argv[1]) {
        usage();
    } else {
        register int i, candidate = -1;

        for(i = 0;  i < ARRAY_SIZE(command);  i++) {
            if (strstr(command[i], argv[1]) == command[i]) {
                if (candidate < 0) {
                    candidate = i;
                } else {
                    printf("no help, <%s> is a bit ambiguous\n", argv[1]);
                    usage();
                }
            }
        }

        if (candidate < 0) {
            printf("no help, <%s> is unknown command\n", argv[1]);
            usage();
        } else {
            handler[candidate]();
        }
    }
}