#include <pcap.h>

struct store {
    int num_elems;
    int level;
    union store_array_elem* elems[256];
};

struct store_elem {
    int val;
    int num_uses;
    struct store_elem* next;
};

union store_array_elem {
    struct store store;
    struct store_elem store_elem;
};

struct store* store_init(void);
void store_update(struct store*, int);
void store_print(struct store*, int);
void store_close(struct store*);
