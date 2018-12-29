#include "lineTable.h"

#define TABLE_SIZE 1024

static DEFINE_SPINLOCK(spin);

LineTable *line_table_new(bool(*comp_value)(void *, void *), void(*free_value)(void*), bool(*search_value)(void*, void*, unsigned int)) {
	LineTable *lt;
	spin_lock(&spin);
	lt = kmalloc(sizeof(LineTable), GFP_KERNEL);
	if(lt == NULL){
		spin_unlock(&spin);
		return NULL;
	}
	lt->table = kmalloc(sizeof(void *) * TABLE_SIZE, GFP_KERNEL);
	if(lt->table == NULL){
		kfree(lt);
		spin_unlock(&spin);
		return NULL;
	}
	memset(lt->table, 0, sizeof(void *) * TABLE_SIZE);
	lt->comp_value = comp_value;
	lt->free_value = free_value;
	lt->search_value = search_value;
	lt->size = 0;
	spin_unlock(&spin);
	return lt;
}

void line_table_delete(LineTable* lt){
	int i = 0;
	spin_lock(&spin);
	if(lt){
		if(lt->table){
			for(i = 0; i < TABLE_SIZE; i++){
				if((lt->table[i]) && (lt->free_value)){
					lt->free_value(lt->table[i]);
				}
			}
			kfree(lt->table);
			lt->table = NULL;
			lt->comp_value = NULL;
			lt->free_value = NULL;
			lt->search_value = NULL;
			lt->size = 0;
		}
		kfree(lt);
	}
	lt = NULL;
	spin_unlock(&spin);
}

int line_table_put(LineTable *lt, void *value){
	int i = 0;
	spin_lock(&spin);
	for(i = 0; i < lt->size; i++){
		if(lt->comp_value(value, lt->table[i])){
			spin_unlock(&spin);
			return 0;
		}
	}
	if(lt->size >= TABLE_SIZE){
		spin_unlock(&spin);
		return -1;
	}
	lt->table[lt->size] = value;
	lt->size++;
	printk("lineTable size +1 is %d\n", lt->size);
	spin_unlock(&spin);
	return 0;
}

int line_table_search(LineTable *lt, void *value, unsigned int hooknum){
	int i = 0;
	for(i = 0; i < lt->size; i++){
		if(lt->search_value(value, lt->table[i], hooknum)){
			return i;
		}
	}
	return -1;
} 

int line_table_get(LineTable *lt, void *value){
	int i = 0;
	for(i = 0; i < lt->size; i++){
		if(lt->comp_value(value, lt->table[i])){
			return i;
		}
	}
	return -1;
}

int line_table_rm(LineTable *lt, int index){
	int i = 0;
	spin_lock(&spin);
	if((index < 0) || (index >= lt->size)){
		printk("line table rm failed");
		spin_unlock(&spin);
		return -1;
	}
	lt->free_value(lt->table[index]);
	for(i = index + 1; i < lt->size; i++){
		lt->table[i - 1] = lt->table[i];
	}
	lt->table[lt->size - 1] = NULL;
	lt->size--;
	printk("lineTable size -1 is %d\n", lt->size);
	spin_unlock(&spin);
	return 0;
}

int line_table_clear(LineTable *lt, bool(*func)(void*, int), int index){
	int i = 0, j = 0;
	spin_lock(&spin);
	if(lt){
		if(lt->table){
			for(i = 0; i < lt->size; ){
				if(func(lt->table[i], index)){
					lt->free_value(lt->table[i]);
					for(j = i + 1; j < lt->size; j++){
						lt->table[j - 1] = lt->table[j];
					}
					lt->table[lt->size - 1] = NULL;
					lt->size--;
					printk("line table size -1 is %d\n", lt->size);
				}
				else{
					i++;
				}
			}
		}
	}
	spin_unlock(&spin);
	return 0;
}