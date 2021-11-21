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
		printf("Server �׽�Ʈ�� Client\n");
		printf("1.�α���\n");
		printf("2.ID �Է�\n");
		printf("3.Credits\n");
		printf("4.����\n");
		printf("==================================\n");
		printf("���ڸ� �Է��ϰ� ����Ű�� ��������.\n");
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
				puts("ä�ü����� �����Ͽ����ϴ�.");
			}

			hJoinThread = (HANDLE)_beginthreadex(NULL, 0, JoinRoom, (void*)&hSocket, 0, NULL);
			WaitForSingleObject(hJoinThread, INFINITE);

		}
		else if (menu_input == '1' && ID[0] == NULL)
		{
			printf("Error : ID �Է��� �����Ͽ� �ּ���.\n");
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
			printf("�ƹ�Ű�� ������ ���α׷��� ����˴ϴ�.\n");
			system("pause");
			exit(1);
		}
		else
		{
			printf("�߸� �Է��Ͽ����ϴ�.\n");
		}
	}

	closesocket(hSocket);
	WSACleanup();
	return 0;
}

unsigned WINAPI JoinRoom(void *arg) // �� ���� Ȯ��
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
			printf("%c�� �� : ", RecvMsg[0]);
			printf("���� �ο� : %c/%d�� ", RecvMsg[2], ROOM_MAX_CLNT);
			if (RecvMsg[1] == '0')
			{
				printf("- ���� ���� -\n");
			}
			else if (RecvMsg[1] == '1')
			{
				printf("- ���� �� -\n");
			}
			else if (RecvMsg[1] == '2')
			{
				printf("- ���� �ʰ� -\n");
			}
		}
		printf("=======================================\n");
		printf("������ ���ϴ� �� ��ȣ�� �Է��ϰ� ����Ű�� ��������.\n");
		printf("���� �޴��� ���ư����� q�� �Է��ϼ���.\n");
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
				printf("������ ����á���ϴ�.\n");
				continue;
			}
			else if (strcmp(RecvMsg, "0xGStart") == 0)
			{
				printf("������ ���۵Ǿ� ������ �Ұ����մϴ�.\n");
				continue;
			}
			else
			{
				printf("%c���濡 �����Ͽ����ϴ�.\n", room_input);
				printf("========================\n");
				
				hSndThread = (HANDLE)_beginthreadex(NULL, 0, (_beginthreadex_proc_type)SendMsg, (void*)&hSocket, 0, NULL);
				hRcvThread = (HANDLE)_beginthreadex(NULL, 0, (_beginthreadex_proc_type)RecvMsg, (void*)&hSocket, 0, NULL);

				WaitForSingleObject(hSndThread, INFINITE);
				WaitForSingleObject(hRcvThread, INFINITE);

				
			}

		}
		else if (room_input == 'q')
		{
			printf("���� �޴��� ���ư��ϴ�.\n");
			break;
		}
		else
		{
			printf("�߸� �Է��Ͽ����ϴ�.\n");
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
		printf("ID�� �Է��ϼ���(�ѱ� 8�� �̳�, ���� 15�� �̳�) : ");
		getchar();
		fgets(buffer, BUF_SIZE, stdin);
		last_enter_delete(buffer);
		strcpy(ID, buffer);
		strLen = strlen(ID);
		if (strLen >= 16)
		{
			printf("�ִ� ���̿��� ������ϴ�.\n");
			continue;
		}
		else
		{
			break;
		}
	}
	printf("ID ������ �Ϸ� �Ǿ����ϴ�.\n");

}
void last_enter_delete(char* message) // fgets ������ ���Ͱ��� NULL���ڷ� ����.
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