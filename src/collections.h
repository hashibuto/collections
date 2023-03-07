#ifndef COLLECTIONS_H
#define COLLECTIONS_H

#define HASHMAP_DEFAULT_CAPACITY 16
#define HASHMAP_CAPACITY_GROW_THRESHOLD 0.75

struct PtrLink {
    struct PtrLink *next;
    struct PtrLink *prev;
    void *item;
};

struct HashMap {
    void **items;
    unsigned int size;
    unsigned int capacity;
    struct PtrLink *head;
    struct PtrLink *tail;
};

struct HashMapEntry {
    struct HashMapEntry *prev;
    struct HashMapEntry *next;
    struct PtrLink *link;
    unsigned long long hash;
    char *key;
    void *value;
};

extern struct HashMap *hashmap_new();
extern void hashmap_put(struct HashMap *hashmap, char *key, void *value, int size);
extern void hashmap_put_int(struct HashMap *hashmap, char *key, int value);
extern void hashmap_put_str(struct HashMap *hashmap, char *key, char *value);
extern void *hashmap_get(struct HashMap *hashmap, char *key);
extern void hashmap_del(struct HashMap *hashmap, char *key);
extern struct PtrLink *hashmap_iter(struct HashMap *hashmap);
extern struct HashMapEntry *hashmap_iter_get(struct PtrLink *iter);
extern void hashmap_free(struct HashMap *hashmap);
extern void hashmap_optimize(struct HashMap *map);

#endif /* COLLECTIONS_H */