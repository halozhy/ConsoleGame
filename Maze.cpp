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
	//找出与当前位置相邻的墙
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
 * 返回0表示可以走，返回-1表示不能走
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
 * 将相对坐标转为绝对坐标
 */
struct point toAbsolutePos(struct point p, SMALL_RECT rect) {
	p.y = p.y + 1 + (rect.Right - n * 2) / 2 - 1;
	p.x = p.x + rect.Bottom / 2;
	return p;
}


//注意：下列代码的x代表列（垂直），y代表行（水平），这点和Game.cpp中的略有不同
int playMaze(SMALL_RECT rect) {
	//将地图全部置为墙
	memset(G, WALL, sizeof(G));
	//定义起始点
	G[1][1] = NOTHING;
	start.x = start.y = 1;

	srand((unsigned)time(NULL));//随机数种子

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
	
	//打印地图
	system("cls");
	gotoxy((rect.Right - n*2) / 2-1, rect.Bottom / 2);

	for (int i = 0;i <= m + 1;i++) {
		for (int j = 0;j <= n + 1;j++) {
			if (i == start.x && j == start.y) {
				//printf("%c%c", 0xa7, 0xb0);
				printf("%s", "我");
			}
			else if (i == m && j == n) {
				printf("%s", "终");
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
	//打印地图完成
	nowPos = start;
	oldPos = nowPos;
	//printf("%d %d", start.x, start.y);

	while (true)
	{
		int ch;
		if (_kbhit()) {//如果有按键按下
			ch = _getch();
			//w 119
			//s 115
			//a 97
			//d 100
			//↑ 224 72
			//↓ 224 80
			//← 224 75
			//→ 224 77
			//回车 13
			//空格 32
			switch (ch)
			{
			case 224:
				ch = _getch();
				switch (ch)
				{
				case 72:
					//上
					nowPos.x--;
					if (checkRoute() == -1) {
						nowPos.x++;
					}
					break;
				case 80:
					//下
					nowPos.x++;
					if (checkRoute() == -1) {
						nowPos.x--;
					}
					break;
				case 75:
					//左
					nowPos.y -= 2;
					if (checkRoute() == -1) {
						nowPos.y += 2;
					}
					break;
				case 77:
					//右
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
				//上
				nowPos.x--;
				if (checkRoute() == -1) {
					nowPos.x++;
				}
				break;
			case 115:
				//下
				nowPos.x++;
				if (checkRoute() == -1) {
					nowPos.x--;
				}
				break;
			case 97:
				//左
				nowPos.y-=2;
				if (checkRoute() == -1) {
					nowPos.y+=2;
				}
				break;
			case 100:
				//右
				nowPos.y+=2;
				if (checkRoute() == -1) {
					nowPos.y -= 2;
				}
				break;
			case 13:
			case 32:
				//确定
				break;
			default:
				break;
			}

			//局部刷新，不做全局刷新
			//注意：使用gotoxy时需要注意坐标位置的转换
			//1 消除原来的点
			struct point printPos = toAbsolutePos(oldPos, rect);
			gotoxy(printPos.y, printPos.x);
			printf("  ");
			//2 显示最新的位置
			printPos = toAbsolutePos(nowPos, rect);
			gotoxy(printPos.y, printPos.x);
			printf("%s", "我");
			//3 将新的位置赋值为旧的位置
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

			//如果走到了终点
			if (nowPos.x == m && nowPos.y == (n * 2 - 1)) {
				return 1;
			}

		}
	}
	return 0;
}
