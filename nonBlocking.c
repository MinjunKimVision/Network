#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "packet.h"

void ip_header_print(struct ip_header_t* ip);

int main() {
    int sock_tcp, sock_icmp, sock_udp, ret_111, ret_222, ret_333, len, buf_len;
    char buffer_111[65535], buffer_222[65535], buffer_333[65535];
    struct sockaddr_in from;
    struct ifreq ifr;
    char* interface = "virbr0"; // 현재 인터넷의 인터페이스를 선택
    struct ip_header_t* ip;

    sock_tcp = socket(PF_INET, SOCK_RAW, IPPROTO_TCP);
    sock_icmp = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP);
    sock_udp = socket(PF_INET, SOCK_RAW, IPPROTO_UDP);
    strncpy(ifr.ifr_name, interface, strlen(interface));

    //ifr.ifr_flags |=IFF_PROMISC;
    if (ioctl(sock_tcp, SIOCGIFFLAGS, &ifr) < 0) {
        perror("SIOCGIFFLAGS error : ");
        exit(1);
    }

    ifr.ifr_flags |= IFF_PROMISC;// 모든주소 받기

    if (ioctl(sock_tcp, SIOCSIFFLAGS, &ifr) < 0) {
        perror("Could not set the PROMISC flag : ");
        exit(1);
    }


    len = sizeof(from);
    buf_len = sizeof(buffer_111);

    while (1) {
        ret_111 = recvfrom(sock_tcp, buffer_111, buf_len, MSG_DONTWAIT, (struct sockaddr*) & from, &len);
        ret_222 = recvfrom(sock_icmp, buffer_222, buf_len, MSG_DONTWAIT, (struct sockaddr*) & from, &len);
        ret_333 = recvfrom(sock_udp, buffer_333, buf_len, MSG_DONTWAIT, (struct sockaddr*) & from, &len);

        if (ret_111 > 0) {
            printf("rx data length from tcp_socket = %d\n", ret_111);
            struct ip_header_t* ip = (struct ip_header_t*) & buffer_111;
            if ((ip->protocol == PROTO_TCP) || (ip->protocol == PROTO_ICMP)) {
                ip_header_print(ip);
            }
        }

        if (ret_222 > 0) {
            printf("rx data length from icmp_socket = %d\n", ret_222);
            struct ip_header_t* ip = (struct ip_header_t*) & buffer_222;
            if ((ip->protocol == PROTO_TCP) || (ip->protocol == PROTO_ICMP)) {
                ip_header_print(ip);
            }
        }
        if (ret_333 > 0) {
            printf("rx data length from udp_socket = %d\n", ret_333);
            struct ip_header_t* ip = (struct ip_header_t*) & buffer_222;
            if ((ip->protocol == PROTO_UDP)) {
                ip_header_print(ip);
            }
        }

        if ((ret_111 < 0) && (ret_222 < 0) && (ret_333 < 0)) {
            memset(buffer_111, 0x00, 65535);
            memset(buffer_222, 0x00, 65535);
            memset(buffer_333, 0x00, 65535);
            printf("######################################## \n\n\n");
            sleep(2);
        }

    }

    return 0;
}

void ip_header_print(struct ip_header_t* ip)
{
    struct in_addr src, dst;
    printf("================ IP header ===============\n");
    src.s_addr = ip->src_ip;
    dst.s_addr = ip->dst_ip;

    printf("Src ip = %s\n", inet_ntoa(src));
    printf("Dst ip = %s\n", inet_ntoa(dst));
    printf("Total length = %d\n", ntohs(ip->length));
    printf("Identification = %d\n", ntohs(ip->id));
    printf("TTL = %d\n", ip->ttl);
    printf("PROTOCOL = %d\n", ip->protocol);
    printf("Checksum = 0x%X\n", ntohs(ip->checksum));

    printf("\n\n\n");
}
/******************   eof  ***********************/
