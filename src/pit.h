#if !defined(__PIT_H__)
#define __PIT_H__

typedef unsigned int  uint;
typedef unsigned long ulong;
typedef unsigned char uchar;
typedef int bool;

#include <time.h>
#include "object.h"
#include "table.h"
#include "pager.h"

/* #defines */
#ifndef FALSE
#define FALSE (0)
#endif

#ifndef TRUE
#define TRUE  (1)
#endif

#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif

#ifndef max
#define max(a,b) ((a) > (b) ? (a) : (b))
#endif

#define for_each_activity(ptr) for (ptr = (PActivity)activities->slots; (ptr - (PActivity)activities->slots) < activities->number_of_records; ptr++)
#define for_each_project(ptr)  for (ptr = (PProject)projects->slots; (ptr - (PProject)projects->slots) < projects->number_of_records; ptr++)
#define for_each_note(ptr)     for (ptr = (PNote)notes->slots; (ptr - (PNote)notes->slots) < notes->number_of_records; ptr++)
#define for_each_task(ptr)     for (ptr = (PTask)tasks->slots; (ptr - (PTask)tasks->slots) < tasks->number_of_records; ptr++)
#define for_each_user(ptr)     for (ptr = (PUser)users->slots; (ptr - (PUser)users->slots) < users->number_of_records; ptr++)

/* Externals. */
extern PTable activities;
extern PTable notes;
extern PTable projects;
extern PTable tasks;
extern PTable users;

/* args.c */
int    pit_arg_is_option(char **arg);
int    pit_arg_option(char **arg);
char  *pit_arg_string(char **arg, char *required);
ulong  pit_arg_number(char **arg, char *required);
time_t pit_arg_time(char **arg, char *required);

/* db.c */
int  pit_init(char *argv[]);
void pit_db_load();
void pit_db_save();
void pit_db_initialize();

/* activity.c project.c task.c user.c */
int   pit_project(char *argv[]);
int   pit_task(char *argv[]);
char *pit_current_user();
PActivity pit_add_activity(ulong id, char *subject, char *message, ulong user_id);

/* util.c */
void die(char *msg, ...);
void perish(char *prefix);
char *mem2str(char *mem, int len);
char *home_dir(char *username, int len);
char *expand_path(char *path, char *expanded);


#endif
