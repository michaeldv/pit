#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pit.h"

#define TABLE_INCREMENT 10
/*
** Initialize the table by alloocating necessary memory chunks.
*/
PTable pit_table_initialize(int record_size, int flags)
{
    PTable pt = calloc(1, sizeof(Table));

    pt->flags             = flags;
    pt->record_size       = record_size;
    pt->number_of_slots   = TABLE_INCREMENT;
    pt->number_of_records = 0;
    pt->auto_increment    = 0;
    pt->current           = 0;
    pt->index_size        = TABLE_INCREMENT;
    pt->slots             = calloc(TABLE_INCREMENT, pt->record_size);
    pt->index             = calloc(TABLE_INCREMENT, sizeof(char *));

    return pt;
}

/*
** Return the address of next avaiable slot within pt->slots chunk.
*/
static char *table_available_slot(PTable pt)
{
    return pt->slots + pt->number_of_records * pt->record_size;
}

/*
** Return the address of last stored record.
*/
static char *table_last_record(PTable pt)
{
    if (pt->number_of_records == 0) {
        return pt->slots;
    } else {
        return table_available_slot(pt) - pt->record_size;
    }
}

/*
** Return the address of next available pointer within pt->index chunk.
*/
static char **table_available_index(PTable pt)
{
    return HAS_ID(pt) ? (pt->index + pt->auto_increment) : NULL;
}

/*
** Return the address of last pointer within pt->index chunk.
*/
static char **table_last_index(PTable pt)
{
    if (HAS_ID(pt)) {
        if (pt->auto_increment == 0) {
            return pt->index;
        } else {
            return table_last_index(pt) - sizeof(char *);
        }
    }
    return NULL;
}

/*
** Re-allocate the index and set newly added memory chunk to 0.
*/
static char **table_extend_index(PTable pt)
{
    if (HAS_ID(pt)) {
        pt->index_size += TABLE_INCREMENT;
        pt->index = realloc(pt->index, pt->index_size * sizeof(char *));
        memset(table_available_index(pt), 0, TABLE_INCREMENT * sizeof(char *));
        return pt->index;
    }

    return NULL;
}

/*
** Extend the table involves three steps. First, reallocate pt->slots
** chunk adding TABLE_INCREMENT empty slots. Then extend the index.
** Finally, adjust existing indices to point to reallocated record slots.
*/
static PTable table_extend(PTable pt)
{
    register int i;
    register char **pi;

    pt->number_of_slots += TABLE_INCREMENT;
    /*
    ** Re-allocate the records and set newly added memory chunk to 0.
    */
    pt->slots = realloc(pt->slots, pt->number_of_slots * pt->record_size);
    memset(table_available_slot(pt), 0, TABLE_INCREMENT * pt->record_size);

    if (HAS_ID(pt)) {
        /*
        ** Extend the index and make its entries point to reallocatted records.
        */
        table_extend_index(pt);
        for (i = 0, pi = pt->index;  i < pt->auto_increment;  i++, pi++) {
            if (*pi != NULL) {
                *pi = pt->slots + i * pt->record_size;
            }
        }
    }
    return pt;
}

/*
** Find a record by id and return its address.
*/
char *pit_table_find(PTable pt, int id)
{
    if (HAS_ID(pt)) {
        if (pt->number_of_records == 0 || id <= 0 || id > pt->auto_increment) {
            return NULL;
        } else {
            return *(pt->index + id - 1);
        }
    }
    return NULL;
}

/*
** Delete a record by its id. Return the address of deleted record or NULL
** if the record was not found.
*/
char *pit_table_delete(PTable pt, int id)
{
    register char *pr = (char *)pit_table_find(pt, id);

    if (pr) {
        register char *last = table_last_record(pt);
        /*
        ** Overwrite current record by shifting over remaining slots.
        */
        if (pr != last) {
            memmove(pr, pr + pt->record_size, last - pr);
        }
        /*
        ** Set the slot occupied by the last record to zero.
        */
        memset(last, 0, pt->record_size);

        if (HAS_ID(pt)) {
            register int i;
            register char **pi;
            /*
            ** Set current record pointer to NULL, then update the rest of
            ** the index to point to the shifted records.
            */
            pi = pt->index + id - 1;
           *pi++ = NULL;

            for (i = id;  i < pt->auto_increment; i++) {
                *pi++ -= pt->record_size;
            }
        }
        pt->number_of_records--;
    }

    return pr;
}

