#include <stdio.h>
#include <stdlib.h>
#include "collections.h"

#define HASHMAP_DEFAULT_CAPACITY 16
#define HASHMAP_CAPACITY_GROW_THRESHOLD 0.75

// hashmap_new creates a new hashmap
struct HashMap *hashmap_new() {
    return _hashmap_new(HASHMAP_DEFAULT_CAPACITY);
}

struct HashMap *hashmap_put(struct HashMap *hashmap, char *key, void *value, int size) {
    struct HashMapEntry *hash_map_entry = _hashmap_new_item(key, value, size);
    return _hashmap_put(hashmap, hash_map_entry);
}

// hashmap_put_int is a convenience method for storing an integer value on the hashmap
struct HashMap *hashmap_put_int(struct HashMap *hashmap, char *key, int value) {
    return hashmap_put(hashmap, key, &value, sizeof(int));
}

// hashmap_put_str is a convenience function for storing a string value on the hashmap
struct HashMap *hashmap_put_str(struct HashMap *hashmap, char *key, char *value) {
    return hashmap_put(hashmap, key, &value, strlen(value)+1);
}

// hashmap_iter returns a hashmap iterator object.  the iterator can be safely used until the hashmap 
// is modified, at which point the iterator is considered unsafe or inaccurate.
struct HashMapIter *hashmap_iter(struct HashMap *hashmap) {
    struct HashMapIter *hashmap_iter = malloc(sizeof(struct HashMapIter));
    hashmap_iter->hashmap = hashmap;
    hashmap_iter->cur_entry = NULL;
    hashmap_iter->cur_index = 0;
}

// hashmap_iter_next returns NULL if we've reached the end of iteration, or a HashMapEntry.
struct HashMapEntry *hashmap_iter_next(struct HashMapIter *iter) {
    if (iter->cur_entry == NULL) {
        while (iter->cur_index < iter->hashmap->capacity && iter->hashmap->items[iter->cur_index] == NULL) {
            // Move along the hashmap until we get to the end, or we hit a filled entry
            iter->cur_index++;
        }
        if (iter->cur_index >= iter->hashmap->capacity) {
            // We've reached the end
            return NULL;
        }

        iter->cur_entry = iter->hashmap->items[iter->cur_index];
    }

    struct HashMapEntry *ret = iter->cur_entry;

    // Advance to the next position
    iter->cur_entry = iter->cur_entry->next;
    if (iter->cur_entry == NULL) {
        iter->cur_index++;
    }

    return ret;
}

// _hashmap_new_item allocates memory for a single item to be stored in the hash map.
struct HashMapEntry *_hashmap_new_item(char *key, void *value, int value_size) {
    unsigned long long hash = _hash(key);
    int key_offset = sizeof(struct HashMapEntry);
    int key_size = strlen(key) + 1;
    int value_offset = key_offset + key_size;
    void *allocation = malloc(value_offset + value_size);
    struct HashMapEntry *hashMapEntry = allocation;
    hashMapEntry->prev = NULL;
    hashMapEntry->next = NULL;
    hashMapEntry->key_offset = key_offset;
    hashMapEntry->value_offset = value_offset;
    hashMapEntry->hash = hash;
    memcpy(allocation + key_offset, key, key_size);
    memcpy(allocation + value_offset, value, value_size); 
}

struct HashMap *_hashmap_put(struct HashMap *hashmap, struct HashMapEntry *hashmap_entry) {
    if ((float)hashmap->size + 1 / (float)hashmap->capacity >= HASHMAP_CAPACITY_GROW_THRESHOLD) {
        struct HashMap *new_hashmap = hashmap_new(hashmap->capacity * 2);
        struct HashMapIter *iter = hashmap_iter(hashmap);
        
        struct HashMapEntry *next;
        while((next = hashmap_iter_next(iter)) != NULL) {
            _hashmap_put(new_hashmap, next);
        }
    }
    
    int offset = hashmap_entry->hash % hashmap->capacity;
    if (hashmap->items[offset] == NULL) {
        // Drop the new entry there
        hashmap->items[offset] = hashmap_entry;
    } else {
        struct HashMapEntry *cur_ent = hashmap->items[offset];
        while (cur_ent->next != NULL) {
            cur_ent = cur_ent->next;
        }
        cur_ent->next = hashmap_entry;
        hashmap_entry->prev = cur_ent;
    }

    struct PtrLink *link = malloc(sizeof(struct PtrLink));
    link->next = NULL;
    link->item = hashmap_entry;

    if (hashmap->head == NULL) {
        link->prev = NULL;
        hashmap->head = link;
        hashmap->tail = link;
    } else {
        hashmap->tail->next = link;
        hashmap->tail = link;
    }
}

// _hashmap_new creates a new hashmap
struct HashMap *_hashmap_new(int capacity) {
    struct HashMap *hashmap = malloc(sizeof(struct HashMap));
    hashmap->items = calloc(capacity, sizeof(struct HashMapEntry *));

    hashmap->size = 0;
    hashmap->capacity = capacity;
    hashmap->head = NULL;
    hashmap->tail = NULL;

    return hashmap;
}

// _hash returns the hashed value of the provided key
unsigned long long _hash(char *key) {
    const int p = 97;
    const int m = 1e9 + 9;
    unsigned long long p_pow = 1;
    unsigned long long int hash = 0;

    for (int i = 0; i < strlen(key); i++) {
        hash = (hash + (key[i] - '!' + 1) * p_pow) % m;
        p_pow = (p_pow * p) % m;
    }

    return hash;
}