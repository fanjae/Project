#include <stdio.h>
#include <Windows.h>

#pragma warning (disable:4996)
void gotoxy(int x, int y)
{
	COORD Pos;
	Pos.X = x;
	Pos.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
}

void gotoxy_in(int x, int y)
{
	COORD Pos;
	Pos.X = x;
	Pos.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_INPUT_HANDLE), Pos);
}

int main(void)
{
	gotoxy(0, 2);
	int n;
	printf("Test\n");
	gotoxy_in(0, 20);
	scanf("%d", &n);

}