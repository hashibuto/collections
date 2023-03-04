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

// HashMapEntry gets overlayed onto a block of memory allocated by alloc to accomodate the entirety of the data, which includes 
// the HashMapEntry itselt at the start, as well as the hash (64 bits), followed by the key (size known at allocation time), followed by the
// value, of which the size is not stored.
struct HashMapEntry {
    struct HashMapEntry *prev;
    struct HashMapEntry *next;
    unsigned long long hash;
    int key_offset;
    int value_offset;
};

struct HashMapIter {
    struct HashMap *hashmap;
    unsigned int cur_index;
    struct HashMapEntry *cur_entry;
};