#if !defined(__TABLE_H__)
#define __TABLE_H__

typedef struct {
    ulong   flags;              /* Bit mask with table flags */
    ulong   record_size;        /* Record size in bytes; all records are of fixed size */
    ulong   number_of_slots;    /* Number of slots allocated, each slot is 'record_size' long */
    ulong   number_of_records;  /* Number of records currently stored in slots */
    ulong   auto_increment;     /* Current value of record id */
    uchar  *slots;              /* Memory chunk to store records; compacted when a record gets deleted (no holes) */
    uchar **index;              /* Memory chunk to store pointers to individual records, holes for deleted record ids */
} Table, *PTable;

PTable table_initialize(ulong record_size, ulong flags);
PTable table_free(PTable pt);
uchar *table_find(PTable pt, ulong id);
uchar *table_delete(PTable pt, ulong id);
uchar *table_insert(PTable pt, uchar *record);
int table_save(FILE *file, PTable pt);
PTable table_load(FILE *file);

static uchar *table_available_slot(PTable pt);
static uchar *table_last_record(PTable pt);
static uchar **table_available_index(PTable pt);
static uchar **table_last_index(PTable pt);
static PTable table_extend(PTable pt);

#endif