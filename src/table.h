#if !defined(__TABLE_H__)
#define __TABLE_H__

#define TABLE_HAS_ID          1
#define TABLE_HAS_CREATED_AT  2
#define TABLE_HAS_UPDATED_AT  4
#define TABLE_HAS_TIMESTAMPS  (TABLE_HAS_CREATED_AT | TABLE_HAS_UPDATED_AT)

typedef struct {
    ulong   flags;              /* Bit mask with table flags. */
    ulong   record_size;        /* Record size in bytes; all records are of fixed size. */
    ulong   number_of_slots;    /* Number of slots allocated, each slot is 'record_size' long. */
    ulong   number_of_records;  /* Number of records currently stored in slots. */
    ulong   auto_increment;     /* Current value of record id. */
    ulong   current;            /* The id of currently selected record, one per table. */
    uchar  *slots;              /* Memory chunk to store records; compacted when a record gets deleted (no holes). */
    uchar **index;              /* Memory chunk to store pointers to individual records, holes for deleted record IDs. */
} Table, *PTable;

PTable pit_table_initialize(ulong record_size, ulong flags);
PTable pit_table_free(PTable pt);
uchar *pit_table_find(PTable pt, ulong id);
uchar *pit_table_delete(PTable pt, ulong id);
uchar *pit_table_insert(PTable pt, uchar *record);

uchar *pit_table_current(PTable pt);
uchar *pit_table_mark(PTable pt, ulong id);

int    pit_table_save(FILE *file, PTable pt);
PTable pit_table_load(FILE *file);

#endif