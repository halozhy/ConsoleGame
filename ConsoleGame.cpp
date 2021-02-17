#include <stdio.h>  
#include <string.h>  
#include <stdlib.h>  
#include <Windows.h>  
#include <conio.h> 
#include <string> 
#include <vector> 
#include "Maze.h"
using namespace std;

#define DELAY_TIME 20

/**
 * 全屏控制台窗口
 * 取消标题栏及边框
 */
void fullScreen()
{
	HWND hwnd = GetForegroundWindow();
	int cx = GetSystemMetrics(SM_CXSCREEN);            /* 屏幕宽度 像素 */
	int cy = GetSystemMetrics(SM_CYSCREEN);            /* 屏幕高度 像素 */

	LONG l_WinStyle = GetWindowLong(hwnd, GWL_STYLE);   /* 获取窗口信息 */
	/* 设置窗口信息 最大化 取消标题栏及边框 */
	SetWindowLong(hwnd, GWL_STYLE, (l_WinStyle | WS_POPUP | WS_MAXIMIZE) & ~WS_CAPTION & ~WS_THICKFRAME & ~WS_BORDER);

	SetWindowPos(hwnd, HWND_TOP, 0, 0, cx, cy, 0);
}

/**
 * 获取窗口的大小，返回一个SMALL_RECT类型的结构体
 */
SMALL_RECT SizeOfWindow(HANDLE hConsoleOutput)
{
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(hConsoleOutput, &info);
	return info.srWindow;
}
/**
 * 将光标移动到（x,y）这个位置
 */
void gotoxy(short x, short y) {
	COORD coord = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}
/**
 * 隐藏控制台的光标
 *
 */
void HideCursor(bool n)
{
	CONSOLE_CURSOR_INFO cursor_info = { 1, n };
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor_info);
}
/**
 * 在屏幕中间打印一行字，无'\n'
 *
 */
void printAtCenter(SMALL_RECT rect, const char* str) {
	system("cls");
	gotoxy(rect.Right / 2 - (short)strlen(str) / 2, rect.Bottom / 2);
	printf("%s", str);
}


/**
 * 以time为延迟，逐字打印字符串，无'\n'
 * 在打印文字期间，如果按下空格则可以实现跳过
 *
 */
void printSleep(const char* str, int time) {
	size_t i = 0;
	short skip = 0;
	while (true)
	{
		int ch;
		if (_kbhit()) {//如果有按键按下
			ch = _getch();
			if (ch == 32)//如果是空格键
			{
				skip = 1;
			}
		}

		if ((*str) == '\0') {
			break;
		}

		putchar((int)str[0]);

		str++;
		i++;
		if (skip == 0) {
			Sleep(time);
		}

	}
}
/**
 * 在屏幕中间以time为延迟，打印一行字
 *
 */
void printCenterSleep(SMALL_RECT rect, const char* str, int time) {
	system("cls");
	for (short i = 0; i < rect.Right; i++)
	{
		gotoxy(i, rect.Bottom / 2);
		putchar(32);
	}
	gotoxy(rect.Right / 2 - (short)strlen(str) / 2, rect.Bottom / 2);
	printSleep(str, time);
	while (true)
	{
		char c = _getch();
		if (c == 32) {
			break;
		}
	}
}

/**
 * 以time为延迟，逐字在下方打印字符串，无'\n'
 *
 */
void printBottom(SMALL_RECT rect, const char* name, const char* str, int time) {
	system("cls");
	//先打印出边框
	//打印上边界线
	short top = (short)((float)3 / 4 * rect.Bottom);
	//printf("%f\n", top);
	gotoxy(1, top);
	for (size_t i = 0; i < rect.Right; i++)
	{
		putchar('-');
	}
	//打印左边界线
	gotoxy(1, top + 1);
	for (short i = top + 1; i < rect.Bottom; i++)
	{
		gotoxy(1, i);
		putchar('|');
	}
	//打印下边界线
	gotoxy(1, rect.Bottom);
	for (size_t i = 0; i < rect.Right; i++)
	{
		putchar('-');
	}
	//打印右边界线
	gotoxy(rect.Right, top + 1);
	for (short i = top + 1; i < rect.Bottom; i++)
	{
		gotoxy(rect.Right, i);
		putchar('|');
	}
	//打印边框结束
	//开始局部更新
	for (short j = top + 1; j < rect.Bottom; j++)
	{
		gotoxy(2, j);
		for (short i = 2; i < rect.Right; i++)
		{
			putchar(' ');
		}

	}
	//局部更新结束
	//打印名字
	gotoxy(4, top + 2);
	//printSleep(name, 20);
	printf("%s", name);
	//putchar(':');

	//打印内容
	//Sleep(100);
	gotoxy(6, top + 4);
	printSleep(str, time);
	while (true)
	{
		char c = _getch();
		if (c == 32) {
			break;
		}
	}
}

