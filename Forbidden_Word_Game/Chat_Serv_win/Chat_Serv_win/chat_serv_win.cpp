#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <vector>
#include <process.h>

#pragma warning (disable:4996)
#pragma comment (lib,"ws2_32.lib")

#define ROOM_SIZE 5
#define ROOM_MAX_CLNT 4
#define BUF_SIZE 100
#define MAX_CLNT 256
#define TEST_PORT "5050"
#define USING_TEST_PORT 1

using namespace std;

unsigned WINAPI HandleClnt(void* arg);
void SendMsg(SOCKET hClntSock, int RoomNumber, char *msg, int len);
void ErrorHandling(const char* msg);
void Init();

typedef struct room_list
{
	int room_number; // 방 번호
	int vaild; // Room 오픈 여부
	int clntCnt; // 해당 방에 접속한 인원 수
}room_list;

typedef struct DB_Client
{
	int* sock_copy;
	struct sockaddr_in clnt_adr;
	char message[BUF_SIZE];
}DB_Client;

typedef struct client_list // client 정보를 확인하는 list
{
	SOCKET clntSocks;
	int Room_number;
}client_list;

HANDLE hMutex;
client_list Client_Information[MAX_CLNT];
SOCKET clntSocks[MAX_CLNT];
room_list room[ROOM_SIZE];
int clntCnt = 0;

int main(int argc, char* argv[])
{
	WSADATA wsaData;
	SOCKET hServSock, hClntSock;
	SOCKADDR_IN servAdr, clntAdr;
	HANDLE hThread;
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
	hMutex = CreateMutex(NULL, FALSE, NULL);
	hServSock = socket(PF_INET, SOCK_STREAM, 0);

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

	Init();
	printf("IT'S RUN!\n");
	while (1)
	{
		clntAdrSz = sizeof(clntAdr);
		int strLen = 0;
		char clnt_Message[BUF_SIZE];

		clntAdrSz = sizeof(clntAdr);
		hClntSock = accept(hServSock, (SOCKADDR*)&clntAdr, &clntAdrSz);

		WaitForSingleObject(hMutex, INFINITE);
		//clntSocks[clntCnt++] = hClntSock;
		Client_Information[clntCnt].clntSocks = hClntSock;
		Client_Information[clntCnt].Room_number = 0;
		clntCnt++;
		ReleaseMutex(hMutex);
	
		hThread = (HANDLE)_beginthreadex(NULL, 0, HandleClnt, (void*)&hClntSock, 0, NULL);
		printf("CONNECT client IP : %s\n", inet_ntoa(clntAdr.sin_addr));
		
	}
	closesocket(hServSock);
	WSACleanup();
	return 0;
}

