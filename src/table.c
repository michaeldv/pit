#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef unsigned long ulong;
typedef unsigned char uchar;

#define TABLE_INCREMENT       5
#define TABLE_HAS_ID          1
#define TABLE_HAS_CREATED_AT  2
#define TABLE_HAS_UPDATED_AT  4
#define TABLE_HAS_TIMESTAMPS  (TABLE_HAS_CREATED_AT | TABLE_HAS_UPDATED_AT)

typedef struct {
    ulong   flags;              /* Bit mask with table flags */
    ulong   record_size;        /* Record size in bytes; all records are of fixed size */
    ulong   number_of_slots;    /* Number of slots allocated, each slot is 'record_size' long */
    ulong   number_of_records;  /* Number of records currently stored in slots */
    ulong   auto_increment;     /* Current value of record id */
    uchar  *slots;              /* Memory chunk to store records; compacted when a record gets deleted (no holes) */
    uchar **index;              /* Memory chunk to store pointers to individual records, holes for deleted record ids */
} Table, *PTable;

/*
** Initialize the table by alloocating necessary memory chunks.
*/
PTable table_initialize(ulong record_size, ulong flags) {
    PTable pt;

    pt = calloc(1, sizeof(Table));
    pt->flags             = flags | TABLE_HAS_ID;
    pt->record_size       = record_size;
    pt->number_of_slots   = TABLE_INCREMENT;
    pt->number_of_records = 0;
    pt->auto_increment    = 0;
    pt->slots             = calloc(TABLE_INCREMENT, pt->record_size);
    pt->index             = calloc(TABLE_INCREMENT, sizeof(uchar *));

    return pt;
}

/*
** Return the address of next avaiable slot within pt->slots chunk.
*/
uchar *table_available_slot(PTable pt) {
    return pt->slots + pt->number_of_records * pt->record_size;
}

/*
** Return the address of last stored record.
*/
uchar *table_last_record(PTable pt) {
    if (pt->number_of_records == 0) {
        return pt->slots;
    } else {
        return table_available_slot(pt) - pt->record_size;
    }
}

/*
** Return the address of next available pointer within pt->index chunk.
*/
uchar **table_available_index(PTable pt) {
    return pt->index + pt->auto_increment;
}

/*
** Return the address of last pointer within pt->index chunk.
*/
uchar **table_last_index(PTable pt) {
    if (pt->auto_increment == 0) {
        return pt->index;
    } else {
        return table_last_index(pt) - sizeof(char *);
    }
}

/*
** Extend the table involves three steps. First, we reallocate pt->slots
** chunk adding TABLE_INCREMENT empty slots. Then we add the same number
** of empty indices to the the pt->index chunk. Finally, we adjust
** existing indices to make them point to reallocated record slots.
*/
PTable table_extend(PTable pt) {
    register long i;
    register uchar **pi;

    puts("EXTENDING...\n");
    pt->number_of_slots += TABLE_INCREMENT;

    /* Re-allocate the records and set newly added memory chunk to 0. */
    pt->slots = realloc(pt->slots, pt->number_of_slots * pt->record_size);
    memset(table_available_slot(pt), 0, TABLE_INCREMENT * pt->record_size);

    /* Re-allocate the index and set newly added memory chunk to 0. */
    pt->index = realloc(pt->index, pt->number_of_slots * sizeof(char *));
    memset(table_available_index(pt), 0, TABLE_INCREMENT * sizeof(char *));

    /* Reassign index entries to point to the re-allocatted records. */
    for (i = 0, pi = pt->index;  i < pt->auto_increment;  i++, pi++) {
        if (*pi != NULL) {
            *pi = pt->slots + i * pt->record_size;
        }
    }

    return pt;
}

uchar *table_find(PTable pt, ulong id) {
    if (pt->number_of_records == 0 || id < 0 || id > pt->auto_increment) {
        return NULL;
    } else {
        return *(pt->index + id);
    }
}

uchar *table_insert(PTable pt, uchar *record) {
    uchar **pi;

    if (pt->number_of_records >= pt->number_of_slots) {
        pt = table_extend(pt);
    }

     pi = table_available_index(pt);
    *pi = table_available_slot(pt);
    memmove(table_available_slot(pt), record, pt->record_size);

    pt->number_of_records++;
    pt->auto_increment++;

    return *pi;
}



PTable table_free(PTable pt) {
    if (pt) {
        if (pt->index) {
            free(pt->index);
        }
        if (pt->slots) {
            free(pt->slots);
        }
        free(pt);
    }

    return NULL;
}


int main() {
    PTable pt;
    typedef struct {
        ulong id;
        char name[30];
    } Record;

    Record rec, *prec;
    int i;

    pt = table_initialize(sizeof(Record), 0);
    for(i = 0;  i <= 100;  i++) {
        rec.id = i;
        prec = (Record *)table_insert(pt, (uchar *)&rec);
        printf("%08lX, %08lX (id: %lu)\n", (ulong)*table_last_record(pt), (ulong)table_last_record(pt), prec->id);
    }

    for(i = 100;  i >= 0;  i--) {
        prec = (Record *)table_find(pt, i);
        printf("id: %lu\n", prec->id);
    }

    table_free(pt);
    printf("OK\n");
    return 0;
}