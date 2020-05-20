//=================================================
//	gcc -o myping myping.c 
//	./myping 220.69.240.134  
//=================================================

#include <stdlib.h>
#include <string.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <netdb.h>

int in_cksum(u_short *p, int n);

int main(int argc, char **argv)
{
   int icmp_socket;
   int ret;
   struct hostent *HostName;
   struct icmp *p, *rp;
   struct sockaddr_in addr, from;
   struct ip *ip;
   char buffer[1024];
   int sl;
   int hlen;

   clock_t start;
   clock_t end;
   double D_time[5];
   double Max,Min,Avg;
   struct hostent *HostName;
   u_long HostAddr;

   for(int i = 0; i <4 ; i ++){
   // ICMP에대응하는 RAW 소켓을 생성한다.
   icmp_socket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
   if(icmp_socket < 0)
   {
      perror("socket error : ");
      exit(0);
   }

   memset(buffer, 0x00, 1024);

   // icmp 패킷을 만든다.
   p = (struct icmp *)buffer;
   p->icmp_type = ICMP_ECHO;
   p->icmp_code = 0;
   p->icmp_cksum = 0;
   p->icmp_seq = 15;
   p->icmp_id = getpid();

   //p->icmp_cksum = in_cksum((u_short *)p, 1000);
  p->icmp_cksum = in_cksum((u_short *)p, sizeof(*p));
   memset(&addr, 0, sizeof(addr));
   HostName = gethostbyname(argv[1]);
   HostAddr = *((u_long *)(HostName -> h_addr_list[0]));
   addr.sin_addr.s_addr = inet_addr(inet_ntoa(HostAddr));
   addr.sin_family = AF_INET;

   // icmp 패킷을 상대 호스트로 전송한다.
   	start = clock();
	ret = sendto(icmp_socket,p,sizeof(*p), 0, (struct sockaddr *)&addr, sizeof(addr));
   	if (ret< 0)
   	{
      		perror("sendto error : ");
      		exit(0);
   	}

   	// 상대 호스트로 부터 icmp 응답을 기다린다.
   	sl = sizeof(from);
   	ret = recvfrom(icmp_socket,buffer, 1024, 0, (struct sockaddr *)&from, &sl);
   	if (ret < 0)
   	{
   	   //printf("%d %d %d\n", ret, errno, EAGAIN);
   	   perror("recvfrom error : ");
   	   exit(0);
   	}
	end = clock();
	D_time[i] = (double) (end - start)/CLOCKS_PER_SEC;
   	// 응답을 받았다면
   	// 해당 호스트의 IP정보와
   	// icmp 패킷 정보를 출력한다.
   	ip =	 (struct ip *)buffer;
   	hlen = ip->ip_hl*4;
   	rp = (struct icmp *)(buffer+hlen);
   	printf("reply from %s\n", inet_ntoa(from.sin_addr));
   	printf("Type : %d \n", rp->icmp_type);
   	printf("Code : %d \n", rp->icmp_code);
   	printf("Seq  : %d \n", rp->icmp_seq);
   	printf("Iden : %d \n", rp->icmp_id);
   	printf("TTL  : %d \n", ip->ip_ttl);
   	printf("RDT  : %lf \n", (double) (end - start)/CLOCKS_PER_SEC);
   }
   Max = Min = Avg = D_time[0];
   for(int i = 1; i <4; i++){
   	if(Max < D_time[i])
		Max = D_time[i];
	if(Min > D_time[i])
		Min = D_time[i];
        Avg = Avg + D_time[i]; 	
   }
   Avg = Avg/(double)4;
   printf("Max delay time is : %lf \n", Max);
   printf("Min delay time is : %lf \n", Min);
   printf("Avg delay time is : %lf \n", Avg);

   return 1;
}

int in_cksum( u_short *p, int n )
{
   register u_short answer;
   register long sum = 0;
   u_short odd_byte = 0;

   while( n > 1 )
   {
      sum += *p++;
      n -= 2;

   }

   if( n == 1 )
   {
      *( u_char* )( &odd_byte ) = *( u_char* )p;
      sum += odd_byte;

   }

   sum = ( sum >> 16 ) + ( sum & 0xffff );
   sum += ( sum >> 16 );
   answer = ~sum;

   return ( answer );

}
// --------------  end of file ----------------

