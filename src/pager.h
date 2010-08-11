#if !defined(__PAGER_H__)
#define __PAGER_H__

#define PAGER_ACTION  1
#define PAGER_PROJECT 2
#define PAGER_TASK    4
#define PAGER_NOTE    8

typedef struct _Pager {
    int   type;
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
            int subject;
        } action;
    } max;
} Pager, *PPager;

PPager pit_pager_initialize(int type, int number_of_entries);
void   pit_pager_print(PPager ppager, char *entry);
void   pit_pager_flush(PPager ppager);
void   pit_pager_free(PPager ppager);

#endif