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
    int     max_id;
    int     max_name;
    int     max_status;
    int     max_priority;
    int     min_deadline;
    uchar  *entries;
} Pager, *PPager;

PPager pit_pager_initialize(ulong number_of_entries);
uchar *pit_pager_add(PPager pp, uchar *entry);
void   pit_pager_dump(PPager pp);
void   pit_pager_free(PPager pp, int deep);

#endif