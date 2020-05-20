/*******************************************
  - Non Blocking recvfrom with DONWAITFLAG
  - 2017.04.19 by ywcha 
  - tcp와 icmp 메시지를 수신 시에 ip 헤더를 출력하는 프로그램 
********************************************/
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
   int sock_tcp, sock_icmp, ret_111, ret_222, len, buf_len;
   char buffer_111[65535], buffer_222[65535];
   struct sockaddr_in from;
   struct ifreq ifr;
   char *interface = "enp2s0"; //시스템 마다 변경되어야 할 인터페이스 
   struct ip_header_t *ip;

   sock_tcp = socket(PF_INET, SOCK_RAW, IPPROTO_TCP);
   sock_icmp = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP);

   strncpy(ifr.ifr_name, interface, strlen(interface));

   if(ioctl(sock_tcp, SIOCGIFFLAGS, &ifr) < 0) {
      perror("SIOCGIFFLAGS error : ");
      exit(1);
   }

   ifr.ifr_flags |= IFF_PROMISC;

   if (ioctl(sock_tcp, SIOCSIFFLAGS, &ifr) < 0) {
      perror("Could not set the PROMISC flag : ");
      exit(1);
   }
  //인터페이스가 프로미스큐어스로 설정되었으므로 sock_icmp는 하지 않아도 됨 

   len = sizeof(from);
   buf_len = sizeof(buffer_111); 

   while(1) {
      ret_111 = recvfrom(sock_tcp, buffer_111, buf_len, MSG_DONTWAIT, (struct sockaddr *)&from, &len);
      ret_222 = recvfrom(sock_icmp, buffer_222, buf_len, MSG_DONTWAIT, (struct sockaddr *)&from, &len);

      if(ret_111 > 0) {
        printf("rx data length from tcp_socket = %d\n", ret_111);
   	struct ip_header_t *ip = (struct ip_header_t *)&buffer_111;

      	if ((ip->protocol == PROTO_TCP) || (ip->protocol==PROTO_ICMP))
  		ip_header_print(ip); 
      }

      if(ret_222 > 0) {
        printf("rx data length from icmp_socket = %d\n", ret_222);
   	struct ip_header_t *ip = (struct ip_header_t *)&buffer_222;
      	if ((ip->protocol == PROTO_TCP) || (ip->protocol==PROTO_ICMP))
  		ip_header_print(ip); 
      }

      if((ret_111 < 0) && (ret_222 <0)) 
        printf("######################################## \n\n\n");

      sleep(5); 
   }

   return 0;
}

void ip_header_print(struct ip_header_t *ip)
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