/**
 * 打印出分支选项
 * select[][100]为要打印的选项的字符串二维数组,
 * posX为选项文字左上角(不包含指针)的X位置, posY为选项文字左上角(不包含指针)的Y位置,
 * num为选项的个数
 * 返回: 选择的是第几个选项(从1开始)
 *
 */
int printSelect(char select[][100], short posX, short posY, int num) {
	//打印分支选项
	for (size_t i = 0; i < num; i++)
	{
		gotoxy(posX, (short)(posY + i * 2));
		printf("%s", select[i]);
	}
	//打印指针
	gotoxy(posX - 3, posY);//指针的X位置 = 第一个选项的文字的位置 - 3
	printf("%s", "->");
	short curPosX = posX - 3;//和上一条注释同理
	short curPosY = posY;
	int curSel = 1;	

	while (true)
	{
		int ch;
		if (_kbhit()) {//如果有按键按下
			ch = _getch();
			//w 119
			//s 115
			//↑ 224 72
			//↓ 224 80
			//回车 13
			//空格 32
			if (ch == 224)
			{
				ch = _getch();
				switch (ch)
				{
				case 72:
					//上
					//不显示原始光标
					gotoxy(curPosX, curPosY);
					printf("%s", "  ");

					curSel--;
					if (curSel < 1) {//循环到最后一个
						curSel = num;
						curPosY = posY + 2 * (num - 1);
					}
					else {
						curPosY = curPosY - 2;
					}
					//printf("shang");
					break;
				case 80:
					//下
					//不显示原始光标
					gotoxy(curPosX, curPosY);
					printf("%s", "  ");

					curSel++;
					if (curSel > num) {
						curSel = 1;
						curPosY = posY;
					}
					else {
						curPosY = curPosY + 2;
					}
					break;
				default:
					break;
				}
			}
			else if (ch == 119) {
				//上
				//不显示原始光标
				gotoxy(curPosX, curPosY);
				printf("%s", "  ");

				curSel--;
				if (curSel < 1) {
					curSel = num;
					curPosY = posY + 2 * (num - 1);
				}
				else {
					curPosY = curPosY - 2;
				}
				//printf("shang");

			}
			else if (ch == 115)
			{
				//下
				//不显示原始光标
				gotoxy(curPosX, curPosY);
				printf("%s", "  ");

				curSel++;
				if (curSel > num) {
					curSel = 1;
					curPosY = posY;
				}
				else {
					curPosY = curPosY + 2;
				}
				//printf("xia");
			}
			else if (ch == 13 || ch == 32)
			{
				//确定
				//printf("OK");
				return curSel;
			}

			//刷新光标
			gotoxy(curPosX, curPosY);
			printf("%s", "->");
		}
	}
	return -1;
}

/**
 * 以time为延迟，逐字在下方打印字符串，无'\n'
 * 同时打印出分支选项
 * 返回: 选择的是第几个选项
 *
 */
