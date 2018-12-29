#include "hashTable.h"

#define TABLE_SIZE 1024

static DEFINE_SPINLOCK(spin);

static int num = 0;

static unsigned int hash_function(char *key){
    unsigned int hash = 0;
    while (*key){
       hash = (hash << 5) + hash + *key;
       key++;
    }
    return hash;
}

HashTable *hash_table_new(void(*free_value)(void*)){
    HashTable *ht;
    spin_lock(&spin);
    ht = kmalloc(sizeof(HashTable), GFP_KERNEL);
    if(ht == NULL){
        printk("kmalloc ht fail\n");
        spin_unlock(&spin);
        return NULL;
    }
    ht->table = kmalloc(sizeof(struct key_value*) * TABLE_SIZE, GFP_KERNEL);
    if(ht->table == NULL){
        printk("kmalloc ht->table fail");
        kfree(ht);
        spin_unlock(&spin);
        return NULL;
    }
    memset(ht->table, 0, TABLE_SIZE * sizeof(struct key_value*));
    ht->free_value = free_value;
    spin_unlock(&spin);
    return ht;
}

void hash_table_delete(HashTable *ht){
    int i = 0;
    spin_lock(&spin);
    if(ht){
        if(ht->table){
            for(i = 0; i < TABLE_SIZE; i++){
                struct key_value *kv = ht->table[i];
                struct key_value *p = NULL;
                while(kv){
                    p = kv;
                    kv = kv->next;
                    if(ht->free_value){
                        ht->free_value(p->value);
                    }
                    kfree(p->key);
                    p->next = NULL;
                    kfree(p);
                }
            }
            kfree(ht->table);    
        }
        ht->free_value = NULL;
        ht->table = NULL;
        kfree(ht);
    }
    ht = NULL;
    spin_unlock(&spin);
}

void hash_table_rm(HashTable *ht, char *key){
    int hash;
    struct key_value *kv, *prep;
    spin_lock(&spin);
    hash = hash_function(key) % TABLE_SIZE;
    kv = ht->table[hash];
    prep = kv;
    while(kv){
        if(strcmp(key, kv->key) == 0){
            if(kv == prep){
                ht->table[hash] = kv->next;
            }else{
                prep->next = kv->next;    
            }
            if(ht->free_value){
                ht->free_value(kv->value);
            }
            kfree(kv->key);
            kv->next = NULL;
            kfree(kv);
            num--;
            printk("link table size -1 is %d\n", num);
            spin_unlock(&spin);
            return;
        }
        prep = kv;
        kv = kv->next;
    }
    spin_unlock(&spin);
}

void hash_table_update(HashTable *ht, bool(*func)(void*)){
    int i = 0;
    struct key_value *kv, *before, *after;
    spin_lock(&spin);
    if(ht){
        if(ht->table){
            for(i = 0; i < TABLE_SIZE; i++){
                kv = ht->table[i];
                before = after = kv;
                while(kv){
                    after = kv->next;
                    if(func(kv->value)){
                        if(kv == before){
                            ht->table[i] = after;
                        }else{
                            before->next = after;    
                        }
                        if(ht->free_value){
                            ht->free_value(kv->value);
                        }
                        kfree(kv->key);
                        kv->next = NULL;
                        kfree(kv);
                        num--;
                        printk("link table size -1 is %d\n", num);
                    }
                    else{
                        before = kv;
                    }
                    kv = after;
                }
            }
        }
    }
    spin_unlock(&spin);
}

void  hash_table_clear(HashTable *ht, bool(*func)(void*, int), int index){
    int i = 0;
    struct key_value *before, *kv, *after;
    spin_lock(&spin);
    if(ht){
        if(ht->table){
            for(i = 0; i < TABLE_SIZE; i++){
                kv = ht->table[i];
                before = after = kv;
                while(kv){
                    after = kv->next;
                    if(func(kv->value, index)){
                        if(kv == before){
                            ht->table[i] = after;
                        }else{
                            before->next = after;    
                        }
                        if(ht->free_value){
                            ht->free_value(kv->value);
                        }
                        kfree(kv->key);
                        kv->next = NULL;
                        kfree(kv);
                        num--;
                        printk("link table size -1 is %d\n", num);
                    }
                    else{
                        before = kv;
                    }
                    kv = after;
                }
            }
        }
    }
    spin_unlock(&spin);
}

int hash_table_put(HashTable *ht, char *key, void *value){
    int hash;
    struct key_value *kv, *prep;
    spin_lock(&spin);
    hash = hash_function(key) % TABLE_SIZE;
    kv = ht->table[hash];
    prep = kv;

    while(kv){
        if(strcmp(key, kv->key) == 0){
            if(ht->free_value){
                ht->free_value(kv->value);
            }
            kv->value = value;
            break;
        }
        prep = kv;
        kv = kv->next;
    }

    if(kv == NULL){
        struct key_value *p = kmalloc(sizeof(struct key_value), GFP_KERNEL);
        if(p == NULL){
            spin_unlock(&spin);
            return -1;
        }
        p->next = NULL;
        p->key = key;
        p->value = value;

        if(prep == NULL){
            ht->table[hash] = p;
        }
        else{
            prep->next = p;
        }
        num++;
        printk("link table size +1 is %d\n", num);
    }
    spin_unlock(&spin);
    return 0;
}

void *hash_table_get(HashTable *ht, char *key){
    int hash;
    struct key_value *kv;
    if(ht){
        if(ht->table){
            hash = hash_function(key) % TABLE_SIZE;
            kv = ht->table[hash];    
            while(kv){
                if(strcmp(key, kv->key) == 0){
                    return kv->value;
                }
                kv = kv->next;
            }
        }
    }
    return NULL;
}

char *hash_table_str(HashTable *ht, char*(*to_string)(void*)){
    int i = 0;
    char *str, *p, *q;
    struct key_value *kv;
    
    if(ht){
        if(ht->table){
            str = kmalloc(1024, GFP_ATOMIC);
            memset(str, 0, 1024);
            p = str;
            for(i = 0; i < TABLE_SIZE; i++){
                kv = ht->table[i];
                while(kv){
                    if(strlen(str) >= 1024){
                        return str;
                    }
                    if(to_string){
                        q = to_string(kv->value);
                        memcpy(p, q, strlen(q));
                        p += strlen(q);
                        *p = '\n';
                        p++;
                        kfree(q);
                    }
                    kv = kv->next;
                }
            }
            return str;
        }
    }
    return NULL;
}