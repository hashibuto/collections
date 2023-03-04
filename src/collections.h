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
    int key_offset;
    int value_offset;
};

extern struct HashMap *hashmap_new();
extern struct HashMap *hashmap_put(struct HashMap *hashmap, char *key, void *value, int size);
extern struct HashMap *hashmap_put_int(struct HashMap *hashmap, char *key, int value);
extern struct HashMap *hashmap_put_str(struct HashMap *hashmap, char *key, char *value);
extern void *hashmap_get(struct HashMap *hashmap, char *key);
extern void hashmap_del(struct HashMap *hashmap, char *key);
extern struct PtrLink *hashmap_iter(struct HashMap *hashmap);
extern struct HashMapEntry *hashmap_iter_get(struct PtrLink *iter);