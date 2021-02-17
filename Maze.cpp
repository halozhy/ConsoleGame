#include <iostream>
#include <vector>
#include <ctime>
#include <conio.h> 
#include <Windows.h> 
#include "ConsoleGame.h"

using namespace std;
#define m 11//row
#define n 39
#define down 1
#define right 2
#define left 4
#define up 8
#define WALL -1
#define NOTHING 0

struct block {
	int row, column, direction;
	block(int _row, int _column, int _direction) {
		row = _row;
		column = _column;
		direction = _direction;
	}
};
struct point {
	int x;
	int y;
}start, end;

vector<block> myblock;
int x_num = 1, y_num = 1;
int G[100][100];

struct point nowPos;
struct point oldPos;


void FindBlock() {
	//�ҳ��뵱ǰλ�����ڵ�ǽ
	if (x_num + 1 <= m && G[x_num + 1][y_num] == WALL) {//down
		myblock.push_back(block(x_num + 1, y_num, down));
	}
	if (y_num + 1 <= n && G[x_num][y_num + 1] == WALL) {//right
		myblock.push_back(block(x_num, y_num + 1, right));
	}
	if (x_num - 1 >= 1 && G[x_num - 1][y_num] == WALL) {//up
		myblock.push_back(block(x_num - 1, y_num, up));
	}
	if (y_num - 1 >= 1 && G[x_num][y_num - 1] == WALL) {//left
		myblock.push_back(block(x_num, y_num - 1, left));
	}
}

/*
 * ����0��ʾ�����ߣ�����-1��ʾ������
 */
int checkRoute() {
	if (nowPos.y == 1) {
		if (G[nowPos.x][nowPos.y] == WALL) {
			return -1;
		}
	}
	else {
		if (G[nowPos.x][(nowPos.y + 1) / 2] == WALL) {
			return -1;
		}
	}
	return 0;
}

/*
 * ���������תΪ��������
 */
struct point toAbsolutePos(struct point p, SMALL_RECT rect) {
	p.y = p.y + 1 + (rect.Right - n * 2) / 2 - 1;
	p.x = p.x + rect.Bottom / 2;
	return p;
}


//ע�⣺���д����x�����У���ֱ����y�����У�ˮƽ��������Game.cpp�е����в�ͬ
int playMaze(SMALL_RECT rect) {
	//����ͼȫ����Ϊǽ
	memset(G, WALL, sizeof(G));
	//������ʼ��
	G[1][1] = NOTHING;
	start.x = start.y = 1;

	srand((unsigned)time(NULL));//���������

	FindBlock();

	while (myblock.size()) {
		int BlockSize = (int)myblock.size();

		int randnum = rand() % BlockSize;
		block SelectBlock = myblock[randnum];
		x_num = SelectBlock.row;
		y_num = SelectBlock.column;
		switch (SelectBlock.direction) {
		case down: {
			x_num++;
			break;
		}
		case right: {
			y_num++;
			break;
		}
		case left: {
			y_num--;
			break;
		}
		case up: {
			x_num--;
			break;
		}
		}
		if (G[x_num][y_num] == WALL) {
			G[SelectBlock.row][SelectBlock.column] = G[x_num][y_num] = NOTHING;
			FindBlock();
		}
		else {
		}
		myblock.erase(myblock.begin() + randnum);
	}
	
	//��ӡ��ͼ
	system("cls");
	gotoxy((rect.Right - n*2) / 2-1, rect.Bottom / 2);

	for (int i = 0;i <= m + 1;i++) {
		for (int j = 0;j <= n + 1;j++) {
			if (i == start.x && j == start.y) {
				//printf("%c%c", 0xa7, 0xb0);
				printf("%s", "��");
			}
			else if (i == m && j == n) {
				printf("%s", "��");
			}
			else if (G[i][j] == NOTHING) {
				printf("  ");
			}
			else {
				//printf("%c%c", 0xa8, 0x80);
				printf("%c%c", '#', '#');
			}
		}
		printf("\n");
		for (short i = 0; i < (short)((rect.Right - n * 2) / 2) - 1; i++)
		{
			putchar(32);

		}
	}
	//��ӡ��ͼ���
	nowPos = start;
	oldPos = nowPos;
	//printf("%d %d", start.x, start.y);

	while (true)
	{
		int ch;
		if (_kbhit()) {//����а�������
			ch = _getch();
			//w 119
			//s 115
			//a 97
			//d 100
			//�� 224 72
			//�� 224 80
			//�� 224 75
			//�� 224 77
			//�س� 13
			//�ո� 32
			switch (ch)
			{
			case 224:
				ch = _getch();
				switch (ch)
				{
				case 72:
					//��
					nowPos.x--;
					if (checkRoute() == -1) {
						nowPos.x++;
					}
					break;
				case 80:
					//��
					nowPos.x++;
					if (checkRoute() == -1) {
						nowPos.x--;
					}
					break;
				case 75:
					//��
					nowPos.y -= 2;
					if (checkRoute() == -1) {
						nowPos.y += 2;
					}
					break;
				case 77:
					//��
					nowPos.y += 2;
					if (checkRoute() == -1) {
						nowPos.y -= 2;
					}
					break;
				default:
					break;
				}
				break;
			case 119:
				//��
				nowPos.x--;
				if (checkRoute() == -1) {
					nowPos.x++;
				}
				break;
			case 115:
				//��
				nowPos.x++;
				if (checkRoute() == -1) {
					nowPos.x--;
				}
				break;
			case 97:
				//��
				nowPos.y-=2;
				if (checkRoute() == -1) {
					nowPos.y+=2;
				}
				break;
			case 100:
				//��
				nowPos.y+=2;
				if (checkRoute() == -1) {
					nowPos.y -= 2;
				}
				break;
			case 13:
			case 32:
				//ȷ��
				break;
			default:
				break;
			}

			//�ֲ�ˢ�£�����ȫ��ˢ��
			//ע�⣺ʹ��gotoxyʱ��Ҫע������λ�õ�ת��
			//1 ����ԭ���ĵ�
			struct point printPos = toAbsolutePos(oldPos, rect);
			gotoxy(printPos.y, printPos.x);
			printf("  ");
			//2 ��ʾ���µ�λ��
			printPos = toAbsolutePos(nowPos, rect);
			gotoxy(printPos.y, printPos.x);
			printf("%s", "��");
			//3 ���µ�λ�ø�ֵΪ�ɵ�λ��
			oldPos = nowPos;
			

			//debug
			/*gotoxy1(90, 0);
			printf("                   ");
			gotoxy1(90, 0);
			printf("%d %d %d", G[start.x][(start.y)], start.x, start.y);

			gotoxy1(0, 13);
			for (int i = 0;i <= m + 1;i++) {
				for (int j = 0;j <= n + 1;j++) {
					printf("%d ", G[i][j]);
				}
				printf("\n");
			}*/

			//����ߵ����յ�
			if (nowPos.x == m && nowPos.y == (n * 2 - 1)) {
				return 1;
			}

		}
	}
	return 0;
}
