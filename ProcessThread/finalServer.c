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
char buf[BUF_LEN+1] = {'\0',};//두 함수가 같은 버퍼를 사용합니다.
struct sockaddr_in server_addr, client_addr;
int server_fd,client_fd,thr_id,len,s;      /*소켓번호*/
pthread_t udp_thread;
int msg_size;

int main(int argc, char *argv[]){

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

   /* iterative echo 서비스 수행 */
   while(1) {
      memset(buf,'\0',BUF_LEN+1);
      printf("Server : waiting request.\n");
         if((msg_size = recvfrom(s, buf, BUF_LEN, 0,(struct sockaddr *)&client_addr, &len))<0){
         printf("recvfrom error\n");
         exit(1);
      }else{
         i++;
         thr_id = pthread_create(&udp_thread,NULL,udpRxThread,(void*)&client_fd);
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
   int msg_size;
   pthread_t id;
   
   id = pthread_self();
   printf("Thread[%ud]-Rx Msg:%s,\n",id,buf);
   strncat(buf, "-server sended.",14);
   printf("%s\n",buf);
   if(sendto(s, buf, msg_size+22, 0, (struct sockaddr *)&client_addr, len)<0) {
      printf("Send to Fault\n");
      exit(1);
   }
   printf("%d Thread Id at udpThreadFunc = %ud\n",i,(int)id);
   //srand(time(NULL));
   sleep(10);//원훨환 테스트를 위해 10초로 고정하였습니다.
   i--;//현재 실행되는 쓰레드의 개수를 가늠하기 위함입니다.
}
