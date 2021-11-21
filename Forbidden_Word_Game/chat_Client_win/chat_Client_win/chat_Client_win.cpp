#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <process.h>

#define BUF_SIZE 100
#define TEST_MODE 1
#define TEST_IP "183.101.168.117"
#define TEST_PORT "5050"
#define ROOM_SIZE 5
#define ROOM_MAX_CLNT 4

#pragma warning (disable:4996)
#pragma comment (lib,"ws2_32.lib")

unsigned WINAPI JoinRoom(void * arg);
unsigned WINAPI SendMsg(void* arg);
unsigned WINAPI RecvMsg(void* arg);
void ErrorHandling(const char* message);
void Set_ID_Name();
void last_enter_delete(char* data);
char ID[BUF_SIZE];

int main(int argc, char* argv[])
{
	WSADATA wsaData;
	SOCKADDR_IN servAdr;
	SOCKET hSocket;
	HANDLE hJoinThread;
	char message[BUF_SIZE];
	int strLen;

	if (TEST_MODE == 1)
	{
		strcpy(ID, "TEST");
	}

	while (1)
	{
		char menu_input;
		printf("==================================\n");
		printf("Server 테스트용 Client\n");
		printf("1.로그인\n");
		printf("2.ID 입력\n");
		printf("3.Credits\n");
		printf("4.종료\n");
		printf("==================================\n");
		printf("숫자를 입력하고 엔터키를 누르세요.\n");
		scanf(" %1c", &menu_input);
		rewind(stdin);

		if (menu_input == '1' && ID[0] != NULL)
		{
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
				servAdr.sin_addr.s_addr = inet_addr(TEST_IP);
				servAdr.sin_port = htons(atoi(TEST_PORT));
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
				puts("채팅서버에 접속하였습니다.");
			}

			hJoinThread = (HANDLE)_beginthreadex(NULL, 0, JoinRoom, (void*)&hSocket, 0, NULL);
			WaitForSingleObject(hJoinThread, INFINITE);

		}
		else if (menu_input == '1' && ID[0] == NULL)
		{
			printf("Error : ID 입력을 진행하여 주세요.\n");
		}
		else if (menu_input == '2')
		{
			Set_ID_Name();
		}
		else if (menu_input == '3')
		{

		}
		else if (menu_input == '4')
		{
			printf("아무키나 누르면 프로그램이 종료됩니다.\n");
			system("pause");
			exit(1);
		}
		else
		{
			printf("잘못 입력하였습니다.\n");
		}
	}

	closesocket(hSocket);
	WSACleanup();
	return 0;
}

