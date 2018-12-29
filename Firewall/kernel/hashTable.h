#ifndef HASHTABLE_H_
#define HASHTABLE_H_

#include <linux/slab.h>
#include <linux/string.h>
#include <linux/spinlock.h>

struct key_value{
    struct key_value* next;
    char *key;
    void *value;
};

typedef struct HashTable{
    struct key_value **table;
    void (*free_value)(void*);
}HashTable;

HashTable* hash_table_new(void(*free_value)(void*));
void  hash_table_delete(HashTable *ht);
int   hash_table_put(HashTable *ht, char *key, void *value);
void* hash_table_get(HashTable *ht, char *key);
void  hash_table_rm (HashTable *ht, char *key);
char* hash_table_str(HashTable *ht, char*(*to_string)(void*));
void  hash_table_update(HashTable *ht, bool(*func)(void*));
void  hash_table_clear(HashTable *ht, bool(*func)(void*, int), int index);

#endif