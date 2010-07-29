#if !defined(__PAGER_H__)
#define __PAGER_H__

#define PAGER_ACTIVITY  1
#define PAGER_PROJECT   2
#define PAGER_TASK      4
#define PAGER_NOTE      8
#define PAGER_USER     16

typedef struct _Pager {
    ulong   type;
    ulong   number_of_entries;
    uchar   *entries;
    union {
        struct {
            int id;
            int name;
            int status;
        } project;
        struct {
            int id;
            int name;
            int status;
            int priority;
            int deadline;
        } task;
    } max;
} Pager, *PPager;

PPager pit_pager_initialize(ulong type, ulong number_of_entries);
void   pit_pager_print(PPager ppager, uchar *entry);
void   pit_pager_flush(PPager ppager);
void   pit_pager_free(PPager ppager);

#endif