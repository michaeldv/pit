#if !defined(__MODELS_H__)
#define __MODELS_H__

typedef struct _Header {
    char   signature[3];
    int    schema_version;
    char   created_by[32];
    char   updated_by[32];
    time_t created_at;
    time_t updated_at;
} Header, *PHeader;
    
typedef struct _Project {
    int    id;
    char   username[32];            /* User the project belongs to. */
    char   name[128];               /* Project name. */
    char   status[16];              /* Project status. */
    int    number_of_tasks;         /* Number of tasks for the project. */
    time_t created_at;              /* When the project was created? */
    time_t updated_at;              /* When the project was last updated? */
} Project, *PProject;

typedef struct _Task {
    int    id;
    int    project_id;              /* Which project the task belongs to? */
    char   username[32];            /* User the task belongs to. */
    char   name[128];               /* Task name. */
    char   status[16];              /* Task status. */
    char   priority[16];            /* Task priority. */
    time_t date;                    /* Generic date/time, ex: task deadline. */
    time_t time;                    /* Generic time, ex: time spent on the task. */
    int    number_of_notes;         /* Number of notes for the task. */
    time_t created_at;              /* When the task was created? */
    time_t updated_at;              /* When the task was last updated? */
} Task, *PTask;

typedef struct _Note {
    int    id;
    int    project_id;              /* Project the note belongs to (0 if belongs to task). */
    int    task_id;                 /* Task the note belongs to (0 if belongs to project). */
    char   username[32];            /* User who created the note. */
    char   message[255];            /* The body of the note. */
    time_t created_at;              /* When the note was created? */
    time_t updated_at;              /* When the note was last updated? */
} Note, *PNote;

typedef struct _Action {
    int    project_id;              /* Project id (always set). */
    int    task_id;                 /* Task id (set for task or note related actions). */
    int    note_id;                 /* Note id (set for note related actions only). */
    char   username[32];            /* Who added the log message? */
    char   message[255];            /* Log message. */
    time_t created_at;              /* When log message was added? */    
} Action, *PAction;

typedef union _Options {
    struct {
        int    id;
        char  *name;
        char  *status;
        char  *priority;
        time_t date;
        time_t date_max;
        time_t time;
        time_t time_max;
    } task;
    struct {
        int    id;
        char  *name;
        char  *status;
    } project;
    struct {
        int    id;
        char  *message;
    } note;
} Options, *POptions;

#endif
