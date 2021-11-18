#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <process.h>

#pragma warning(disable:4996)
#pragma commnet(lib,"ws2_32.lib")

#define ROOM_SIZE 5
#define BUF_SIZE 120
#define MAX_CLNT 256;

void ErrorHandling(const char *msg);

typedef struct client_list // 접속 여부를 확인하는 clinet_list
{
	char id[25];
	struct sockaddr_in clnt_adr;
	bool connect; // is_Connect?
}client_list;

typedef struct chat_room
{
	int *serv_sock;
	int room_number;
	int port;
	char name[10];
}information;

typedef struct room_list
{
	bool vaild; // Room 오픈 여부
	char name[25];
	HANDLE tid;
	int port;
	int clnt_cnt;
}room_list;

typedef struct DB_Client
{
	int *sock_copy;
	struct sockaddr_in clnt_adr;
	char message[BUF_SIZE];
}DB_Client;

int main(int argc, char *argv[])
{
	WSADATA wsaData;
	SOCKET hServSock, hClntSock;
	SOCKADDR_IN servAdr, clntAdr;
	int clntAdrSz;

	if (argc != 2)
	{
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		ErrorHandling("WSAStartup() Error!");
	}

	hServSock = socket(PF_INET,SOCK_STREAM, 0);

	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family = AF_INET;
	servAdr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAdr.sin_port = htons(atoi(argv[1]));

	if (bind(hServSock, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
	{
		ErrorHandling("bind() Error!");
	}
	if (listen(hServSock, 5) == SOCKET_ERROR)
	{
		ErrorHandling("listen() Error");
	}

	while (1)
	{
		clntAdrSz = sizeof(clntAdr);
		hClntSock = accept(hServSock, (SOCKADDR*)&clntAdr, &clntAdrSz);

	}

}

void ErrorHandling(const char *msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}


