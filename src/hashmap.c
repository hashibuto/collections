#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "collections.h"

struct HashMap *_hashmap_new(int capacity);
void _hashmap_put(struct HashMap *hashmap, struct HashMapEntry *hashmap_entry);
struct HashMapEntry *_hashmap_new_item(char *key, void *value, int value_size);
void *_hashmap_get(struct HashMap *hashmap, char *key, unsigned long long hash);
void _hashmap_optimize(struct HashMap *map, int size, int do_shrink);
void _hashmap_insert_in_hashmap(struct HashMap *map, struct HashMapEntry *hashmap_entry);
unsigned long long _hash(char *key);

// hashmap_new creates a new hashmap
struct HashMap *hashmap_new() {
    return _hashmap_new(HASHMAP_DEFAULT_CAPACITY);
}

void hashmap_put(struct HashMap *hashmap, char *key, void *value, int size) {
    struct HashMapEntry *hash_map_entry = _hashmap_new_item(key, value, size);
    _hashmap_put(hashmap, hash_map_entry);
}

// hashmap_put_int is a convenience method for storing an integer value on the hashmap
void hashmap_put_int(struct HashMap *hashmap, char *key, int value) {
    hashmap_put(hashmap, key, &value, sizeof(int));
}

// hashmap_put_str is a convenience function for storing a string value on the hashmap
void hashmap_put_str(struct HashMap *hashmap, char *key, char *value) {
    hashmap_put(hashmap, key, value, strlen(value)+1);
}

// hashmap_get returns NULL if the key cannot be located, otherwise a value pointer
void *hashmap_get(struct HashMap *hashmap, char *key) {
    unsigned long long hash = _hash(key);
    struct HashMapEntry *ent = _hashmap_get(hashmap, key, hash);
    if (ent == NULL) {
        return NULL;
    }
    return ent->value;
}

// hashmap_del removes an item from the map or does nothing if the key doesn't exist
void hashmap_del(struct HashMap *hashmap, char *key) {
    unsigned long long hash = _hash(key);
    struct HashMapEntry *ent = _hashmap_get(hashmap, key, hash);
    if (ent == NULL) {
        return;
    }

    int offset = hash % hashmap->capacity;
    if (ent->prev == NULL) {
        if (ent->next == NULL) {
            hashmap->items[offset] = NULL;
        } else {
            ent->next->prev = NULL;
            hashmap->items[offset] = ent->next;
        }
    } else {
        if (ent->next == NULL) {
            ent->prev->next = NULL;
        } else {
            ent->prev->next = ent->next;
        }
    }

    struct PtrLink *link = ent->link;
    if (link->prev == NULL) {
        if (link->next == NULL) {
            // First and last link
            hashmap->head = NULL;
            hashmap->tail = NULL;
        } else {
            // First link but not last
            hashmap->head = link->next;
            link->next->prev = NULL;
        }
    } else {
        if (link->next == NULL) {
            // Last link
            hashmap->tail = link->prev;
            link->prev->next = NULL;
        } else {
            // Inner link
            link->prev->next = link->next;
            link->next->prev = link->prev;
        }
    }
    hashmap->size--;

    free(link);
    free(ent);
}

void hashmap_free(struct HashMap *hashmap) {
    struct PtrLink *iter = hashmap_iter(hashmap);
    while (iter != NULL) {
        struct PtrLink *next_iter = iter->next;
        free(iter->item);
        free(iter);
        iter = next_iter;
    }
    free(hashmap->items);
    free(hashmap);
}

// hashmap_optimize will attempt to optimize the provided hashmap by either expanding or collapsing it.  If the map is already optimally sized
// no penalty will be incurred.
void hashmap_optimize(struct HashMap *map) {
    _hashmap_optimize(map, map->size, 1);
}

