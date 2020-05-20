/*----------------------------------------------------------------
파일명 : myecho.c
기  능 : echo 서비스를 요구하는 TCP(연결형) 클라이언트
리눅스 : gcc -o myecho myecho.c
유닉스 : gcc -o myecho myecho.c -lsocket -lnsl
사용법 : myecho 127.0.0.1
----------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUF_LEN 128

int main(int argc, char *argv[]){
	int s, n, len_in, len_out;
	int flag = 1;
	struct sockaddr_in server_addr;
	char *haddr;
	char buf[BUF_LEN+1];
	char checkStream[] = "%%%";
	char checkBuf[BUF_LEN+1];
	if(argc != 3){
		printf("usage: %s ip_address port \n", argv[0]);
		exit(0);
	}
	haddr = argv[1];
	if((s = socket(PF_INET, SOCK_STREAM, 0)) < 0){
		printf("Cant't create socket\n");
		exit(0);
	}
	/* echo 서버의 소켓주소 구조체 작성 */
	bzero((char *)&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(haddr);
	server_addr.sin_port = htons(atoi(argv[2]));
	/* 연결요청 */
	if(connect(s, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
		printf("Can't connect.\n");
		exit(0);
	}

	send(s,checkStream,BUF_LEN,0);
	recv(s,checkBuf,BUF_LEN,0);
	if(!strcmp(checkBuf,"%%%")==0){
		printf("connection blast.\n");
	}
	else{
		while(flag ==1){
			/* 키보드 입력을 받음 */
			printf("Input any string : ");
			if(fgets(buf, BUF_LEN, stdin)) {
				buf[BUF_LEN] = '\0';
				len_out = strlen(buf);
			} else {
				printf("fgets error\n");
				exit(0);
			}
			/* echo 서버로 메시지 송신 */
			if (send(s, buf, len_out, 0) < 0) {
				printf("write error\n");
				exit(0);
			}
			if(strcmp(buf, "BYE\n")==0||strcmp(buf, "bye\n")==0){
				printf("BYE or bye is called. quit\n");
				flag = 0;
				break;
			}
			/* 수신된 echo 메시지 화면 출력 */
			printf("Echoed string : ");
			if(recv(s, buf, len_out, 0) < 0){
				printf("read error\n");
				exit(0);
			}
			printf("%s", buf);
		}//else
	}
	close(s);
}



