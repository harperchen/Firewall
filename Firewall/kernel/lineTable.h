#ifndef LINETABLE_H_
#define LINETABLE_H_

#include <linux/slab.h>
#include <linux/string.h>
#include <linux/spinlock.h>

typedef struct LineTable{
    void **table;
    bool (*comp_value)(void *, void *);
	void (*free_value)(void *);
    bool (*search_value)(void *, void *, unsigned int);
    int size;
}LineTable;

LineTable* line_table_new(bool(*comp_value)(void *, void *), void(*free_value)(void*), bool(*search_value)(void*, void*, unsigned int));
void line_table_delete(LineTable* lt);
int  line_table_put(LineTable *lt, void *value);
int  line_table_search(LineTable *lt, void *value, unsigned int hooknum);
int  line_table_get(LineTable *lt, void *value);
int line_table_rm(LineTable *lt, int index);
int line_table_clear(LineTable *lt, bool(*func)(void *, int), int index);

#endif