void _hashmap_optimize(struct HashMap *map, int size, int do_shrink) {
    int new_capacity = 0;
    float grow_thresh = (float)size / (float)map->capacity;
    if (grow_thresh >= HASHMAP_CAPACITY_GROW_THRESHOLD) {
        new_capacity = map->capacity * 2;
    } else if (do_shrink && ((float)size / (float)(map->capacity / 2)) < HASHMAP_CAPACITY_GROW_THRESHOLD && map->capacity > HASHMAP_DEFAULT_CAPACITY) {
        new_capacity = map->capacity / 2;
        while(((float)size / (float)(new_capacity / 2)) < HASHMAP_CAPACITY_GROW_THRESHOLD && new_capacity > HASHMAP_DEFAULT_CAPACITY) {
            new_capacity /= 2;
        }
    }

    if (new_capacity != 0) {
        free(map->items);
        map->items = calloc(new_capacity, sizeof(void *));
        map->capacity = new_capacity;

        struct PtrLink *iter = hashmap_iter(map);
        while (iter != NULL) {
            struct HashMapEntry *hme = iter->item;
            // Reset these since the linked list is changing
            hme->prev = NULL;
            hme->next = NULL;
            _hashmap_insert_in_hashmap(map, hme);
            iter = iter->next;
        }
    }
}

// hashmap_iter returns a linked list link entry which can be used to traverse the map, using the ->next
// pointer.  the terminal point of the list will have a ->next entry of NULL.
struct PtrLink *hashmap_iter(struct HashMap *hashmap) {
    return hashmap->head;
}

// hashmap_iter_get returns the HashMapEntry pointer from the current iter value.
struct HashMapEntry *hashmap_iter_get(struct PtrLink *iter) {
    return iter->item;
}

// _hashmap_get returns NULL if the key cannot be located, otherwise a HashMapEntry pointer
void *_hashmap_get(struct HashMap *hashmap, char *key, unsigned long long hash) {
    int offset = hash % hashmap->capacity;

    struct HashMapEntry *cur_ent = hashmap->items[offset];
    while (cur_ent != NULL) {
        if (cur_ent->hash == hash && (strcmp(cur_ent->key, key) == 0)) {
            return cur_ent;
        }
        cur_ent = cur_ent->next;
    }

    return NULL;
}


// _hashmap_new_item allocates memory for a single item to be stored in the hash map.
struct HashMapEntry *_hashmap_new_item(char *key, void *value, int value_size) {
    unsigned long long hash = _hash(key);
    int key_offset = sizeof(struct HashMapEntry);
    int key_size = strlen(key) + 1;
    int value_offset = key_offset + key_size;
    struct HashMapEntry *hashMapEntry = malloc(value_offset + value_size);
    hashMapEntry->prev = NULL;
    hashMapEntry->next = NULL;
    hashMapEntry->link = NULL;
    hashMapEntry->key = (char *) (((void*)hashMapEntry) + key_offset);
    hashMapEntry->value = (void *) (((void *)hashMapEntry) + value_offset);
    hashMapEntry->hash = hash;
    memcpy(hashMapEntry->key, key, key_size);
    memcpy(hashMapEntry->value, value, value_size);
    return hashMapEntry;
}

void _hashmap_put(struct HashMap *map, struct HashMapEntry *hashmap_entry) {
    _hashmap_optimize(map, map->size+1, 0);
    _hashmap_insert_in_hashmap(map, hashmap_entry);

    struct PtrLink *link = malloc(sizeof(struct PtrLink));
    link->next = NULL;
    link->item = hashmap_entry;
    hashmap_entry->link = link;

    if (map->head == NULL) {
        link->prev = NULL;
        map->head = link;
        map->tail = link;
    } else {
        map->tail->next = link;
        link->prev = map->tail;
        map->tail = link;
    }
    map->size++;
}

void _hashmap_insert_in_hashmap(struct HashMap *map, struct HashMapEntry *hashmap_entry) {
    int offset = hashmap_entry->hash % map->capacity;
    struct HashMapEntry *cur_ent = map->items[offset];
    if (cur_ent == NULL) {
        // Drop the new entry there
        map->items[offset] = hashmap_entry;
    } else {
        while (cur_ent->next != NULL) {
            cur_ent = cur_ent->next;
        }
        cur_ent->next = hashmap_entry;
        hashmap_entry->prev = cur_ent;
    }
}

// _hashmap_new creates a new hashmap
struct HashMap *_hashmap_new(int capacity) {
    struct HashMap *hashmap = malloc(sizeof(struct HashMap));
    hashmap->items = calloc(capacity, sizeof(void *));
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
