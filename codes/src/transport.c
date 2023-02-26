#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "net.h"
#include "transport.h"

uint16_t cal_cksm(unsigned char *buf, int len) {
    uint32_t sum = 0;
    uint16_t *ptr = (uint16_t *) buf;

    while (len > 1) {
        sum += *ptr++;
        len -= 2;
    }

    if (len == 1) {
        sum += *(unsigned char *)ptr;
    }

    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);

    return (uint16_t) (~sum);
}

uint16_t cal_tcp_cksm(struct iphdr iphdr, struct tcphdr tcphdr, uint8_t *pl, int plen)
{
    // [TODO]: Finish TCP checksum calculation
    uint16_t tcp_len = ntohs(iphdr.tot_len) - iphdr.ihl*4;
    unsigned char *buf = (unsigned char *) malloc(sizeof(struct pseudo_header) + tcp_len);
    memset(buf, 0, sizeof(struct pseudo_header) + tcp_len);

    struct pseudo_header *ph = (struct pseudo_header *) buf;
    ph->src_addr = iphdr.saddr;
    ph->dst_addr = iphdr.daddr;
    ph->reserved = 0;
    ph->protocol = IPPROTO_TCP;
    ph->tcp_length = htons(tcp_len);
    memcpy(buf + sizeof(struct pseudo_header), tcphdr, tcp_len);

    uint16_t tcp_cksm= cal_cksm(buf, sizeof(struct pseudo_header) + tcp_len);
    free(buf);
    return tcp_cksm;
}

uint8_t *dissect_tcp(Net *net, Txp *self, uint8_t *segm, size_t segm_len)
{
    // [TODO]: Collect information from segm
    // (Check IP addr & port to determine the next seq and ack value)
    // Return payload of TCP
}

Txp *fmt_tcp_rep(Txp *self, struct iphdr iphdr, uint8_t *data, size_t dlen)
{
    // [TODO]: Fill up self->tcphdr (prepare to send)

    return self;
}

inline void init_txp(Txp *self)
{
    self->pl = (uint8_t *)malloc(IP_MAXPACKET * sizeof(uint8_t));
    self->hdrlen = sizeof(struct tcphdr);

    self->dissect = dissect_tcp;
    self->fmt_rep = fmt_tcp_rep;
}

