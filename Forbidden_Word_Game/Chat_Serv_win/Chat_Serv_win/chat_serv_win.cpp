#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <process.h>

#pragma warning (disable:4996)
#pragma comment (lib,"ws2_32.lib")

#define ROOM_SIZE 5
#define BUF_SIZE 120
#define MAX_CLNT 256
#define TEST_PORT "5050"
#define USING_TEST_PORT 1

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
	char message[BUF_SIZE];

	if (USING_TEST_PORT == 0)
	{
		if (argc != 2)
		{
			printf("Usage : %s <port>\n", argv[0]);
			exit(1);
		}
	}

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		ErrorHandling("WSAStartup() Error!");
	}

	hServSock = socket(PF_INET,SOCK_STREAM, 0);

	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family = AF_INET;
	servAdr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAdr.sin_port = htons(atoi(TEST_PORT));

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
		printf("IT'S RUN!\n");
		clntAdrSz = sizeof(clntAdr);
		int strLen = 0;
		char clnt_Message[BUF_SIZE];
		char clnt_Send_Message[BUF_SIZE];
		hClntSock = accept(hServSock, (SOCKADDR*)&clntAdr, &clntAdrSz);
		
		while((strLen = recv(hClntSock, clnt_Message, sizeof(clnt_Message), 0)) != 0)
		{
			clnt_Message[strLen-1] = 0;
			printf("Message From Clinet : %s\n", clnt_Message);
			send(hClntSock, clnt_Message, strLen, 0);
		}



	}

}

void ErrorHandling(const char *msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}


