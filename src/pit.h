#if !defined(__PIT_H__)
#define __PIT_H__
#define PIT_VERSION "0.1.0"
#define PIT_SCHEMA_VERSION 1

typedef int bool;

#define __USE_XOPEN
#include <time.h>       /* __USE_XOPEN needed for strptime() */
#include "object.h"
#include "table.h"
#include "format.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

#ifndef FALSE
#define FALSE (0)
#endif

#ifndef TRUE
#define TRUE (1)
#endif

#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif

#ifndef max
#define max(a,b) ((a) > (b) ? (a) : (b))
#endif

#define for_each_project(ptr) PProject ptr; for (ptr = (PProject)projects->slots; (ptr - (PProject)projects->slots) < projects->number_of_records; ptr++)
#define for_each_task(ptr)    PTask ptr;    for (ptr = (PTask)tasks->slots;       (ptr - (PTask)tasks->slots)       < tasks->number_of_records;    ptr++)
#define for_each_note(ptr)    PNote ptr;    for (ptr = (PNote)notes->slots;       (ptr - (PNote)notes->slots)       < notes->number_of_records;    ptr++)
#define for_each_action(ptr)  PAction ptr;  for (ptr = (PAction)actions->slots;   (ptr - (PAction)actions->slots)   < actions->number_of_records;  ptr++)

/* Externals. */
extern PHeader header;
extern PTable  actions;
extern PTable  notes;
extern PTable  projects;
extern PTable  tasks;

/* Command handlers and database APIs */
void pit_init(char *argv[]);
void pit_info(char *argv[]);
void pit_project(char *argv[]);
void pit_task(char *argv[]);
void pit_note(char *argv[]);
void pit_log(char *argv[]);
void pit_info(char *argv[]);
void pit_help(char *argv[]);
void pit_version();
void pit_db_load();
void pit_db_save();
void pit_db_initialize();
void pit_action(PAction pa);
void pit_task_list(POptions po, PProject pp);
void pit_task_delete(int id, PProject pp);
void pit_note_list(PTask pt);
void pit_note_delete(int id, PTask pt);


/* Argument parsing helpers */
int    pit_arg_is_option(char **arg);
int    pit_arg_option(char **arg);
char  *pit_arg_string(char **arg, char *required);
int    pit_arg_number(char **arg, char *required);
time_t pit_arg_date(char **arg, char *required);
time_t pit_arg_time(char **arg, char *required);

/* Misc utilities */
void  die(char *msg, ...);
void  perish(char *prefix);
char *str2str(char *str);
char *mem2str(char *mem, int len);
bool  is_zero(char *mem, int len);
char *stristr(char *haystack, char *needle);
void  printa(char *msg[]);
char *current_user();
char *home_dir(char *username, int len);
char *expand_path(char *path, char *expanded);
char *format_date(time_t date);
char *format_time(time_t time);
char *format_timestamp(time_t timestamp);
char *inline_replace(char *this, char *old, char *new);

#endif
