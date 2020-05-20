#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>
#include <unistd.h>
#define BUF_LEN 128
void *udpRxThread(void *clientSocket);
int i=0;

char buf[BUF_LEN] = {'\0',}; 

int main(int argc, char *argv[]){
   struct sockaddr_in server_addr, client_addr;
   int server_fd,client_fd,thr_id,len,s;      /*소켓번호*/
   pthread_t udp_thread;

   if(argc != 2) {
      printf("usage: %s port\n", argv[0]);
      exit(0);
   }

   /*소켓 생성*/
   if((s=socket(PF_INET, SOCK_DGRAM, 0))<0) {
      printf("Server: Can't open stream socket.");
      exit(0);
   }

   /*server_addr을 '\0' 으로 초기화 */
   bzero((char *)&server_addr, sizeof(server_addr));
   bzero((char *)&client_addr, sizeof(client_addr));

   /*server_addr 세팅 */
   server_addr.sin_family = AF_INET;
   server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
   server_addr.sin_port = htons(atoi(argv[1]));

   /*bind()호출*/
   if(bind(s, (struct sockaddr *)&server_addr, sizeof(server_addr)) <0) {
      printf("Server: Can't bind local address.\n");
      exit(0);
   }
   len = sizeof(struct sockaddr);
   int msg_size;
   //char buf[BUF_LEN+1];
   /* iterative echo 서비스 수행 */
   while(1) {
      printf("Server : waiting request.\n");
      
      if((msg_size = recvfrom(s, buf, BUF_LEN, 0,(struct sockaddr *)&client_addr, &len))<0){
         printf("recvfrom error\n");
         exit(1);
      }else{
	printf("Recv Msg : %s\n", buf);
         thr_id = pthread_create(&udp_thread,NULL,udpRxThread,(void*)&client_fd);
         i++;
         printf("%d Thread Id at main =%ud\n",i,udp_thread);
         if(thr_id<0){
            perror("Thread Error");
            continue;
         }
      }

      sleep(3);
      printf("sendto complete\n");
   }
}
void *udpRxThread(void *clientSocket){
   int msg_size,c_socket;
   //char buf[BUF_LEN+1];
   pthread_t id;
   
   id = pthread_self();
   c_socket=*((int*)clientSocket);
   
   //msg_size = recv(c_socket,buf,BUF_LEN,0);
   printf("Thread[%ud]-Rx Msg:%s,\n",id,buf);
   send(c_socket,buf,msg_size,0);
   
   /*
   if((msg_size = recvfrom(c_socket, buf, BUF_LEN, 0,(struct sockaddr *)&in6_addr, &link))<0){
      printf("recvfrom error\n");
      exit(1);
   }
   printf("Server : A client data received.\n");

   if(sendto(c_socket, buf, msg_size, 0, (struct sockaddr *)&client_addr, len)<0) {
      exit(1);
   }
   printf("%d Thread Id at udpThreadFunc = %ud\n",c_socket,(int)id);*/
   srand(time(NULL));
   sleep(rand()%3);
   i--;
}