unsigned WINAPI HandleClnt(void* arg)
{
	SOCKET hClntSock = *((SOCKET*)arg);
	int i, strLen = 0;
	char msg[BUF_SIZE];
	char sub_msg[BUF_SIZE];
	while (1)
	{
		strLen = recv(hClntSock, msg, sizeof(msg), 0);
		strncpy(sub_msg, msg, 6);
		sub_msg[6] = 0;;
		if (strcmp(sub_msg, "0xRoom") == 0)
		{
			WaitForSingleObject(hMutex, INFINITE);
			for (int i = 0; i < ROOM_SIZE; i++)
			{
				sprintf(msg, "%d%d%d", room[i].room_number, room[i].vaild, room[i].clntCnt);
				printf("%s\n", msg);
				send(hClntSock, msg, 4, 0);
				msg[0] = '\0';
			}
			ReleaseMutex(hMutex);
		}
		else if (strcmp(sub_msg, "0xJoin") == 0)
		{
			int RoomIndex = (msg[6] - '0') -1;

			WaitForSingleObject(hMutex, INFINITE);
			if (room[RoomIndex].clntCnt == MAX_CLNT)
			{
				strcpy(msg, "0xisFull");
				send(hClntSock, msg, 9, 0);
				ReleaseMutex(hMutex);

			}
			else if (room[RoomIndex].vaild == 1)
			{
				strcpy(msg, "0xGStart");
				send(hClntSock, msg, 9, 0);
				ReleaseMutex(hMutex);
			}
			else
			{
				strcpy(msg, "0xLogin!");
				send(hClntSock, msg, 9, 0);
				for (int i = 0; i < MAX_CLNT; i++)
				{
					if (hClntSock == Client_Information[i].clntSocks)
					{
						Client_Information[i].Room_number = RoomIndex;
						room[RoomIndex].clntCnt++;
						break;
					}
				}
				for (int i = 0; i < ROOM_SIZE; i++)
				{
					printf("%d\n", room[i].clntCnt);
				}
				ReleaseMutex(hMutex);
						
				while ((strLen = recv(hClntSock, msg, sizeof(msg), 0)) > 0)
				{
					msg[strLen] = 0;
					printf("[Room %d] : Logged : %s\n", RoomIndex+1, msg);
					SendMsg(hClntSock, RoomIndex, msg, strLen);
					strLen = 0;
				}

				WaitForSingleObject(hMutex, INFINITE);
				for (i = 0; i < clntCnt; i++)
				{
					if (hClntSock == Client_Information[i].clntSocks)
					{
						while (i++ < clntCnt - 1)
						{
							Client_Information[i].clntSocks = Client_Information[i + 1].clntSocks;
							Client_Information[i].Room_number = Client_Information[i + 1].Room_number;
							break;
						}
					}
				}
				clntCnt--;
				ReleaseMutex(hMutex);
				closesocket(hClntSock);
				

			}
		}
	}

	/*
	while ((strLen = recv(hClntSock, msg, sizeof(msg), 0)) > 0)
	{
		printf("Logged : %s\n", msg);
		SendMsg(hClntSock, msg, strLen);
		strLen = 0;
		memset(msg, 0, sizeof(msg));
	}


	WaitForSingleObject(hMutex, INFINITE);
	for (i = 0; i < clntCnt; i++)
	{
		if (hClntSock == clntSocks[i])
		{
			while (i++ < clntCnt - 1)
			{
				clntSocks[i] = clntSocks[i + 1];

			}
			break;
		}
	}
	clntCnt--;
	ReleaseMutex(hMutex);
	closesocket(hClntSock); */
	return 0;
}
/*
unsigned WINAPI HandleClnt(void* arg)
{
	SOCKET hClntSock = *((SOCKET*)arg);
	int strLen = 0, i;
	char msg[BUF_SIZE];
	while ((strLen = recv(hClntSock, msg, sizeof(msg), 0)) > 0)
	{
		printf("Logged : %s\n", msg);
		SendMsg(hClntSock, msg, strLen);
		strLen = 0;
		memset(msg, 0, sizeof(msg));
	}

		
	WaitForSingleObject(hMutex, INFINITE);
	for (i = 0; i < clntCnt; i++)
	{
		if (hClntSock == clntSocks[i])
		{
			while (i++ < clntCnt - 1)
			{
				clntSocks[i] = clntSocks[i + 1];
			}
			break;
		}
	}
	clntCnt--;
	ReleaseMutex(hMutex);
	closesocket(hClntSock);
	return 0;
}*/

/*
void SendMsg(SOCKET hClntSock, char* msg, int len)
{
	WaitForSingleObject(hMutex, INFINITE);
	for (int i = 0; i < clntCnt; i++)
	{
		if (hClntSock == clntSocks[i])
		{
			continue;
		}
		send(clntSocks[i], msg, len, 0);
	}
	ReleaseMutex(hMutex);
}
*/

void SendMsg(SOCKET hClntSock, int RoomNumber, char *msg, int len)
{
	WaitForSingleObject(hMutex, INFINITE);
	for (int i = 0; i < clntCnt; i++)
	{
		if (hClntSock == Client_Information[i].clntSocks)
		{
			continue;
		}
		if (Client_Information[i].Room_number == RoomNumber)
		{
			send(Client_Information[i].clntSocks, msg, len, 0);
		}
	}
	ReleaseMutex(hMutex);
}
void ErrorHandling(const char* msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}

void Init()
{
	// 방 초기화
	for(int i=0; i<ROOM_SIZE; i++)
	{
		room[i].room_number = i + 1;
		room[i].vaild = 0;
		room[i].clntCnt = 0; 
	}
	
}
