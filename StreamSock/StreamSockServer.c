/*----------------------------------------------------------------
파일명 : myecho_server.c
기  능 : echo 서비스를 수행하는 서버
리눅스 : gcc -o myecho_server myecho_server.c
유닉스 : gcc -o myecho_server myecho_server.c -lsocket -lnsl
사용법 : myecho_server 2049
----------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ctype.h>

#define BUF_LEN 128

int main(int argc, char *argv[]) {
	struct sockaddr_in server_addr, client_addr;
	int server_fd, client_fd;   /* 소켓 번호 */
	int len, msg_size;
	int flag = 1;
	char buf[BUF_LEN+1];
	char checkStream[] = "%%%";
	char checkBuf[BUF_LEN+1];
	if(argc !=2){
		printf("usage: %s port\n", argv[0]);
		exit(0);
	}
	/* 소켓 생성 */
	if((server_fd = socket(PF_INET, SOCK_STREAM, 0)) < 0){
		printf("Server: Can't open stream socket.");
		exit(0);
	}
	
	/* server_addr을 '\0'으로 초기화 */
	bzero((char *)&server_addr, sizeof(server_addr));
	/* server_addr 세팅 */
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(atoi(argv[1]));
	
	/* bind() 호출 */
	if(bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
		printf("Server: Can't bind local address.\n");
		exit(0);
	}
	
	/* 소켓을 수동 대기모드로 세팅 */
	listen(server_fd, 5);
	
	/* iterative echo 서비스 수행 */
	while(flag == 1) {
		printf("Server : waiting connection request.\n");
		len = 16;
		/* 연결요청을 기다림 */
		client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &len);
		if(client_fd < 0) {
			printf("Server: accept failed.\n");
			exit(0);
		}
		send(client_fd,checkStream,4,0);
                recv(client_fd,checkBuf,sizeof(checkBuf),0);
                if(!strcmp(checkBuf,"%%%")==0){
                        printf("Connection blast\n");
                }
                else{
			printf("Server : A client connected.\n");
			while(flag ==1) {
				msg_size = recv(client_fd, buf, sizeof(buf), 0);
				for(int i = 0; i < (int) strlen(buf); i++){
					if(buf[i]>='A' && buf[i]<='Z'){
						buf[i] = tolower(buf[i]);
					}
					else if(buf[i]>='a' && buf[i] <='z'){
						buf[i] = toupper(buf[i]);
					}
				}
				if(strcmp(buf,"bye\n")==0){
					printf("BYE is called. quit\n");
					flag = 0;
					break;
				}
				else if (strcmp(buf,"BYE\n")==0){
					printf("bye is called. client has quit\n");
					break;
				}
				send(client_fd, buf, msg_size, 0);
			}
			close(client_fd);
		}//else
	}
	close(server_fd);
}

