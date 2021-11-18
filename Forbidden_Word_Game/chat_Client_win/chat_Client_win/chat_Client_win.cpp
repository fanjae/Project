#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define BUF_SIZE 1024
#define TEST_MODE 1
#define TEST_IP 127.0.0.1
#define TEST_PORT 5050

#pragma warning (disable:4996)
#pragma comment (lib,"ws2_32.lib")

void ErrorHandling(const char *message);

int main(int argc, char *argv[])
{
	WSADATA wsaData;
	SOCKET hSocket;
	char message[BUF_SIZE];
	int strLen;
	SOCKADDR_IN servAdr;

	if (TEST_MODE == 0)
	{
		if (argc != 3)
		{
			printf("Usage : %s <IP> <port>\n", argv[0]);
			exit(1);
		}
	}

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		ErrorHandling("WSAStartup() Error");
	}

	hSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (hSocket == INVALID_SOCKET)
	{
		ErrorHandling("socket() error");
	}

	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family = AF_INET;
	if (TEST_MODE == 1)
	{
		servAdr.sin_addr.s_addr = inet_addr("127.0.0.1");
		servAdr.sin_port = htons(atoi("5050"));
	}

	if (TEST_MODE == 0)
	{
		servAdr.sin_addr.s_addr = inet_addr(argv[1]);
		servAdr.sin_port = htons(atoi(argv[2]));
	}

	if (connect(hSocket, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
	{
		ErrorHandling("connect() error!");
	}
	else
	{
		puts("Connected...............");
	}

	while (1)
	{
		fputs("Input message(Q to quit) : ", stdout);
		fgets(message, BUF_SIZE, stdin);

		if (!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
		{
			break;
		}

		send(hSocket, message, strlen(message), 0);
		strLen = recv(hSocket, message, BUF_SIZE - 1, 0);
		message[strLen] = 0;
		printf("Message from server : %s\n", message);
	}
	closesocket(hSocket);
	WSACleanup();
	return 0;
}

void ErrorHandling(const char *message) // ErrorHandling Function 
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}