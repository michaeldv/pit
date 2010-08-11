#if !defined(__TABLE_H__)
#define __TABLE_H__

#define TABLE_HAS_ID          1
#define TABLE_HAS_CREATED_AT  2
#define TABLE_HAS_UPDATED_AT  4
#define TABLE_HAS_TIMESTAMPS  (TABLE_HAS_CREATED_AT | TABLE_HAS_UPDATED_AT)

typedef struct _Table {
    int    flags;              /* Bit mask with table flags. */
    int    record_size;        /* Record size in bytes; all records are of fixed size. */
    int    number_of_slots;    /* Number of slots allocated, each slot is 'record_size' long. */
    int    number_of_records;  /* Number of records currently stored in slots. */
    int    auto_increment;     /* Current value of record id. */
    int    current;            /* The id of currently selected record, one per table. */
    char  *slots;              /* Memory chunk to store records; compacted when a record gets deleted (no holes). */
    char **index;              /* Memory chunk to store pointers to individual records, holes for deleted record IDs. */
} Table, *PTable;

PTable pit_table_initialize(int record_size, int flags);
void   pit_table_free(PTable pt);
char  *pit_table_find(PTable pt, int id);
char  *pit_table_delete(PTable pt, int id);
char  *pit_table_insert(PTable pt, char *record);

char  *pit_table_current(PTable pt);
char  *pit_table_mark(PTable pt, int id);

int    pit_table_save(FILE *file, PTable pt);
PTable pit_table_load(FILE *file);

#endif