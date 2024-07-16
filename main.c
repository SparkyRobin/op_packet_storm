#include <stdio.h>
#include "ip_store.h"
#include "transport_protocol_store.h"

#define ETHERNET_HEADER_LENGTH 14

struct out_data {
    int tot_num;
    int tot_len;
};

void analyse(pcap_t* handle, struct out_data* info, struct store* ip_store, transport_store transport_store) {
    const u_char* packet;
    struct pcap_pkthdr packet_header;

    packet = pcap_next(handle, &packet_header);
    while (packet != NULL) {
        info->tot_num += 1;
        info->tot_len += packet_header.caplen;
        store_update(ip_store, *((long unsigned int*) (packet + ETHERNET_HEADER_LENGTH + 16)));
        transport_add(transport_store, *(packet + ETHERNET_HEADER_LENGTH + 9));
        packet = pcap_next(handle, &packet_header);
    }
}

int main(int argv, char** argc) {
    if (argv < 2) printf("please provide a filepath");
    struct out_data info;
    info.tot_num = 0;
    info.tot_len = 0;
    struct store* ip_store = store_init();
    transport_store transport_store = {0};
    char error_buffer[PCAP_ERRBUF_SIZE];
    pcap_t *handle = pcap_open_offline(argc[1], error_buffer);
    if (handle == NULL) {
        printf("%s is not a valid .pcap file", argc[1]);
        return 1;
    }
    analyse(handle, &info, ip_store, transport_store);
    printf("average packet size: %d B\ntotal volume: %d B\n\ndestination ips:\n", info.tot_len/info.tot_num, info.tot_len);
    store_print(ip_store, 10);
    transport_print(transport_store);

    while(1) {
        printf("\n\nsee more destination ip addresses(number of ips or 0 for all), or quit(-1)\n");
        int choice = 0;
        scanf("%d", &choice);
        printf("\n");

        if (choice >= 0) {
            store_print(ip_store, choice);
        } else {
            store_close(ip_store);
            return 0;
        }
    }
}
