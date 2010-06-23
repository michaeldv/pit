#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define TABLE_INCREMENT       5
#define TABLE_HAS_ID          1
#define TABLE_HAS_CREATED_AT  2
#define TABLE_HAS_UPDATED_AT  4

typedef struct {
    unsigned long  flags;
    unsigned long  record_size;
    unsigned long  number_of_slots;
    unsigned long  number_of_records;
    unsigned long  autoincrement;
    unsigned char *records;
    unsigned char *index;
} Table, *PTable;


PTable table_initialize(unsigned long record_size, unsigned long flags) {
    PTable pt;

    pt = calloc(1, sizeof(Table));
    pt->flags             = flags | TABLE_HAS_ID;
    pt->record_size       = record_size;
    pt->number_of_slots   = TABLE_INCREMENT;
    pt->number_of_records = 0;
    pt->autoincrement     = 0;
    pt->records           = calloc(TABLE_INCREMENT, pt->record_size);
    pt->index             = calloc(TABLE_INCREMENT, sizeof(unsigned long *));

    return pt;
}

unsigned char *table_next_record(PTable pt) {
    return pt->records + pt->number_of_records * pt->record_size;
}

unsigned char *table_last_record(PTable pt) {
    if (pt->number_of_records == 0) {
        return pt->records;
    } else {
        return table_next_record(pt) - pt->record_size;
    }
}

unsigned char *table_next_index(PTable pt) {
    return pt->index + pt->autoincrement * sizeof(unsigned long *);
}

unsigned char *table_last_index(PTable pt) {
    if (pt->autoincrement == 0) {
        return pt->index;
    } else {
        return table_last_index(pt) - sizeof(unsigned long *);
    }
}


PTable table_extend(PTable pt) {
    register long i;
    unsigned long *pi;

    pt->number_of_slots += TABLE_INCREMENT;

    /* Re-allocate the records and set newly added memory chunk to 0. */
    pt->records = realloc(pt->records, pt->number_of_slots * pt->record_size);
    memset(table_next_record(pt), 0, TABLE_INCREMENT * pt->record_size);

    /* Re-allocate the index and set newly added memory chunk to 0. */
    pt->index = realloc(pt->index, pt->number_of_slots * sizeof(unsigned long *));
    memset(table_next_index(pt), 0, TABLE_INCREMENT * sizeof(unsigned long *));

    /* Reassign index entries to point to the re-allocatted records. */
    for (i = 0, pi = (unsigned long *)pt->index;  i < pt->autoincrement;  i++, pi++) {
        *pi = (unsigned long)(pt->records + pt->record_size * i);
    }

    return pt;
}

unsigned long *table_find(PTable pt, unsigned long id) {
    if (pt->number_of_records == 0 || id < 0 || id > pt->autoincrement) {
        return NULL;
    } else {
        return (unsigned long *) *((unsigned long *)pt->index + id);
    }
}

PTable table_insert(PTable pt, unsigned long *record) {
    unsigned long **pi;

    if (pt->number_of_records >= pt->number_of_slots) {
        pt = table_extend(pt);
    }

    memmove(table_next_record(pt), record, pt->record_size);
    // printf("rec: %lu, %08lX, mov: %08lX, next: %08lX\n", (unsigned long)*record, (unsigned long)*record, (unsigned long)*table_next_record(pt), (unsigned long)table_next_record(pt));
    pi = (unsigned long **)table_next_index(pt);
    
    *pi = (unsigned long *)table_next_record(pt);
    pt->number_of_records++;
    pt->autoincrement++;

    return pt;
}



PTable table_free(PTable pt) {
    if (pt) {
        if (pt->index) {
            free(pt->index);
        }
        if (pt->records) {
            free(pt->records);
        }
        free(pt);
    }

    return NULL;
}


int main() {
    PTable pt;
    typedef struct {
        unsigned long id;
        char name[30];
    } Record;

    Record rec, *prec;
    int i;

    pt = table_initialize(sizeof(Record), 0);
    for(i = 0;  i <= 100;  i++) {
        rec.id = i;
        pt = table_insert(pt, (unsigned long *)&rec);
        printf("%08lX, %08lX\n", (unsigned long)*table_last_record(pt), (unsigned long)table_last_record(pt));
    }

    for(i = 100;  i >= 0;  i--) {
        prec = (Record *)table_find(pt, i);
        printf("%lu\n", prec->id);
    }

    table_free(pt);
    printf("OK\n");
}