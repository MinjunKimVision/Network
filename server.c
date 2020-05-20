#include <stdio.h>
//#include <WinSock2.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

//#pragma comment(lib, "ws2_32")
//#pragma warning(disable : 4996)

#define PORT 4567
#define PACKET_SIZE 1024
#define SERVER_IP "192.168.0.13"

int main() {
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	SOCKET Socket;
	Socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	SOCKADDR_IN Addr = { '\0', };
	Addr.sin_family = AF_INET;
	Addr.sin_port = htons(PORT);
	Addr.sin_addr.s_addr = inet_addr(SERVER_IP);
	connect(Socket, (SOCKADDR*)&Addr, sizeof(Addr));
	char Message[] = "Client Send";
	send(Socket, Message, strlen(Message), 0);
	char Buffer[PACKET_SIZE] = { '\0', };
	recv(Socket, Buffer, PACKET_SIZE, 0);
	printf("Recv Massge : %s\n", Buffer);
	closesocket(Socket);
	WSACleanup();
	getchar();
	return 0;
}
