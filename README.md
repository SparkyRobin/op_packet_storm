# Coretech "Operation Packet Storm" Solution

## Installation
Clone this repository
```bash
git clone linklinklink
```
Install dependencies
```bash
sudo apt update && sudo apt upgrade
sudo apt install libpcap-dev
```
Compile program from inside the cloned repository
```bash
gcc main.c -o op_packet_storm ip_store.c transport_protocol_store.c -lpcap
```

## Usage
Call the compiled program with the filepath of the .pcap file you wish to examine.
```bash
./op_packet_storm ./packet-storm.pcap
```