/*
** Insert a record and return its address. The table gets extended
** as necessary.
*/
char *pit_table_insert(PTable pt, char *record)
{
    register char *pr;
    register time_t now = time(NULL);

    if (pt->number_of_records >= pt->number_of_slots) {
        pt = table_extend(pt);
    } else {
        if (HAS_ID(pt) && pt->auto_increment >= pt->index_size) {
            table_extend_index(pt);
        }
    }

    pr = table_available_slot(pt);
    memmove(pr, record, pt->record_size);
    pt->number_of_records++;

    if (HAS_ID(pt)) {
        /*
        ** Save current slot address in the index.
        */
        register char **pi = table_available_index(pt);
        *pi = pr;
        /*
        ** Update record id if the table has primary key. The id must be the first
        ** record field of type "unsigned long".
        */
        pt->auto_increment++;
        *(int *)*pi = pt->auto_increment;
    }
    /*
    ** Update created_at and/or updated_at which must be last one or two record
    ** fields of type "time_t".
    */
    if (HAS_CREATED_AT(pt) || HAS_UPDATED_AT(pt)) {
        *(time_t *)(pr + pt->record_size - sizeof(time_t)) = now;
    }
    if (HAS_CREATED_AT(pt) && HAS_UPDATED_AT(pt)) {
        *(time_t *)(pr + pt->record_size - sizeof(time_t) * 2) = now;
    }
    return pr;
}

/*
** Find current record.
*/
char *pit_table_current(PTable pt)
{
    return pit_table_find(pt, pt->current);
}

/*
** Set current record as indicated by the id, then find and return it.
*/
char *pit_table_mark(PTable pt, int id)
{
    return pit_table_find(pt, pt->current = id);
}

/*
** Release pt->slots and pt->index memory chunks, then free the table itself.
*/
void pit_table_free(PTable pt)
{
    if (pt) {
        if (pt->index) {
            free(pt->index);
        }
        if (pt->slots) {
            free(pt->slots);
        }
        free(pt);
    }
}

/*
** Save the contents of the table to file. The file handle should be
** open with for writing.
*/
int pit_table_save(FILE *file, PTable pt)
{
    register int written = 0;
    /*
    ** Save table header data: flags, record_size, number_of_slots,
    ** number_of_records, auto_increment, current, and index_size.
    */
    written += fwrite(pt, sizeof(int), 7, file);
    /*
    ** Save the records. Note that we save the actual (not allocated) data.
    */
    written += fwrite(pt->slots, pt->record_size, pt->number_of_records, file);
    return written;
}

/*
** Load the contents of the table from file. The file handle should be
** open with for reading.
*/
PTable pit_table_load(FILE *file)
{
    PTable pt;
    register int read = 0;
    register int i;
    char *pr, **pi = NULL;
    
    pt = calloc(1, sizeof(Table));
    /*
    ** First read the header.
    */
    read += fread(pt, sizeof(int), 7, file);
    /*
    ** Now allocate slots and index based on the original number of slots.
    */
    /*** printf("Allocating %d slots\n", pt->number_of_slots); ***/
    pt->slots = pr = calloc(pt->number_of_slots, pt->record_size);
    if (HAS_ID(pt)) {
        pt->index = pi = calloc(pt->index_size, sizeof(char *));
    }
    /*
    ** Now read the records into the slots and rebuild the index if the
    ** table has primary key.
    */
    read += fread(pt->slots, pt->record_size, pt->number_of_records, file);
    if (HAS_ID(pt)) {
        for(i = 1;  i <= pt->number_of_slots;  i++, pi++) {
            if ((int)*pr == i) {
                *pi = pr;
                pr += pt->record_size;
            }
        }
    }
    return pt;
}

/* #define TEST */
#if defined(TEST)

typedef struct {
    int id;
    int value;
    char name[30];
    time_t created_at;
    time_t updated_at;
} Record;

void dump(Record *prec) {
    if (prec) {
        printf("(%08lX) id: %08d, value: %d, created_at: %ld, updated_at: %ld\n",
        (unsigned long)prec, prec->id, prec->value, (time_t)prec->created_at, (time_t)prec->updated_at);
    } else {
        printf("(NULL)\n");
    }
}

void dump_all(PTable pt) {
    int i;
    Record *prec;

    for(i = 1;  i <= pt->number_of_slots;  i++) {
        prec = (Record *)pit_table_find(pt, i);
        if (prec) {
            dump(prec);
        } else {
            printf("%d not found\n", i);
        }
    }
}

int main() {
    PTable pt;

    Record rec, *prec;
    int i, total = 3;

    pt = pit_table_initialize(sizeof(Record), TABLE_HAS_ID | TABLE_HAS_TIMESTAMPS);
    for(i = 0;  i < total;  i++) {
        rec.id = 0;
        rec.value = 0x11223344 + i + 1;
        strcpy(rec.name, "test");
        rec.created_at = rec.updated_at = (time_t)0;

        prec = (Record *)pit_table_insert(pt, (char *)&rec);
        dump(prec);
    }
    prec = (Record *)pit_table_find(pt, total - 1);
    pit_table_mark(pt, prec->id);
    printf("current: %d\n", pt->current);

    printf("Deleting %d\n", 1);
    prec = (Record *)pit_table_delete(pt, 1);
    printf("current: %d\n", pt->current);
    dump_all(pt);

    FILE *file = fopen("/tmp/.pit", "w");
    pit_table_save(file, pt);
    fclose(file);

    file = fopen("/tmp/.pit", "r");
    pt = pit_table_load(file);
    dump_all(pt);
    printf("current: %d\n", pt->current);
    fclose(file);

    pit_table_free(pt);
    printf("OK\n");
    return 0;
}
#endif
