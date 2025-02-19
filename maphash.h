#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef unsigned long long Hash;
#define HashNodeMaxKeyLength 50
#define HashTableSize 2

typedef enum {
    StringHashTabel,
    IntHashTable,
    BooleanHashTable
} HashTableDatatype;

typedef struct HashNode{
    char key[HashNodeMaxKeyLength];
    void* value;
    struct HashNode* next;
} HashNode;


typedef struct HashTable{
    HashNode* cell[HashTableSize];
    HashTableDatatype type;
    bool (*insert)(struct HashTable*, char*, void*);
    void* (*get)(struct HashTable*, char*);
} HashTable;

Hash djb2_hash(const char *str) {
    Hash hash = 23;
    while (*str) {
        hash = ((hash << 5) + hash) + *str;
        str++;
    }
    return hash;
}

bool _hashtable_insert(HashTable* hashTable, char* key, void* _v){
    if (hashTable->type == StringHashTabel) {
        char* value = (char*)_v;
        Hash hash = djb2_hash((const char*)key) % HashTableSize;
        HashNode** cell = hashTable->cell + hash;
        if (*cell == NULL) { // no exit node it this cell yet
            *cell = (HashNode* )malloc(sizeof(HashNode)); // create new node
            strcpy((**cell).key, key);
            (*cell)->value = value;
            (*cell)->next = NULL;
            return true;
        }
        else {
            HashNode* prev = *cell;
            while (true) {
                if (strcmp(prev->key, key) == 0) {
                    prev->value = value;
                    return true;
                }
                if (prev->next != NULL) prev = prev->next;
                else break;
            }
            HashNode* newnode = (HashNode*)malloc(sizeof(HashNode)); // create new node
            // Join newnode at the end of link list
            prev->next = newnode;
            strcpy(newnode->key, key);
            newnode->value = value;
            return true;
        }
    }

    return false;
}

void* _hashtable_get(HashTable* hashtable, char* key) {
    if (hashtable->type == StringHashTabel) {
        Hash hash = djb2_hash((const char*)key) % HashTableSize;
        HashNode** cell = hashtable->cell + hash;
        if (*cell == NULL) return NULL;
        else {
            while (true) {
                if (strcmp((**cell).key, key) == 0) {
                    return (**cell).value;
                }
                if ((**cell).next != NULL) *cell = (**cell).next;
                else {
                    break;
                }
            }
            return NULL;
        }
    }
    return NULL;
}

void* _hashtable_delete(HashTable* hashtable, char* key) {
    if (hashtable->type == StringHashTabel) {
        Hash hash = djb2_hash((const char*)key) % HashTableSize;
        HashNode** cell = hashtable->cell + hash;
        if (*cell == NULL) return NULL;
        else {
            HashNode* prev = NULL;
            while (true) {
                if (strcmp((**cell).key, key) == 0) {
                    void* value = (**cell).value;

                    // Free HashNode
                    free(*cell);
                    *cell = NULL;
                    return value;
                }
                if ((**cell).next != NULL) {

                    prev = *cell; 
                    *cell = (**cell).next;
                }
                else {
                    break;
                }
            }
            return NULL;
        }
    }
    return NULL;
}

HashTable* createHashTable(HashTableDatatype dataType) {

    HashTable* hashTable = (HashTable*)malloc(sizeof(HashTable));
    hashTable->type = dataType;
    // contruct method
    hashTable->insert = _hashtable_insert;
    hashTable->get = _hashtable_get;

    for (int i = 0; i < HashTableSize; i++) {
        hashTable->cell[i] = NULL;
    }

    return hashTable;
}

bool deleteHashTable(HashTable* hashTable) {
    if (hashTable == NULL) return false;
    for (int i = 0; i < HashTableSize; i++) {
        if (hashTable->cell[i] != NULL){
            free(hashTable->cell[i]);
        }
    }
    free(hashTable);
    hashTable = NULL;
    return true;
}
