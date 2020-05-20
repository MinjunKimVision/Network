#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ctype.h>
#define BUF_LEN 128


int main(int argc, char *argv[]){
	struct sockaddr_in server_addr, client_addr;
	int s;				/*소켓번호*/
	int len, msg_size;
	char buf[BUF_LEN + 1]={'\0',};

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
		printf("Server : waiting request.\n");

		/*연결요청을 기다림*/
		if(msg_size = recvfrom(s, buf, BUF_LEN, 0,(struct sockaddr *)&client_addr, &len)<0){
			printf("recvfrom error\n");
			exit(1);
		}
		printf("Server : A client data received.\n");
		for(int i = 0; i <(int)strlen(buf)+1;i++){
			buf[i] = toupper(buf[i]);
		}
		printf("%s\n",buf);
		if(sendto(s, buf, msg_size, 0, (struct sockaddr *)&client_addr, len)<0) {
			exit(1);
		}
		printf("%s\n",buf);
		printf("sendto complete\n");
	}
}
