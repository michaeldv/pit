#if !defined(__MODELS_H__)
#define __MODELS_H__

typedef struct _Project {
    ulong  id;
    char   name[128];               /* Project name. */
    char   status[16];              /* Project status. */
    ulong  number_of_open_tasks;    /* Number of open tasks. */
    ulong  number_of_closed_tasks;  /* Number of closed tasks. */
    ulong  closed_by;               /* Who closed the project? */
    ulong  created_by;              /* Who created the project? */
    ulong  updated_by;              /* Who last updated the project? */
    time_t closed_at;               /* When the project was closed? */
    time_t created_at;              /* When the project was created? */
    time_t updated_at;              /* When the project was last updated? */
} Project, *PProject;

typedef struct _Task {
    ulong  id;
    ulong  project_id;              /* Which project the task belongs to? */
    char   name[128];               /* Task name. */
    char   status[16];              /* Task status. */
    char   priority[16];            /* Task priority. */
    time_t deadline;                /* Task deadline. */
    ulong  number_of_notes;         /* Number of notes. */
    ulong  closed_by;               /* Who closed the task? */
    ulong  created_by;              /* Who created the task? */
    ulong  updated_by;              /* Who last updated the task? */
    time_t closed_at;               /* When the task was closed? */
    time_t created_at;              /* When the task was created? */
    time_t updated_at;              /* When the task was last updated? */
} Task, *PTask;

typedef struct _Note {
    ulong  id;
    ulong  task_id;                 /* Task the note belongs to. */
    char   message[255];            /* The body of the note. */
    ulong  created_by;              /* Who created the note? */
    ulong  updated_by;              /* Who last updated the note? */
    time_t created_at;              /* When the note was created? */
    time_t updated_at;              /* When the note was last updated? */
} Note, *PNote;

typedef struct _Activity {
    ulong  subject_id;              /* Reference to the specific Project, Task, or Note. */
    char   subject[16];             /* Project, Task, or Note. */
    char   message[255];            /* Log message. */
    ulong  created_by;              /* Who added log message? */
    time_t created_at;              /* When log message was added? */    
} Activity, *PActivity;

typedef struct _User {
    ulong  id;
    char   username[32];            /* Username. */
    char   email[32];               /* User's email. */
} User, *PUser;

#endif
