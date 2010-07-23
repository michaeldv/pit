#if !defined(__PIT_H__)
#define __PIT_H__

typedef unsigned int  uint;
typedef unsigned long ulong;
typedef unsigned char uchar;

#include <time.h>
#include "models.h"
#include "table.h"

/* Externals. */
extern PTable projects;
extern PTable tasks;
extern PTable notes;
extern PTable activities;
extern PTable users;

/* Command line parsing. */
int    pit_arg_is_option(char **arg);
int    pit_arg_option(char **arg);
char  *pit_arg_string(char **arg, char *required);
ulong  pit_arg_number(char **arg, char *required);
time_t pit_arg_time(char **arg, char *required);

/* Database. */
int  pit_init(char *argv[]);
void pit_db_load();
void pit_db_save();
void pit_db_initialize();

/* Models. */
int   pit_project(char *argv[]);
int   pit_task(char *argv[]);
char *pit_current_user();
PActivity pit_add_activity(ulong id, char *subject, char *message, ulong user_id);

/* Utilities. */
void die(char *msg, ...);
void perish(char *prefix);

#endif