int printBottomWithSelect(SMALL_RECT rect, const char* name, const char* str, int time, char select[][100], int num) {
	system("cls");
	//先打印出边框
	//打印上边界线
	short top = (short)((float)3 / 4 * rect.Bottom);
	//printf("%f\n", top);
	gotoxy(1, top);
	for (size_t i = 0; i < rect.Right; i++)
	{
		putchar('-');
	}
	//打印左边界线
	gotoxy(1, top + 1);
	for (short i = top + 1; i < rect.Bottom; i++)
	{
		gotoxy(1, i);
		putchar('|');
	}
	//打印下边界线
	gotoxy(1, rect.Bottom);
	for (size_t i = 0; i < rect.Right; i++)
	{
		putchar('-');
	}
	//打印右边界线
	gotoxy(rect.Right, top + 1);
	for (short i = top + 1; i < rect.Bottom; i++)
	{
		gotoxy(rect.Right, i);
		putchar('|');
	}
	//打印边框结束
	//开始局部更新
	for (short j = top + 1; j < rect.Bottom; j++)
	{
		gotoxy(2, j);
		for (short i = 2; i < rect.Right; i++)
		{
			putchar(' ');
		}

	}
	//局部更新结束
	//打印名字
	gotoxy(4, top + 2);
	//printSleep(name, 20);
	printf("%s", name);
	//putchar(':');

	//打印内容
	//Sleep(100);
	gotoxy(6, top + 4);
	printSleep(str, time);



	//打印分支选项
	short maxLen = (short)strlen(select[0]);
	for (size_t i = 0; i < num; i++)
	{
		if (maxLen < strlen(select[i])) {
			maxLen = (short)strlen(select[i]);
		}
	}
	short pos = (short)top + 2;
	for (size_t i = 0; i < num; i++)
	{
		gotoxy((short)rect.Right - 6 - maxLen, (short)(pos++ + i));
		printf("%s", select[i]);
	}

	return printSelect(select, (short)rect.Right - 6 - maxLen, (short)top + 2, num);
}

/**
 * 开屏“logo”展示
 */
void logo(SMALL_RECT rect) {
	Sleep(1000);
	char str[] = "A PBB Team Game";
	gotoxy(rect.Right / 2 - (short)strlen(str) / 2, rect.Bottom / 2);
	printSleep(str, 30);
	Sleep(1500);
	system("cls");
	Sleep(500);
}

/*
 * 传入一个文本文件名，读取其中的内容，保存至vector<string>里
 * 注意：文本文件需要保存为GBK(ANSI)编码格式才可被正确打开
 */
vector<string> fileReader(const char *fileName) {
	vector<string> v;
	FILE* file;
	fopen_s(&file,fileName, "r");
	if (file != NULL) {
		while (!feof(file)) {
			char buf[1000];
			fgets(buf, 1000, file);
			v.push_back(buf);
		}
	}
	else {
		exit(-1);
	}
	return v;
}
/*
 * 解析脚本并执行
 */
void run(vector<string> v, SMALL_RECT rect) {
	for (size_t i = 0; i < v.size(); i++)
	{
		if (strcmp(v.at(i).c_str(),"[MAZE]\n") == 0)
		{
			playMaze(rect);
		}
		else if (strcmp(v.at(i).c_str(),"[DIALOG]\n")==0) {
			i++;
			printBottom(rect, v.at(i).c_str(), v.at(i+1).c_str(), DELAY_TIME);
			i++;
		}
		else if (strcmp(v.at(i).c_str(), "[SEL]\n")==0) {
			i++;
			char name[1000];
			strcpy_s(name,v.at(i++).c_str());
			char str[1000];
			strcpy_s(str, v.at(i++).c_str());
			vector<string> sel;
			if (strcmp(v.at(i).c_str(), "[SEL OPTION]\n") == 0) {
				i++;
				int num = v.at(i)[0] - 48;
				i++;
				for (size_t j = 0; j < num; j++)
				{
					sel.push_back(v.at(i));
					i++;
				}
				char selc[9][100];
				for (size_t j = 0; j < sel.size(); j++)
				{
					strcpy_s(selc[j], sel.at(j).c_str());
				}
				int re = printBottomWithSelect(rect, name, str, DELAY_TIME, selc, (int)sel.size());
				char buf[10];
				sprintf_s(buf, "[%d]\n", re);
				while (strcmp(v.at(i).c_str(), buf) != 0) {
					i++;
				}
			}

			else {
				exit(-1);
			}
		}
		else if (strcmp(v.at(i).c_str(), "[GOTO]\n") == 0) {
			int newI;
			i++;
			int re = sscanf_s(v.at(i).c_str(),"%d", &newI);
			i = newI-2;
		}
		else {
			printCenterSleep(rect, v.at(i).c_str(),DELAY_TIME);
		}
	}
}

/**
 * 开屏菜单
 *
 */
