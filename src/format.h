#if !defined(__PAGER_H__)
#define __PAGER_H__

#define PAGER_ACTION  1
#define PAGER_PROJECT 2
#define PAGER_TASK    4
#define PAGER_NOTE    8

typedef struct _Pager {
    int   type;
    int   indent;
    int   number_of_entries;
    char *entries;
    union {
        struct {
            int id;
            int username;
            int name;
            int status;
        } project;
        struct {
            int id;
            int username;
            int name;
            int status;
            int priority;
            int date;
            int time;
        } task;
        struct {
            int username;
            int message;
        } action;
        struct {
            int id;
            int username;
        } note;
    } max;
} Pager, *PFormat;

PFormat pit_pager_initialize(int type, int indent, int number_of_entries);
void   pit_pager_print(PFormat pf, char *entry);
void   pit_pager_flush(PFormat pf);
void   pit_pager_free(PFormat pf);

#endif