unsigned WINAPI JoinRoom(void *arg) // 방 정보 확인
{
	SOCKET hSocket = *((SOCKET*)arg);
	HANDLE hSndThread, hRcvThread;

	int strLen;
	char room_input;
	char SendMsg[BUF_SIZE],RecvMsg[BUF_SIZE];

	while (1)
	{
		strcpy(SendMsg, "0xRoom");
		send(hSocket, SendMsg, BUF_SIZE, 0);
		printf("===============ROOM LIST===============\n");
		for (int i = 0; i < ROOM_SIZE; i++)
		{
			recv(hSocket, RecvMsg, 4, 0);
			printf("%c번 방 : ", RecvMsg[0]);
			printf("접속 인원 : %c/%d명 ", RecvMsg[2], ROOM_MAX_CLNT);
			if (RecvMsg[1] == '0')
			{
				printf("- 접속 가능 -\n");
			}
			else if (RecvMsg[1] == '1')
			{
				printf("- 게임 중 -\n");
			}
			else if (RecvMsg[1] == '2')
			{
				printf("- 정원 초과 -\n");
			}
		}
		printf("=======================================\n");
		printf("접속을 원하는 방 번호를 입력하고 엔터키를 누르세요.\n");
		printf("메인 메뉴로 돌아가려면 q를 입력하세요.\n");
		printf("=======================================\n");
		scanf(" %1c", &room_input);
		rewind(stdin);

		if (room_input >= '1' && room_input <= '5')
		{
			sprintf(SendMsg, "0xJoin%c", room_input);
			printf("SendMsg : %s\n", SendMsg);
			send(hSocket, SendMsg, 8, 0);
			
			recv(hSocket, RecvMsg, 9, 0);
			printf("%s\n", RecvMsg);
			if (strcmp(RecvMsg, "0xisFull") == 0)
			{
				printf("정원이 가득찼습니다.\n");
				continue;
			}
			else if (strcmp(RecvMsg, "0xGStart") == 0)
			{
				printf("게임이 시작되어 입장이 불가능합니다.\n");
				continue;
			}
			else
			{
				printf("%c번방에 접속하였습니다.\n", room_input);
				printf("========================\n");
				
				hSndThread = (HANDLE)_beginthreadex(NULL, 0, (_beginthreadex_proc_type)SendMsg, (void*)&hSocket, 0, NULL);
				hRcvThread = (HANDLE)_beginthreadex(NULL, 0, (_beginthreadex_proc_type)RecvMsg, (void*)&hSocket, 0, NULL);

				WaitForSingleObject(hSndThread, INFINITE);
				WaitForSingleObject(hRcvThread, INFINITE);

				
			}

		}
		else if (room_input == 'q')
		{
			printf("메인 메뉴로 돌아갑니다.\n");
			break;
		}
		else
		{
			printf("잘못 입력하였습니다.\n");
			continue;
		}
	}
	return 0;
}


unsigned WINAPI SendMsg(void* arg)
{
	SOCKET hSock = *((SOCKET*)arg);
	while (1)
	{
		int strLen = 0;
		char sendMsg[BUF_SIZE] = { 0 };
		char FullMsg[BUF_SIZE] = { 0 };
		fgets(sendMsg, BUF_SIZE, stdin);
		if (sendMsg[0] == '\n' || sendMsg[0] == ' ')
		{
			continue;
		}
		if (!strcmp(sendMsg, "q\n") || !strcmp(sendMsg, "Q\n"))
		{
			closesocket(hSock);
			exit(0);
		}
		last_enter_delete(sendMsg);

		sprintf(FullMsg, "[%s] : %s", ID, sendMsg);
		for (int i = 0; ; i++)
		{
			if (FullMsg[i] != 0)
			{
				strLen++;
			}
			else
			{
				break;
			}
		}
		send(hSock, FullMsg, strLen, 0);
	}
	return 0;
}

unsigned WINAPI RecvMsg(void* arg)
{
	SOCKET hSock = *((SOCKET*)arg);
	int strLen = 0;
	char RecvMsg[BUF_SIZE];
	while (1)
	{
		strLen = recv(hSock, RecvMsg, BUF_SIZE * 2, 0);
		if (strLen == -1)
		{
			return -1;
		}
		RecvMsg[strLen] = '\n';
		RecvMsg[strLen + 1] = 0;
		fputs(RecvMsg, stdout);
	}
	return 0;
}
void ErrorHandling(const char* message) // ErrorHandling Function 
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
void Set_ID_Name()
{
	int strLen = 0;
	char buffer[BUF_SIZE];
	while (1)
	{
		printf("==================================\n");
		printf("ID를 입력하세요(한글 8자 이내, 영문 15자 이내) : ");
		getchar();
		fgets(buffer, BUF_SIZE, stdin);
		last_enter_delete(buffer);
		strcpy(ID, buffer);
		strLen = strlen(ID);
		if (strLen >= 16)
		{
			printf("최대 길이에서 벗어났습니다.\n");
			continue;
		}
		else
		{
			break;
		}
	}
	printf("ID 셋팅이 완료 되었습니다.\n");

}
void last_enter_delete(char* message) // fgets 마지막 엔터값을 NULL문자로 변경.
{
	int i;
	for (i = 0; ; i++)
	{
		if (message[i] == '\n')
		{
			message[i] = 0;
			break;
		}
	}
}