void startMenu(SMALL_RECT rect) {
	//在画面中上部分显示标题与操作指引
	gotoxy(rect.Right / 2 - (short)2, (short)(rect.Bottom / 3 * 1));
	printf("%s", "操作指南：");
	char s1[] = "按下 空格键 跳至下一条对话，或确认选项";
	char s2[] = "通过 WASD 或者 方向键 移动选项光标，或在迷宫游戏中移动";
	gotoxy(rect.Right / 2 - (short)strlen(s1)/2, (short)(rect.Bottom / 3 * 1) + 2);
	printf("%s", s1);
	gotoxy(rect.Right / 2 - (short)strlen(s2)/2, (short)(rect.Bottom / 3 * 1) + 4);
	printf("%s", s2);
	//在画面中下部分显示出菜单
	gotoxy((short)(rect.Right / 2 - 2), (short)(rect.Bottom / 3 * 2));
	//菜单项数组
	char select[2][100] = { "新的故事","回到现实" };
	int re = printSelect(select, rect.Right / 2 - 2, rect.Bottom / 3 * 2, 2);
	//gotoxy(1, 1);
	//printf("%d", re);

	switch (re)
	{
	case 1:
		Sleep(300);
		system("cls");
		Sleep(300);
		run(fileReader("script.sp"), rect);
		break;
	case 2:
		exit(0);
		break;
	default:
		break;
	}
}
void thanks(SMALL_RECT rect) {
	system("cls");
	for (short i = 0; i < rect.Right; i++)
	{
		gotoxy(i, rect.Bottom / 2);
		putchar(32);
	}
	char s1[] = "感谢您游玩PBB团队开发的“文字解谜游戏”——《归》";
	gotoxy(rect.Right / 2 - (short)strlen(s1) / 2, rect.Bottom / 2);
	printSleep(s1, 10);
	char s2[] = "剧本中部分名词基于游戏《原神》的世界观，不过与其主线剧情并无关联，游戏剧情也可不需结合原世界观理解";
	gotoxy(rect.Right / 2 - (short)strlen(s2) / 2, rect.Bottom / 2 + 2);
	printSleep(s2, 10);
	char s3[] = "团队成员：张昊煜20195822，陈可欣20195378，孙璐通20195584";
	gotoxy(rect.Right / 2 - (short)strlen(s3) / 2, rect.Bottom / 2 + 4);
	printSleep(s3, 10);
	while (true)
	{
		char c = _getch();
		if (c == 32) {
			break;
		}
	}
}

int main()
{
	//全屏显示
	//fullScreen();

	//使窗口不可缩放，此行代码最后可以考虑删除掉
	SetWindowLong(GetForegroundWindow(), GWL_STYLE, GetWindowLong(GetForegroundWindow(), GWL_STYLE) & ~WS_SIZEBOX & ~WS_MAXIMIZEBOX & ~WS_MINIMIZEBOX);

	//隐藏滚动条，但是滚动条占的位置实际还是存在的
	HANDLE handle_out;
	CONSOLE_SCREEN_BUFFER_INFO info;
	handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(handle_out, &info);
	SMALL_RECT rect = SizeOfWindow(handle_out);
	//printf("L:%d R:%d T:%d B:%d\n", rect.Left, rect.Right,rect.Top,rect.Bottom);
	//B：53 R：188
	COORD size = { rect.Right + 1,rect.Bottom + 1 };
	SetConsoleScreenBufferSize(handle_out, size);

	//设置窗口标题
	system("title 归");
	//设置背景和前景色
	system("color 07");
	//让控制台支持中文编码
	system("chcp 936");
	system("cls");

	
	//开始
	//gotoxy(1,5);
	//puts("888");
	//gotoxy(rect.Right / 2, rect.Bottom / 2);

	//vector<string> v = fileReader("script.sp");
	//for (size_t i = 0; i < v.size(); i++)
	//{
	//printf("%s", v[i].c_str());
	//}

	logo(rect);

	//隐藏光标
	HideCursor(false);

	startMenu(rect);

	thanks(rect);

	//playMaze(rect);

	//puts("11111111111111111111111");
	//printAtCenter(rect, "11111111111111111111111111111111111111111111111111111111111111");
	////print_sleep("\n果然，不可以吗", 20);
	//printBottom(rect, "一个人名", "进不去，怎么想都进不去吧", 20);
	//printBottom(rect, "派蒙", "进不去，怎么想都进不去吧！", 20);
	//char select[3][100] = { "111","222","333" };
	//int re = printBottomWithSelect(rect, "名字", "一句话", 20, select, 3);
	//char buf[100];
	//sprintf_s(buf, "%s %d %s", "你选择了第", re, "项");
	//printBottom(rect, "派蒙", buf, 20);

	//结束

	CloseHandle(handle_out);
	//Sleep(5000);
	//system("pause");

	return 0;
}

