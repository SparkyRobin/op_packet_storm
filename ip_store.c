#include "ip_store.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct store* store_init(void) {
    struct store* store = malloc(sizeof(struct store));
    store->num_elems = 0;
    store->level = 0;
    memset(&store->elems, 0, 256 * sizeof(union store_array_elem*));
    return store;
}

void store_update(struct store* store, int val) {
    int offset = (val >> (8 * store->level)) & 0xFF;
    if (store->elems[offset] != NULL) {
        if (store->level == 3) {
            store->elems[offset]->store_elem.num_uses += 1;
        } else {
            store_update(&(store->elems[offset])->store, val);
        }
    } else {
        store->elems[offset] = malloc(sizeof(union store_array_elem));
        if (store->level == 3) {
            store->elems[offset]->store_elem.val = val;
            store->elems[offset]->store_elem.num_uses = 1;
            store->elems[offset]->store_elem.next = NULL;
            store->num_elems += 1;
        } else {
            store->elems[offset]->store.level = store->level + 1;
            store->elems[offset]->store.num_elems = 0;
            memset(&store->elems[offset]->store.elems, 0, 256 * sizeof(union store_array_elem*));
            store->num_elems += 1;
            store_update(&(store->elems[offset])->store, val);
        }
    }
}

void store_print_elem(struct store_elem* elem) {
    /*switch (option) {
        case 0:
            printf("%03d.%03d.%03d.%03d", elem->val & 0x000000FF, (elem->val & 0x0000FF00) >> 8, (elem->val & 0x00FF0000) >> 16, (elem->val & 0xFF000000) >> 24);
            break;
        case 1:
            printf("%s", (elem->val == IPPROTO_TCP) ? "TCP" : ((elem->val == IPPROTO_UDP) ? "UDP" : "other"));
            break;
        default:
            printf("%x", elem->val);
            break;
    }*/
    printf("\t%03d.%03d.%03d.%03d", elem->val & 0x000000FF, (elem->val & 0x0000FF00) >> 8, (elem->val & 0x00FF0000) >> 16, (elem->val & 0xFF000000) >> 24);
    printf("  %d\n", elem->num_uses);

}

void store_print_all(struct store* store) {
    if (store->level == 3){
        for (int i = 0; i < 256; i++) {
            union store_array_elem* point = store->elems[i];
            if (point != NULL) store_print_elem(&point->store_elem);
        }
    } else {
        for (int i = 0; i < 256; i++) {
            union store_array_elem* point = store->elems[i];
            if (point != NULL) store_print_all(&point->store);
        }
    }
}

void store_print_count(struct store* store, int count, struct store_elem* head, int* min_uses, int* used) {
    if (store->level == 3){
        for (int i = 0; i < 256; i++) {
            union store_array_elem* point = store->elems[i];
            if (point != NULL) {
                if (point->store_elem.num_uses > *min_uses || *used < count) {
                    struct store_elem* elem_prev = head;
                    struct store_elem* elem_next = head->next;
                    for (int j = 0; j < count; j++) {
                        if (elem_next == NULL) {
                            elem_prev->next = &point->store_elem;
                            if (j == count - 1) *min_uses = point->store_elem.num_uses;
                            break;
                        } else if (point->store_elem.num_uses > elem_next->num_uses) {
                            elem_prev->next = &point->store_elem;
                            point->store_elem.next = elem_next;
                            if (j == count - 1) *min_uses = point->store_elem.num_uses;
                            break;
                        } else {
                            elem_prev = elem_next;
                            elem_next = elem_prev->next;
                        }
                    }
                    *used++;
                }
            }
        }
    } else {
        for (int i = 0; i < 256; i++) {
            union store_array_elem* point = store->elems[i];
            if (point != NULL) store_print_count(&point->store, count, head, min_uses, used);
        }
    }
}

void store_print(struct store* store, int count) {
    if (count <= 0) {
        printf("All destination ip addresses with number of packets sent\n");
        store_print_all(store);
    } else {
        printf("first %d destination ip addresses with number of packets sent\n", count);
        struct store_elem head = {-1, -1, NULL};
        int min_uses = 0;
        int full = 0;
        store_print_count(store, count, &head, &min_uses, &full);
        struct store_elem* curr = head.next;
        for (int i = 0; i < count; i++) {
            if (curr == NULL) break;
            store_print_elem(curr);
            curr = curr->next;
        }
    }
}

void store_close(struct store* store) {
    if (store->level == 3){
        for (int i = 0; i < 256; i++) {
            union store_array_elem* point = store->elems[i];
            if (point != NULL) free(&point->store_elem);
        }
    } else {
        for (int i = 0; i < 256; i++) {
            union store_array_elem* point = store->elems[i];
            if (point != NULL) store_close(&point->store);
        }
    }

    free(store);
}
