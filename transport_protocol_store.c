#include "transport_protocol_store.h"
#include <pcap.h>
#include <stdio.h>

void transport_add(transport_store store, int i) {
    store[i] += 1;
}

void transport_print(transport_store store) {
    printf("\ntransport protocols:\n");
    for (int i = 0; i < 256; i++) {
        if (store[i] != 0) {
            printf("\t%s  %d\n", getprotobynumber(i)->p_name, store[i]);
        }
    }
}
