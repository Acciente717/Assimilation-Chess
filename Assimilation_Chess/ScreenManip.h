#pragma once
#include<iostream>
#include<graphics.h>
#include<conio.h>
#include<vector>
#include<cstring>
#include<cmath>
#include<Windows.h>
#include<process.h>
#include"ChessBoard.h"

using namespace std;

// Logo旋转角度
int mainMenuLogoRotateRadius, playBoardLogoRotateRadius;

// Logo旋转开关
int mainMenuLogoRotateSpeed, playBoardLogoRotateSpeed;

// Logo
IMAGE logo;
IMAGE rotatedLogo;

// 主菜单Logo旋转线程
unsigned _stdcall MainMenuLogoRotate(void* pArg)
{
	while (true)
	{
		Sleep(30);
		mainMenuLogoRotateRadius = (mainMenuLogoRotateRadius - mainMenuLogoRotateSpeed + 360) % 360;
		rotateimage(&rotatedLogo, &logo, double(mainMenuLogoRotateRadius) / 180 * 3.1415926, WHITE);
		putimage(30, 20, &rotatedLogo, SRCCOPY);

		if (mainMenuLogoRotateSpeed == 0)
		{
			return 0;
		}
	}

	return 0;
}

// 游戏界面Logo旋转线程
unsigned _stdcall PlayBoardLogoRotate(void* pArg)
{
	while (true)
	{
		Sleep(30);
		playBoardLogoRotateRadius = (playBoardLogoRotateRadius - playBoardLogoRotateSpeed + 360) % 360;
		rotateimage(&rotatedLogo, &logo, double(playBoardLogoRotateRadius) / 180 * 3.1415926, WHITE);
		putimage(520, 310, &rotatedLogo, SRCCOPY);

		if (playBoardLogoRotateSpeed == 0)
		{
			return 0;
		}
	}

	return 0;
}

// 鼠标坐标
struct MouseCoordinate
{
	int mouseX, mouseY;
};

// 棋子移动向量
struct moveVector
{
	int startHori, startVert, endHori, endVert;
};

// 绘图序列元素
struct toDrawQueue
{
	int rowNum, colomnNum;

	/*
		0 白->空
		1 黑->空
		2 白->黑
		3 黑->白
		4 空->白
		5 空->黑
		6 空->黄
		7 黄->空
	**/
	int drawType;

};

class Screen
{

	// ####################
	// ###  CLI交互区   ###
	// ####################
public:

	// 输出主菜单 
	void ShowMenu()
	{
		cout << "##########################" << endl;
		cout << "#   欢迎进入手动控制台   #" << endl;
		cout << "##########################" << endl;
		cout << "命令列表：" << endl;
		cout << "输入1：开始AI先手游戏" << endl;
		cout << "输入2：开始玩家先手游戏" << endl;
		cout << "输入3：开始双人对战" << endl;
		cout << "输入4：读取记录" << endl;
		cout << "输入142857：训练模式" << endl << endl;
		cout << "行棋控制方式：" << endl;
		cout << "输入： 起始行 起始列 目标行 目标列" << endl;
		cout << "输入： 0 0 0 0 保存" << endl;
		cout << "输入： -1 -1 -1 -1 悔棋" << endl;
		cout << endl << endl << endl;
		return;
	}

	// 输出棋盘
	void ShowBoard(ChessBoard* toBoard)
	{
		toBoard->Print();
		return;
	}

	// 输出赢家
	void ShowWinner(ChessBoard* toBoard)
	{
		if (toBoard->IsTie())
			cout << "平手！游戏结束。" << endl;
		else
		{
			// 当前应落子者为黑方（白胜）
			if (toBoard->IsWhiteWin())
				cout << "白棋胜出！游戏结束。" << endl;
			else
				cout << "黑棋胜出！游戏结束。" << endl;
		}
	}

	// ####################
	// ###  GUI交互区   ###
	// ####################
protected:

	// ################
	// #  调试函数区  #
	// ################

	MouseCoordinate memMouseCoordinate = { 0,0 };
	char coordinateString[8] = "000,000";

	// #调试函数# 鼠标坐标的字符串转换
	void ConvertCoordinateToString(char *str, int coordinate, bool isX)
	{
		if (isX)
		{
			str[2] = coordinate % 10 + '0';
			coordinate /= 10;
			str[1] = coordinate % 10 + '0';
			coordinate /= 10;
			str[0] = coordinate % 10 + '0';
		}
		else
		{
			str[6] = coordinate % 10 + '0';
			coordinate /= 10;
			str[5] = coordinate % 10 + '0';
			coordinate /= 10;
			str[4] = coordinate % 10 + '0';
		}
	}

	// #调试函数# 显示鼠标坐标
	void ShowMouseCoordinate(int tempMouseX, int tempMouseY)
	{
		// 新鼠标坐标
		if (tempMouseX != memMouseCoordinate.mouseX || tempMouseY != memMouseCoordinate.mouseY)
		{
			setwritemode(R2_XORPEN);
			settextcolor(RED);
			outtextxy(0, 0, coordinateString);
			ConvertCoordinateToString(coordinateString, tempMouseX, true);
			ConvertCoordinateToString(coordinateString, tempMouseY, false);
			outtextxy(0, 0, coordinateString);
			setwritemode(R2_COPYPEN);
		}
	}

	// ################
	// #  功能函数区  #
	// ################

protected:

	// ################
	// #    数据区    #
	// ################

	// 当前棋盘记录
	char drawedBoard[11][11];

	// 棋子绘制处理对列
	vector<toDrawQueue> queue;

	// 落子动态追踪坐标
	int dynamicRow = -1, dynamicColomn = -1;

	// 主菜单动态追踪按键号
	int mainMenuDynamicTrackNum = -1;

	// 读取记录动态追踪按键号
	int readDataDynamicTrackNum = -1;

	// 保存记录动态追踪按键号
	int saveDataDynamicTrackNum = -1;

	// 规则、关于页面动态追踪按键号
	int ruleAndAboutDynamicTrackNum = -1;

	// 游戏界面动态追踪按键号
	int playBoardDynamicTrackNum = -1;

	// 图片是否读取过
	bool mainMenuImageRead = false, playBoardImageRead = false, ruleImageRead = false;

	// 是否允许悔棋
	bool screenIsRegretOK = false;

	// 记录状态指示数
	int firstLocationIndicator;
	int secondLocationIndicator;
	int thirdLocationIndicator;
	int fourthLocationIndicator;
	int fifthLocationIndicator;

	// AI难度开关状态
	bool easyAIOn = false;
	bool normalAIOn = true;
	bool hardAIOn = false;

	// 规则页面编号
	int rulePageNum = 1;

	// 主菜单
	// 按键文字
	IMAGE AIFirst;
	IMAGE playerFirst;
	IMAGE dualPlayer;
	IMAGE dataRead;
	IMAGE gameRule;
	IMAGE about;
	IMAGE AILevel;
	IMAGE easyAI;
	IMAGE normalAI;
	IMAGE hardAI;
	IMAGE bottomAdornment;
	IMAGE rightAdornment;
	// 顶部标题
	IMAGE titleImage;
	// 记录读取
	IMAGE statusNormal;
	IMAGE statusError;
	IMAGE statusBroken;
	IMAGE firstLocation;
	IMAGE secondLocation;
	IMAGE thirdLocation;
	IMAGE fourthLocation;
	IMAGE fifthLocation;
	IMAGE cancelDataRead;

	// 游戏界面
	IMAGE whiteGo;
	IMAGE blackGo;
	IMAGE whiteWin;
	IMAGE blackWin;
	IMAGE AIGo;
	IMAGE playerGo;
	IMAGE AIWin;
	IMAGE playerWin;
	IMAGE tie;
	IMAGE regret;
	IMAGE saveFile;
	IMAGE returnToMainMenu;
	IMAGE confirmReturn;
	IMAGE cancelSave;
	IMAGE black;
	IMAGE white;

	// 帮助页面
	IMAGE nextPage;
	IMAGE returnBack;
	IMAGE clickToGo;
	IMAGE goIndicator;
	IMAGE whiteGoesFirst;
	IMAGE yellowToJump;
	IMAGE greenToCopy;
	IMAGE yellowToJump_text;
	IMAGE greenToCopy_text;
	IMAGE copyToAssimilate;
	IMAGE jumpToAssimilate;
	IMAGE copyAssimilationComplete;
	IMAGE jumpAssimilationComplete;
	IMAGE right;
	IMAGE rightUp;
	IMAGE rightDown;
	IMAGE neighborAssimilate_greaterWin;

	// 关于页面
	IMAGE aboutPage;

	// 主菜单
	// 按键文字
	char AIFirst_Address[300];
	char playerFirst_Address[300];
	char dualPlayer_Address[300];
	char dataRead_Address[300];
	char gameRule_Address[300];
	char about_Address[300];
	char logo_Address[300];
	char AILevel_Address[300];
	char easyAI_Address[300];
	char normalAI_Address[300];
	char hardAI_Address[300];
	char bottomAdornment_Address[300];
	char rightAdornment_Address[300];
	// 顶部标题
	char titleImage_Address[300];
	// 记录读取
	char statusNormal_Address[300];
	char statusError_Address[300];
	char statusBroken_Address[300];
	char firstLocation_Address[300];
	char secondLocation_Address[300];
	char thirdLocation_Address[300];
	char fourthLocation_Address[300];
	char fifthLocation_Address[300];
	char cancelDataRead_Address[300];

	// 游戏界面
	char whiteGo_Address[300];
	char blackGo_Address[300];
	char whiteWin_Address[300];
	char blackWin_Address[300];
	char AIGo_Address[300];
	char playerGo_Address[300];
	char AIWin_Address[300];
	char playerWin_Address[300];
	char tie_Address[300];
	char regret_Address[300];
	char saveFile_Address[300];
	char returnToMainMenu_Address[300];
	char confirmReturn_Address[300];
	char cancelSave_Address[300];
	char white_Address[300];
	char black_Address[300];

	// 帮助页面
	char nextPage_Address[300];
	char returnBack_Address[300];
	char clickToGo_Address[300];
	char goIndicator_Address[300];
	char whiteGoesFirst_Address[300];
	char yellowToJump_Address[300];
	char greenToCopy_Address[300];
	char yellowToJump_text_Address[300];
	char greenToCopy_text_Address[300];
	char copyToAssimilate_Address[300];
	char jumpToAssimilate_Address[300];
	char copyAssimilationComplete_Address[300];
	char jumpAssimilationComplete_Address[300];
	char right_Address[300];
	char rightUp_Address[300];
	char rightDown_Address[300];
	char neighborAssimilate_greaterWin_Address[300];

	// 关于页面
	char aboutPage_Address[300];

	// 基本地址
	char basicAddress[300];

	// ################
	// #    函数区    #
	// ################

	// 将鼠标点击向量的原始值转换为棋子移动向量
	bool StandarizeVector(moveVector &vec)
	{
		vec.startHori = (vec.startHori - 40) / 51 + 1;
		vec.endHori = (vec.endHori - 40) / 51 + 1;
		vec.startVert = (vec.startVert - 50) / 51 + 1;
		vec.endVert = (vec.endVert - 50) / 51 + 1;
		if (vec.endHori - vec.startHori > 2 || vec.endVert - vec.startVert > 2)
			return false;
		else
			return true;
	}

	// 生成绘制队列
	void GenerateDrawQueue(ChessBoard *toBoard)
	{
		char toDrawBoard[11][11];
		memcpy(toDrawBoard, toBoard->GetBoard(), sizeof(toDrawBoard));

		for (int i = 2; i <= 8; i++)
		{
			for (int j = 2; j <= 8; j++)
			{
				if (toDrawBoard[i][j] != drawedBoard[i][j])
				{
					/*
						0 白->空
						1 黑->空
						2 白->黑
						3 黑->白
						4 空->白
						5 空->黑
						6 空->黄
						7 黄->空
						8 空->绿
						9 绿->空
					**/
					if (drawedBoard[i][j] == 'O' && toDrawBoard[i][j] == ' ')
						queue.push_back({ i, j, 0 });
					else if (drawedBoard[i][j] == '#' && toDrawBoard[i][j] == ' ')
						queue.push_back({ i, j, 1 });
					else if (drawedBoard[i][j] == 'O' && toDrawBoard[i][j] == '#')
						queue.push_back({ i, j, 2 });
					else if (drawedBoard[i][j] == '#' && toDrawBoard[i][j] == 'O')
						queue.push_back({ i, j, 3 });
					else if (drawedBoard[i][j] == ' ' && toDrawBoard[i][j] == 'O')
						queue.push_back({ i, j, 4 });
					else if (drawedBoard[i][j] == ' ' && toDrawBoard[i][j] == '#')
						queue.push_back({ i, j, 5 });
				}
			}
		}
	}

	// 绘制白棋
	void DrawWhitePiece(int row, int colomn, bool isXOR)
	{
		setlinecolor(BLACK);
		setfillcolor(WHITE);
		setlinestyle(PS_SOLID, 2);
		if (isXOR)
			setrop2(R2_XORPEN);
		else
			setrop2(R2_COPYPEN);
		circle(75 + (colomn - 2) * 50, 65 + (row - 2) * 50, 20);
	}

	// 绘制黑棋
	void DrawBlackPiece(int row, int colomn, bool isXOR)
	{
		setlinecolor(BLACK);
		setfillcolor(BLACK);
		setlinestyle(PS_SOLID, 2);
		if (isXOR)
			setrop2(R2_XORPEN);
		else
			setrop2(R2_COPYPEN);
		solidcircle(75 + (colomn - 2) * 50, 65 + (row - 2) * 50, 20);
	}

	// 绘制空白
	void DrawEmptySpace(int row, int colomn, bool isXOR)
	{
		setlinecolor(WHITE);
		setfillcolor(WHITE);
		setlinestyle(PS_SOLID, 2);
		if (isXOR)
			setrop2(R2_XORPEN);
		else
			setrop2(R2_COPYPEN);
		solidcircle(75 + (colomn - 2) * 50, 65 + (row - 2) * 50, 22);
	}

	// 绘制黄色落子位置
	void DrawYellowPiece(int row, int colomn, bool isXOR)
	{
		setlinecolor(YELLOW);
		setfillcolor(YELLOW);
		setlinestyle(PS_SOLID, 2);
		if (isXOR)
			setrop2(R2_XORPEN);
		else
			setrop2(R2_COPYPEN);
		solidcircle(75 + (colomn - 2) * 50, 65 + (row - 2) * 50, 20);
	}

	// 绘制绿色落子位置
	void DrawGreenPiece(int row, int colomn, bool isXOR)
	{
		setlinecolor(LIGHTGREEN);
		setfillcolor(LIGHTGREEN);
		setlinestyle(PS_SOLID, 2);
		if (isXOR)
			setrop2(R2_XORPEN);
		else
			setrop2(R2_COPYPEN);
		solidcircle(75 + (colomn - 2) * 50, 65 + (row - 2) * 50, 20);
	}

	// 绘制红十字叉线
	void DrawRedCross(int row, int colomn, bool isXOR)
	{
		// 黄色可行落子位置
		if (drawedBoard[row][colomn] == 'Y')
		{
			setlinecolor(LIGHTGREEN);
			setlinestyle(PS_SOLID, 1);
			if (isXOR)
				setrop2(R2_XORPEN);
			else
				setrop2(R2_COPYPEN);
			line(70 + (colomn - 2) * 50, 65 + (row - 2) * 50, 80 + (colomn - 2) * 50, 65 + (row - 2) * 50);
			line(75 + (colomn - 2) * 50, 60 + (row - 2) * 50, 75 + (colomn - 2) * 50, 70 + (row - 2) * 50);
		}
		// 绿色可行落子位置
		else if (drawedBoard[row][colomn] == 'G')
		{
			setlinecolor(0x55FFAA);
			setlinestyle(PS_SOLID, 1);
			if (isXOR)
				setrop2(R2_XORPEN);
			else
				setrop2(R2_COPYPEN);
			line(70 + (colomn - 2) * 50, 65 + (row - 2) * 50, 80 + (colomn - 2) * 50, 65 + (row - 2) * 50);
			line(75 + (colomn - 2) * 50, 60 + (row - 2) * 50, 75 + (colomn - 2) * 50, 70 + (row - 2) * 50);
		}
		// 白色可行落子位置
		else if (drawedBoard[row][colomn] == 'O')
		{
			setlinecolor(CYAN);
			setlinestyle(PS_SOLID, 1);
			if (isXOR)
				setrop2(R2_XORPEN);
			else
				setrop2(R2_COPYPEN);
			line(70 + (colomn - 2) * 50, 65 + (row - 2) * 50, 80 + (colomn - 2) * 50, 65 + (row - 2) * 50);
			line(75 + (colomn - 2) * 50, 60 + (row - 2) * 50, 75 + (colomn - 2) * 50, 70 + (row - 2) * 50);
		}
		// 黑色可行落子位置
		else if (drawedBoard[row][colomn] == '#')
		{
			setlinecolor(RED);
			setlinestyle(PS_SOLID, 1);
			if (isXOR)
				setrop2(R2_XORPEN);
			else
				setrop2(R2_COPYPEN);
			line(70 + (colomn - 2) * 50, 65 + (row - 2) * 50, 80 + (colomn - 2) * 50, 65 + (row - 2) * 50);
			line(75 + (colomn - 2) * 50, 60 + (row - 2) * 50, 75 + (colomn - 2) * 50, 70 + (row - 2) * 50);
		}
	}

	// 处理绘图队列
	void ProcessDrawQueue()
	{
		for (; queue.size() != 0;)
		{
			/*
				0 白->空
				1 黑->空
				2 白->黑
				3 黑->白
				4 空->白
				5 空->黑
				6 空->黄
				7 黄->空
				8 空->绿
				9 绿->空
			**/
			switch (queue[queue.size() - 1].drawType)
			{
			case 0:
				DrawEmptySpace(queue[queue.size() - 1].rowNum, queue[queue.size() - 1].colomnNum, false);
				break;
			case 1:
				DrawEmptySpace(queue[queue.size() - 1].rowNum, queue[queue.size() - 1].colomnNum, false);
				break;
			case 2:
				DrawEmptySpace(queue[queue.size() - 1].rowNum, queue[queue.size() - 1].colomnNum, false);
				DrawBlackPiece(queue[queue.size() - 1].rowNum, queue[queue.size() - 1].colomnNum, false);
				break;
			case 3:
				DrawEmptySpace(queue[queue.size() - 1].rowNum, queue[queue.size() - 1].colomnNum, false);
				DrawWhitePiece(queue[queue.size() - 1].rowNum, queue[queue.size() - 1].colomnNum, false);
				break;
			case 4:
				DrawWhitePiece(queue[queue.size() - 1].rowNum, queue[queue.size() - 1].colomnNum, false);
				break;
			case 5:
				DrawBlackPiece(queue[queue.size() - 1].rowNum, queue[queue.size() - 1].colomnNum, false);
				break;
			case 6:
				DrawYellowPiece(queue[queue.size() - 1].rowNum, queue[queue.size() - 1].colomnNum, false);
				break;
			case 7:
				DrawEmptySpace(queue[queue.size() - 1].rowNum, queue[queue.size() - 1].colomnNum, false);
				break;
			case 8:
				DrawGreenPiece(queue[queue.size() - 1].rowNum, queue[queue.size() - 1].colomnNum, false);
				break;
			case 9:
				DrawEmptySpace(queue[queue.size() - 1].rowNum, queue[queue.size() - 1].colomnNum, false);
				break;
			}
			queue.pop_back();
		}
	}

	// 检测第一次棋子点击是否有效
	bool IsValidFirstChessClick(int x, int y, ChessBoard *toBoard)
	{
		x = (x - 50) / 50 + 2;
		y = (y - 40) / 50 + 2;
		if (drawedBoard[y][x] == (toBoard->GetPlayer() == 0 ? 'O' : '#'))
		{
			for (int i = -2; i <= 2; i++)
				for (int j = -2; j <= 2; j++)
					if (drawedBoard[y + i][x + j] == ' ')
						return true;
			return false;
		}
		else
			return false;
	}

	// 生成可行区域绘制队列
	void GenerateSuggestionQueue(int x, int y, ChessBoard *toBoard, bool erase)
	{
		int colomn = (x - 50) / 50 + 2;
		int row = (y - 40) / 50 + 2;

		char toDrawBoard[11][11];
		memcpy(toDrawBoard, toBoard->GetBoard(), sizeof(toDrawBoard));

		/*
			0 白->空
			1 黑->空
			2 白->黑
			3 黑->白
			4 空->白
			5 空->黑
			6 空->黄
			7 黄->空
		**/

		if (erase)
		{
			for (int i = -2; i <= 2; i++)
			{
				for (int j = -2; j <= 2; j++)
				{
					if (drawedBoard[i + row][j + colomn] == 'Y' || drawedBoard[i + row][j + colomn] == 'G')
					{
						queue.push_back({ row + i, colomn + j, ((i != -2 && i != 2 && j != -2 && j != 2) ? 9 : 7) });
						drawedBoard[row + i][colomn + j] = ' ';
					}
				}
			}
		}
		else
		{
			for (int i = -2; i <= 2; i++)
			{
				for (int j = -2; j <= 2; j++)
				{
					if (drawedBoard[i + row][j + colomn] == ' ')
					{
						queue.push_back({ row + i, colomn + j, ((i != -2 && i != 2 && j != -2 && j != 2) ? 8 : 6) });
						drawedBoard[row + i][colomn + j] = ((i != -2 && i != 2 && j != -2 && j != 2) ? 'G' : 'Y');
					}
				}
			}
		}
	}

	// 动态追踪绘制下一步落子位置红十字
	void DynamicDrawNextStepMouse(int x, int y)
	{
		int colomn = (x - 50) / 50 + 2;
		int row = (y - 40) / 50 + 2;

		if (drawedBoard[row][colomn] == 'Y' || drawedBoard[row][colomn] == 'G')
		{
			// 已绘制红十字位置与当前位置不同
			if (dynamicRow != row || dynamicColomn != colomn)
			{

				DrawRedCross(dynamicRow, dynamicColomn, true); // 取消原先绘制				
				DrawRedCross(row, colomn, true); // 进行当前绘制

				// 更新记录坐标
				dynamicRow = row;
				dynamicColomn = colomn;
			}
		}
		else
		{
			DrawRedCross(dynamicRow, dynamicColomn, true); // 取消原先绘制

			// 更新记录坐标
			dynamicRow = -1;
			dynamicColomn = -1;
		}
	}

	// 动态追踪绘制鼠标悬停位置棋子红十字
	void DynamicDrawChessPointMouse(int x, int y, ChessBoard *toBoard)
	{
		bool flag = false;

		int colomn = (x - 50) / 50 + 2;
		int row = (y - 40) / 50 + 2;

		int switcher = toBoard->GetPlayer();

		if (drawedBoard[row][colomn] == (switcher == 0 ? 'O' : '#'))
		{
			// 已绘制红十字位置与当前位置不同
			if (dynamicRow != row || dynamicColomn != colomn)
			{

				DrawRedCross(dynamicRow, dynamicColomn, true); // 取消原先绘制

				// 检测是否可行
				for (int i = -2; i <= 2; i++)
				{
					for (int j = -2; j <= 2; j++)
					{
						if (drawedBoard[row + i][colomn + j] == ' ')
						{
							DrawRedCross(row, colomn, true);
							flag = true;
							
							// 更新记录坐标
							dynamicRow = row;
							dynamicColomn = colomn;
							break;
						}
					}
					if (flag)
						break;
				}

				// 更新记录坐标
				if (!flag)
				{
					dynamicRow = -1;
					dynamicColomn = -1;
				}
			}
		}
		else
		{
			DrawRedCross(dynamicRow, dynamicColomn, true); // 取消原先绘制

														   // 更新记录坐标
			dynamicRow = -1;
			dynamicColomn = -1;
		}
	}

	// 动态绘制主菜单鼠标所处按键标志
	void DynamicMainMenuMousePositionDraw(int drawPosition, bool erase)
	{
		if (erase)
			setrop2(R2_WHITE);
		else
			setrop2(R2_COPYPEN);
		setlinecolor(BLACK);
		setfillcolor(BLACK);
		setlinestyle(PS_SOLID);
		switch (drawPosition)
		{
		case 0:
			fillcircle(138, 263, 3);
			break;
		case 1:
			fillcircle(138, 323, 3);
			break;
		case 2:
			fillcircle(138, 383, 3);
			break;
		case 3:
			fillcircle(168, 216, 3);
			break;
		case 4:
			fillcircle(168, 336, 3);
			break;
		case 5:
			fillcircle(600, 216, 3);
			break;
		case 6:
			fillcircle(600, 336, 3);
			break;
		case 7:
			fillcircle(590, 472, 3);
			break;
		case 8:
			fillcircle(665, 472, 3);
			break;
		}
		setrop2(R2_COPYPEN);
	}

	// 动态追踪主菜单鼠标所处按键
	void DynamicMainMenuMousePositionCheck(int x, int y)
	{
		/*
			mainMenuDynamicTrackNum值定义
			-1：无
			0 ：简单
			1 ：中等
			2 ：困难
			3 ：AI先手
			4 ：双人对战
			5 ：玩家先手
			6 ：记录读取
			7 ：规则
			8 ：关于
		*/

		int tempTrackNum = -1;

		if (x >= 33 && x < 128)
		{
			if (y >= 235 && y < 290)
				tempTrackNum = 0;
			else if (y >= 295 && y < 350)
				tempTrackNum = 1;
			else if (y >= 355 && y < 410)
				tempTrackNum = 2;
			else
				tempTrackNum = -1;
		}
		else if (x >= 180 && x < 350)
		{
			if (y >= 180 && y < 250)
				tempTrackNum = 3;
			else if (y >= 300 && y < 370)
				tempTrackNum = 4;
			else
				tempTrackNum = -1;
		}
		else if (x >= 418 && x < 588 && y >= 180 && y < 250)
			tempTrackNum = 5;
		else if (x >= 418 && x < 588 && y >= 300 && y < 370)
			tempTrackNum = 6;
		else if (x >= 555 && x < 620 && y >= 425 && y < 465)
			tempTrackNum = 7;
		else if (x >= 630 && x < 695 && y >= 425 && y < 465)
			tempTrackNum = 8;
		else
			tempTrackNum = -1;



		if (mainMenuDynamicTrackNum != tempTrackNum)
		{
			if (mainMenuDynamicTrackNum != -1)
				DynamicMainMenuMousePositionDraw(mainMenuDynamicTrackNum, true);
			DynamicMainMenuMousePositionDraw(tempTrackNum, false);
			mainMenuDynamicTrackNum = tempTrackNum;
		}

	}

	// 动态绘制记录读取鼠标所处按键标志
	void DynamicReadDataMousePositionDraw(int drawPosition, bool erase)
	{
		if (erase)
			setrop2(R2_WHITE);
		else
			setrop2(R2_COPYPEN);
		setlinecolor(BLACK);
		setfillcolor(BLACK);
		setlinestyle(PS_SOLID);
		switch (drawPosition)
		{
		case 0:
			fillcircle(170, 205, 3);
			break;
		case 1:
			fillcircle(598, 205, 3);
			break;
		case 2:
			fillcircle(170, 285, 3);
			break;
		case 3:
			fillcircle(598, 285, 3);
			break;
		case 4:
			fillcircle(170, 365, 3);
			break;
		case 5:
			fillcircle(598, 365, 3);
			break;
		}
		setrop2(R2_COPYPEN);
	}

	// 动态追踪记录读取鼠标所处按键
	void DynamicReadDataMousePositionCheck(int x, int y)
	{
		/*
		readDataDynamicTrackNum值定义
		-1：无
		0 ：1号位
		1 ：2号位
		2 ：3号位
		3 ：4号位
		4 ：5号位
		5 ：取消
		*/

		int tempTrackNum = -1;

		if (x >= 180 && x < 380)
		{
			if (y >= 180 && y < 230)
				tempTrackNum = 0;
			else if (y >= 260 && y < 310)
				tempTrackNum = 2;
			else if (y >= 340 && y < 390)
				tempTrackNum = 4;
			else
				tempTrackNum = -1;
		}
		else if (x >= 388 && x < 588)
		{
			if (y >= 180 && y < 230)
				tempTrackNum = 1;
			else if (y >= 260 && y < 310)
				tempTrackNum = 3;
			else if (y >= 340 && y < 390)
				tempTrackNum = 5;
			else
				tempTrackNum = -1;
		}
		else
			tempTrackNum = -1;

		if (readDataDynamicTrackNum != tempTrackNum)
		{
			if (readDataDynamicTrackNum != -1)
				DynamicReadDataMousePositionDraw(readDataDynamicTrackNum, true);
			DynamicReadDataMousePositionDraw(tempTrackNum, false);
			readDataDynamicTrackNum = tempTrackNum;
		}
	}

	// 动态绘制记录保存鼠标所处按键标志
	void DynamicSaveDataMousePositionDraw(int drawPosition, bool erase)
	{
		if (erase)
			setrop2(R2_WHITE);
		else
			setrop2(R2_COPYPEN);
		setlinecolor(BLACK);
		setfillcolor(BLACK);
		setlinestyle(PS_SOLID);
		switch (drawPosition)
		{
		case 0:
			fillcircle(450, 145, 3);
			break;
		case 1:
			fillcircle(718, 145, 3);
			break;
		case 2:
			fillcircle(450, 205, 3);
			break;
		case 3:
			fillcircle(718, 205, 3);
			break;
		case 4:
			fillcircle(450, 265, 3);
			break;
		case 5:
			fillcircle(718, 265, 3);
			break;
		}
		setrop2(R2_COPYPEN);
	}
	
	// 动态追踪记录保存鼠标所处按键
	void DynamicSaveDataMousePositionCheck(int x, int y)
	{
		/*
		saveDataDynamicTrackNum值定义
		-1：无
		0 ：1号位
		1 ：2号位
		2 ：3号位
		3 ：4号位
		4 ：5号位
		5 ：取消
		*/

		int tempTrackNum = -1;

		if (x >= 460 && x < 560)
		{
			if (y >= 120 && y < 170)
				tempTrackNum = 0;
			else if (y >= 180 && y < 230)
				tempTrackNum = 2;
			else if (y >= 240 && y < 290)
				tempTrackNum = 4;
			else
				tempTrackNum = -1;
		}
		else if (x >= 608 && x < 708)
		{
			if (y >= 120 && y < 170)
				tempTrackNum = 1;
			else if (y >= 180 && y < 230)
				tempTrackNum = 3;
			else if (y >= 240 && y < 290)
				tempTrackNum = 5;
			else
				tempTrackNum = -1;
		}
		else
			tempTrackNum = -1;

		if (saveDataDynamicTrackNum != tempTrackNum)
		{
			if (saveDataDynamicTrackNum != -1)
				DynamicSaveDataMousePositionDraw(saveDataDynamicTrackNum, true);
			DynamicSaveDataMousePositionDraw(tempTrackNum, false);
			saveDataDynamicTrackNum = tempTrackNum;
		}
	}

	// 动态绘制记录保存鼠标所处按键标志
	void DynamicRuleAndAboutMousePositionDraw(int drawPosition, bool erase)
	{
		if (erase)
			setrop2(R2_WHITE);
		else
			setrop2(R2_COPYPEN);
		setlinecolor(BLACK);
		setfillcolor(BLACK);
		setlinestyle(PS_SOLID);
		switch (drawPosition)
		{
		case 0:
			fillcircle(632, 472, 3);
			break;
		}
		setrop2(R2_COPYPEN);
	}

	// 动态追踪规则、关于页面鼠标所处按键
	void DynamicRuleAndAboutMousePositionCheck(int x, int y)
	{
		/*
		saveDataDynamicTrackNum值定义
		-1：无
		0 ：按键
		*/

		int tempTrackNum = -1;

		if (x >= 600 && x < 665)
		{
			if (y >= 425 && y < 465)
				tempTrackNum = 0;
			else
				tempTrackNum = -1;
		}
		else
			tempTrackNum = -1;

		if (ruleAndAboutDynamicTrackNum != tempTrackNum)
		{
			if (ruleAndAboutDynamicTrackNum != -1)
				DynamicRuleAndAboutMousePositionDraw(ruleAndAboutDynamicTrackNum, true);
			DynamicRuleAndAboutMousePositionDraw(tempTrackNum, false);
			ruleAndAboutDynamicTrackNum = tempTrackNum;
		}
	}

	// 动态绘制游戏界面鼠标所处按键标志
	void DynamicPlayBoardMousePositionDraw(int drawPosition, bool erase)
	{
		if (erase)
			setrop2(R2_WHITE);
		else
			setrop2(R2_COPYPEN);
		setlinecolor(BLACK);
		setfillcolor(BLACK);
		setlinestyle(PS_SOLID);
		switch (drawPosition)
		{
		case 0:
			fillcircle(465, 165, 3);
			break;
		case 1:
			fillcircle(703, 165, 3);
			break;
		case 2:
			fillcircle(465, 235, 3);
			break;
		}
		setrop2(R2_COPYPEN);
	}

	// 动态追踪游戏界面鼠标
	void DynamicPlayBoardMousePositionCheck(int x, int y, ChessBoard *toBoard, bool checkChess)
	{
		/*
		playBoardDynamicTrackNum值定义
		-1：无
		0 ：悔棋
		1 ：保存
		2 ：返回主菜单
		*/

		int tempTrackNum = -1;

		if (y >= 140 && y < 190)
		{
			if (x >= 475 && x < 565)
				tempTrackNum = 0;
			else if (x >= 603 && x < 693)
				tempTrackNum = 1;
			else
				tempTrackNum = -1;
		}
		else if (y >= 210 && y < 260)
		{
			if (x >= 475 && x < 693)
				tempTrackNum = 2;
			else
				tempTrackNum = -1;
		}
		else
			tempTrackNum = -1;

		if (checkChess)
		{			
			if (x >= 50 && x < 400)
				if (y >= 40 && y < 390)
					DynamicDrawChessPointMouse(x, y, toBoard);
		}

		if (playBoardDynamicTrackNum != tempTrackNum)
		{
			if (playBoardDynamicTrackNum != -1)
				DynamicPlayBoardMousePositionDraw(playBoardDynamicTrackNum, true);
			DynamicPlayBoardMousePositionDraw(tempTrackNum, false);
			playBoardDynamicTrackNum = tempTrackNum;
		}
	}

	// 主菜单Logo旋转加速
	void MainMenuLogoRotateSpeedUp(int x, int y)
	{
		if ((x - 90) * (x - 90) + (y - 85) * (y - 85) <= 4225)
			mainMenuLogoRotateSpeed = 2;
		else
		{
			mainMenuLogoRotateSpeed = 1;
			return;
		}
	}

	// 游戏界面Logo旋转加速
	void PlayBoardLogoRotateSpeedUp(int x, int y)
	{
		if ((x - 580) * (x - 580) + (y - 375) * (y - 375) <= 4225)
			playBoardLogoRotateSpeed = 2;
		else
		{
			playBoardLogoRotateSpeed = 1;
			return;
		}
	}

	// 右下角按键显示状态更改
	void RightBottomButtonShowStatusChange(bool erase)
	{
		if (erase)
		{
			// 右下区域清空
			fillrectangle(550, 420, 700, 470);
		}
		else
		{
			// 绘制右下角选项
			setlinecolor(BLACK);
			setlinestyle(PS_SOLID, 2);
			roundrect(630, 425, 695, 465, 10, 10);
			setlinecolor(BLACK);
			setlinestyle(PS_SOLID, 2);
			roundrect(555, 425, 620, 465, 10, 10);
			putimage(635, 430, &about, SRCCOPY);
			putimage(560, 430, &gameRule, SRCCOPY);
		}
	}

	// 四个主菜单中心按键显示状态更改
	void MainMenuMainButtonShowStatusChange(bool erase)
	{
		if (erase)
		{
			// 中心区域清空
			setlinecolor(WHITE);
			setfillcolor(WHITE);
			fillrectangle(174, 175, 593, 375);
		}
		else
		{
			// 绘制四个主按钮
			setlinecolor(BLACK);
			setlinestyle(PS_SOLID, 2);
			roundrect(180, 180, 350, 250, 20, 20); // 左上
			roundrect(418, 180, 588, 250, 20, 20); // 右上
			roundrect(180, 300, 350, 370, 20, 20); // 左下
			roundrect(418, 300, 588, 370, 20, 20); // 右下
			putimage(190, 185, &AIFirst, SRCCOPY);
			putimage(428, 185, &playerFirst, SRCCOPY);
			putimage(190, 305, &dualPlayer, SRCCOPY);
			putimage(428, 305, &dataRead, SRCCOPY);
		}
	}

	// 难度选择按键显示状态更改
	void AILevelButtonShowStatusChange(bool erase)
	{
		if (erase)
		{
			// 左侧区域清空
			setlinecolor(WHITE);
			setfillcolor(WHITE);
			fillrectangle(18, 172, 148, 418);
		}
		else
		{
			// 绘制难度选择按钮
			setlinecolor(BLACK);
			if (easyAIOn)
				setlinestyle(PS_SOLID, 2);
			else
				setlinestyle(PS_DASHDOT, 2);
			roundrect(33, 235, 128, 290, 12, 12); // 简单
			if (normalAIOn)
				setlinestyle(PS_SOLID, 2);
			else
				setlinestyle(PS_DASHDOT, 2);
			roundrect(33, 295, 128, 350, 12, 12); // 中等
			if (hardAIOn)
				setlinestyle(PS_SOLID, 2);
			else
				setlinestyle(PS_DASHDOT, 2);
			roundrect(33, 355, 128, 410, 12, 12); // 困难
			putimage(30, 165, &AILevel, SRCCOPY);
			putimage(40, 240, &easyAI, SRCCOPY);
			putimage(40, 300, &normalAI, SRCCOPY);
			putimage(40, 360, &hardAI, SRCCOPY);
		}
	}

	// 装饰显示状态更改
	void AdornmentShowStatusChange(bool erase)
	{
		if (erase)
		{

		}
		else
		{
			// 绘制装饰
			putimage(205, 30, &titleImage, SRCCOPY);
			//putimage(30, 20, &logo, SRCCOPY);
			putimage(35, 425, &bottomAdornment, SRCCOPY);
			putimage(620, 40, &rightAdornment, SRCCOPY);
		}
	}

	// 右下角显示版本信息
	void RightBottomVersionShowStatusChange(bool erase)
	{
		if (erase)
		{

		}
		else
		{
			// 绘制版本信息
			settextcolor(BLACK);
			outtextxy(710, 465, "Ver 1.0.0");
		}
	}

	// 检测记录状态
	void CheckSaveDataStatus(ChessBoard *toBoard)
	{

		firstLocationIndicator = toBoard->TouchData(1);
		secondLocationIndicator = toBoard->TouchData(2);
		thirdLocationIndicator = toBoard->TouchData(3);
		fourthLocationIndicator = toBoard->TouchData(4);
		fifthLocationIndicator = toBoard->TouchData(5);
	}

	// 绘制记录选择按钮
	void DataLocationChooseButtonShowStatusChange(bool erase)
	{
		if (erase)
		{
			// 中心区域清空
			setlinecolor(WHITE);
			setfillcolor(WHITE);
			fillrectangle(175, 175, 593, 400);
		}
		else
		{
			setlinecolor(BLACK);
			setlinestyle(firstLocationIndicator == 1 ? PS_SOLID : PS_DASH, 2);
			roundrect(180, 180, 380, 230, 20, 20); // 1号位
			setlinestyle(secondLocationIndicator == 1 ? PS_SOLID : PS_DASH, 2);
			roundrect(388, 180, 588, 230, 20, 20); // 2号位
			setlinestyle(thirdLocationIndicator == 1 ? PS_SOLID : PS_DASH, 2);
			roundrect(180, 260, 380, 310, 20, 20); // 3号位
			setlinestyle(fourthLocationIndicator == 1 ? PS_SOLID : PS_DASH, 2);
			roundrect(388, 260, 588, 310, 20, 20); // 4号位
			setlinestyle(fifthLocationIndicator == 1 ? PS_SOLID : PS_DASH, 2);
			roundrect(180, 340, 380, 390, 20, 20); // 5号位
			setlinestyle(PS_SOLID, 2);
			roundrect(388, 340, 588, 390, 20, 20); // 取消按钮

												   // 绘制标语
												   // 1号位
			putimage(195, 185, &firstLocation, SRCCOPY);
			if (firstLocationIndicator == 1)
				putimage(305, 187, &statusNormal, SRCCOPY);
			else if (firstLocationIndicator == 2)
				putimage(305, 185, &statusError, SRCCOPY);
			else
				putimage(305, 187, &statusBroken, SRCCOPY);
			// 2号位
			putimage(403, 185, &secondLocation, SRCCOPY);
			if (secondLocationIndicator == 1)
				putimage(513, 187, &statusNormal, SRCCOPY);
			else if (secondLocationIndicator == 2)
				putimage(513, 185, &statusError, SRCCOPY);
			else
				putimage(513, 187, &statusBroken, SRCCOPY);
			// 3号位
			putimage(195, 265, &thirdLocation, SRCCOPY);
			if (thirdLocationIndicator == 1)
				putimage(305, 267, &statusNormal, SRCCOPY);
			else if (thirdLocationIndicator == 2)
				putimage(305, 265, &statusError, SRCCOPY);
			else
				putimage(305, 267, &statusBroken, SRCCOPY);
			// 4号位
			putimage(403, 265, &fourthLocation, SRCCOPY);
			if (fourthLocationIndicator == 1)
				putimage(513, 267, &statusNormal, SRCCOPY);
			else if (fourthLocationIndicator == 2)
				putimage(513, 265, &statusError, SRCCOPY);
			else
				putimage(513, 267, &statusBroken, SRCCOPY);
			// 5号位
			putimage(195, 345, &fifthLocation, SRCCOPY);
			if (fifthLocationIndicator == 1)
				putimage(305, 347, &statusNormal, SRCCOPY);
			else if (fifthLocationIndicator == 2)
				putimage(305, 345, &statusError, SRCCOPY);
			else
				putimage(305, 347, &statusBroken, SRCCOPY);
			// 取消读取
			putimage(400, 341, &cancelDataRead, SRCCOPY);
		}
	}

	// 读取主菜单图片素材
	void ReadMainMenuElements()
	{
		mainMenuImageRead = true;

		// 顶部标题
		strcpy_s(titleImage_Address, basicAddress);
		strcat_s(titleImage_Address, "elements\\titleImage.jpg");
		loadimage(&titleImage, titleImage_Address, 360, 90);

		// 按键文字
		strcpy_s(AIFirst_Address, basicAddress);
		strcat_s(AIFirst_Address, "elements\\AIFirst.JPG");
		loadimage(&AIFirst, AIFirst_Address, 150, 60);
		strcpy_s(playerFirst_Address, basicAddress);
		strcat_s(playerFirst_Address, "elements\\playerFirst.JPG");
		loadimage(&playerFirst, playerFirst_Address, 150, 60);
		strcpy_s(dualPlayer_Address, basicAddress);
		strcat_s(dualPlayer_Address, "elements\\dualPlayer.JPG");
		loadimage(&dualPlayer, dualPlayer_Address, 150, 60);
		strcpy_s(dataRead_Address, basicAddress);
		strcat_s(dataRead_Address, "elements\\dataRead.JPG");
		loadimage(&dataRead, dataRead_Address, 150, 60);
		strcpy_s(gameRule_Address, basicAddress);
		strcat_s(gameRule_Address, "elements\\gameRule.JPG");
		loadimage(&gameRule, gameRule_Address, 55, 30);
		strcpy_s(about_Address, basicAddress);
		strcat_s(about_Address, "elements\\about.JPG");
		loadimage(&about, about_Address, 55, 30);
		strcpy_s(logo_Address, basicAddress);
		strcat_s(logo_Address, "elements\\logo.JPG");
		loadimage(&logo, logo_Address, 130, 130);
		loadimage(&rotatedLogo, logo_Address, 130, 130);
		strcpy_s(AILevel_Address, basicAddress);
		strcat_s(AILevel_Address, "elements\\AILevel.JPG");
		loadimage(&AILevel, AILevel_Address, 120, 60);
		strcpy_s(easyAI_Address, basicAddress);
		strcat_s(easyAI_Address, "elements\\easyAI.JPG");
		loadimage(&easyAI, easyAI_Address, 85, 45);
		strcpy_s(normalAI_Address, basicAddress);
		strcat_s(normalAI_Address, "elements\\normalAI.JPG");
		loadimage(&normalAI, normalAI_Address, 85, 45);
		strcpy_s(hardAI_Address, basicAddress);
		strcat_s(hardAI_Address, "elements\\hardAI.JPG");
		loadimage(&hardAI, hardAI_Address, 85, 45);
		strcpy_s(bottomAdornment_Address, basicAddress);
		strcat_s(bottomAdornment_Address, "elements\\bottomAdornment.JPG");
		loadimage(&bottomAdornment, bottomAdornment_Address, 485, 40);
		strcpy_s(rightAdornment_Address, basicAddress);
		strcat_s(rightAdornment_Address, "elements\\rightAdornment.JPG");
		loadimage(&rightAdornment, rightAdornment_Address, 120, 350);
		strcpy_s(statusNormal_Address, basicAddress);
		strcat_s(statusNormal_Address, "elements\\statusNormal.JPG");
		loadimage(&statusNormal, statusNormal_Address, 70, 40);
		strcpy_s(statusError_Address, basicAddress);
		strcat_s(statusError_Address, "elements\\statusError.JPG");
		loadimage(&statusError, statusError_Address, 70, 40);
		strcpy_s(statusBroken_Address, basicAddress);
		strcat_s(statusBroken_Address, "elements\\statusBroken.JPG");
		loadimage(&statusBroken, statusBroken_Address, 70, 40);
		strcpy_s(firstLocation_Address, basicAddress);
		strcat_s(firstLocation_Address, "elements\\firstLocation.JPG");
		loadimage(&firstLocation, firstLocation_Address, 90, 40);
		strcpy_s(secondLocation_Address, basicAddress);
		strcat_s(secondLocation_Address, "elements\\secondLocation.JPG");
		loadimage(&secondLocation, secondLocation_Address, 90, 40);
		strcpy_s(thirdLocation_Address, basicAddress);
		strcat_s(thirdLocation_Address, "elements\\thirdLocation.JPG");
		loadimage(&thirdLocation, thirdLocation_Address, 90, 40);
		strcpy_s(fourthLocation_Address, basicAddress);
		strcat_s(fourthLocation_Address, "elements\\fourthLocation.JPG");
		loadimage(&fourthLocation, fourthLocation_Address, 90, 40);
		strcpy_s(fifthLocation_Address, basicAddress);
		strcat_s(fifthLocation_Address, "elements\\fifthLocation.JPG");
		loadimage(&fifthLocation, fifthLocation_Address, 90, 40);
		strcpy_s(cancelDataRead_Address, basicAddress);
		strcat_s(cancelDataRead_Address, "elements\\returnToMainMenu.JPG");
		loadimage(&cancelDataRead, cancelDataRead_Address, 180, 47);
		strcpy_s(aboutPage_Address, basicAddress);
		strcat_s(aboutPage_Address, "elements\\aboutPage.JPG");
		loadimage(&aboutPage, aboutPage_Address, 425, 300);
	}

	// 读取游戏界面图片素材
	void ReadPlayBoardElements()
	{
		playBoardImageRead = true;

		strcpy_s(whiteGo_Address, basicAddress);
		strcat_s(whiteGo_Address, "elements\\whiteGo.jpg");
		loadimage(&whiteGo, whiteGo_Address, 400, 100);
		strcpy_s(blackGo_Address, basicAddress);
		strcat_s(blackGo_Address, "elements\\blackGo.jpg");
		loadimage(&blackGo, blackGo_Address, 400, 100);
		strcpy_s(whiteWin_Address, basicAddress);
		strcat_s(whiteWin_Address, "elements\\whiteWin.jpg");
		loadimage(&whiteWin, whiteWin_Address, 400, 100);
		strcpy_s(blackWin_Address, basicAddress);
		strcat_s(blackWin_Address, "elements\\blackWin.jpg");
		loadimage(&blackWin, blackWin_Address, 400, 100);
		strcpy_s(AIGo_Address, basicAddress);
		strcat_s(AIGo_Address, "elements\\AIGo.jpg");
		loadimage(&AIGo, AIGo_Address, 400, 100);
		strcpy_s(playerGo_Address, basicAddress);
		strcat_s(playerGo_Address, "elements\\playerGo.jpg");
		loadimage(&playerGo, playerGo_Address, 400, 100);
		strcpy_s(AIWin_Address, basicAddress);
		strcat_s(AIWin_Address, "elements\\AIWin.jpg");
		loadimage(&AIWin, AIWin_Address, 400, 100);
		strcpy_s(playerWin_Address, basicAddress);
		strcat_s(playerWin_Address, "elements\\playerWin.jpg");
		loadimage(&playerWin, playerWin_Address, 400, 100);
		strcpy_s(tie_Address, basicAddress);
		strcat_s(tie_Address, "elements\\tie.jpg");
		loadimage(&tie, tie_Address, 400, 100);
		strcpy_s(regret_Address, basicAddress);
		strcat_s(regret_Address, "elements\\regret.jpg");
		loadimage(&regret, regret_Address, 75, 43);
		strcpy_s(saveFile_Address, basicAddress);
		strcat_s(saveFile_Address, "elements\\saveFile.jpg");
		loadimage(&saveFile, saveFile_Address, 75, 43);
		strcpy_s(returnToMainMenu_Address, basicAddress);
		strcat_s(returnToMainMenu_Address, "elements\\returnToMainMenu.jpg");
		loadimage(&returnToMainMenu, returnToMainMenu_Address, 188, 48);
		strcpy_s(confirmReturn_Address, basicAddress);
		strcat_s(confirmReturn_Address, "elements\\confirmReturn.jpg");
		loadimage(&confirmReturn, confirmReturn_Address, 160, 38);
		strcpy_s(cancelSave_Address, basicAddress);
		strcat_s(cancelSave_Address, "elements\\cancelSave.jpg");
		loadimage(&cancelSave, cancelSave_Address, 80, 42);
		strcpy_s(white_Address, basicAddress);
		strcat_s(white_Address, "elements\\white.jpg");
		loadimage(&white, white_Address, 65, 60);
		strcpy_s(black_Address, basicAddress);
		strcat_s(black_Address, "elements\\black.jpg");
		loadimage(&black, black_Address, 65, 65);
	}

	// 读取规则界面图片素材
	void ReadRuleElements()
	{
		ruleImageRead = true;

		strcpy_s(clickToGo_Address, basicAddress);
		strcat_s(clickToGo_Address, "elements\\clickToGo.jpg");
		loadimage(&clickToGo, clickToGo_Address, 320, 70);
		strcpy_s(whiteGoesFirst_Address, basicAddress);
		strcat_s(whiteGoesFirst_Address, "elements\\whiteGoesFirst.jpg");
		loadimage(&whiteGoesFirst, whiteGoesFirst_Address, 140, 35);
		strcpy_s(goIndicator_Address, basicAddress);
		strcat_s(goIndicator_Address, "elements\\goIndicator.jpg");
		loadimage(&goIndicator, goIndicator_Address, 110, 110);
		strcpy_s(greenToCopy_Address, basicAddress);
		strcat_s(greenToCopy_Address, "elements\\greenToCopy.jpg");
		loadimage(&greenToCopy, greenToCopy_Address, 110, 110);
		strcpy_s(yellowToJump_Address, basicAddress);
		strcat_s(yellowToJump_Address, "elements\\yellowToJump.jpg");
		loadimage(&yellowToJump, yellowToJump_Address, 110, 110);
		strcpy_s(greenToCopy_text_Address, basicAddress);
		strcat_s(greenToCopy_text_Address, "elements\\greenToCopy_text.jpg");
		loadimage(&greenToCopy_text, greenToCopy_text_Address, 140, 35);
		strcpy_s(yellowToJump_text_Address, basicAddress);
		strcat_s(yellowToJump_text_Address, "elements\\yellowToJump_text.jpg");
		loadimage(&yellowToJump_text, yellowToJump_text_Address, 140, 35);
		strcpy_s(copyToAssimilate_Address, basicAddress);
		strcat_s(copyToAssimilate_Address, "elements\\copyToAssimilate.jpg");
		loadimage(&copyToAssimilate, copyToAssimilate_Address, 150, 110);
		strcpy_s(jumpToAssimilate_Address, basicAddress);
		strcat_s(jumpToAssimilate_Address, "elements\\jumpToAssimilate.jpg");
		loadimage(&jumpToAssimilate, jumpToAssimilate_Address, 150, 110);
		strcpy_s(copyAssimilationComplete_Address, basicAddress);
		strcat_s(copyAssimilationComplete_Address, "elements\\copyAssimilationComplete.jpg");
		loadimage(&copyAssimilationComplete, copyAssimilationComplete_Address, 150, 110);
		strcpy_s(jumpAssimilationComplete_Address, basicAddress);
		strcat_s(jumpAssimilationComplete_Address, "elements\\jumpAssimilationComplete.jpg");
		loadimage(&jumpAssimilationComplete, jumpAssimilationComplete_Address, 150, 110);
		strcpy_s(nextPage_Address, basicAddress);
		strcat_s(nextPage_Address, "elements\\nextPage.jpg");
		loadimage(&nextPage, nextPage_Address, 55, 30);
		strcpy_s(returnBack_Address, basicAddress);
		strcat_s(returnBack_Address, "elements\\returnBack.jpg");
		loadimage(&returnBack, returnBack_Address, 55, 30);
		strcpy_s(rightUp_Address, basicAddress);
		strcat_s(rightUp_Address, "elements\\rightUp.jpg");
		loadimage(&rightUp, rightUp_Address, 50, 50);
		strcpy_s(rightDown_Address, basicAddress);
		strcat_s(rightDown_Address, "elements\\rightDown.jpg");
		loadimage(&rightDown, rightDown_Address, 50, 50);
		strcpy_s(right_Address, basicAddress);
		strcat_s(right_Address, "elements\\right.jpg");
		loadimage(&right, right_Address, 50, 30);
		strcpy_s(neighborAssimilate_greaterWin_Address, basicAddress);
		strcat_s(neighborAssimilate_greaterWin_Address, "elements\\neighborAssimilate_greaterWin.jpg");
		loadimage(&neighborAssimilate_greaterWin, neighborAssimilate_greaterWin_Address, 140, 85);
	}

	// 游戏界面右侧控制按键显示状态更改
	void PlayBoardContronButtonShowStatusChange(bool erase)
	{
		if (erase)
		{
			// 清空右侧按键区
			setlinecolor(WHITE);
			setfillcolor(WHITE);
			fillrectangle(435, 115, 750, 295);
		}
		else
		{
			// 绘制右侧按钮（保存、返回主菜单）
			setlinecolor(BLACK);
			setlinestyle(PS_SOLID, 2);
			roundrect(603, 140, 693, 190, 10, 10);
			putimage(610, 143, &saveFile, SRCCOPY);
			roundrect(475, 210, 693, 260, 10, 10);
			putimage(490, 211, &returnToMainMenu, SRCCOPY);
		}
	}

	// 悔棋按钮状态更改
	void RegretControlButtonShowStatusChange(bool erase)
	{
		if (erase)
		{

		}
		else
		{
			// 绘制悔棋按钮
			setlinecolor(WHITE);
			roundrect(475, 140, 565, 190, 10, 10);
			setlinecolor(BLACK);
			if (screenIsRegretOK)
				setlinestyle(PS_SOLID, 2);
			else
				setlinestyle(PS_DASH, 2);
			roundrect(475, 140, 565, 190, 10, 10);
			putimage(482, 143, &regret, SRCCOPY);
		}
	}

	// 游戏界面logo显示状态更改
	void PlayBoardLogoShowStatusChange(bool erase)
	{
		if (erase)
		{

		}
		else
		{
			// 绘制logo
			putimage(520, 310, &logo, SRCCOPY);
		}
	}

	// 游戏界面绘制行动指示图
	void PlayBoardDirectionShowStatusChange(bool erase, bool isAntiAI, ChessBoard *toBoard)
	{
		if (erase)
		{

		}
		else
		{
			if (isAntiAI)
				putimage(425, 25, &playerGo, SRCCOPY);
			else
			{
				if (toBoard->GetPlayer() == 0)
					putimage(425, 25, &whiteGo, SRCCOPY);
				else
					putimage(425, 25, &blackGo, SRCCOPY);
			}
		}
	}

	// 游戏界面返回主菜单显示状态更改
	void ReturnConfirmButtonShowStatusChange(bool erase)
	{
		if (erase)
		{
			putimage(490, 211, &returnToMainMenu, SRCCOPY);
		}
		else
		{
			// 更改为确认按键
			setlinecolor(BLACK);
			setlinestyle(PS_SOLID, 2);
			setfillcolor(WHITE);
			fillroundrect(475, 210, 693, 260, 10, 10);
			putimage(505, 217, &confirmReturn, SRCCOPY);
		}
	}

	// 游戏界面记录位按钮绘制
	void PlayBoardSaveDataButtonShowStatusChange(bool erase)
	{
		if (erase)
		{
			// 清空右侧按键区
			setlinecolor(WHITE);
			setfillcolor(WHITE);
			fillrectangle(435, 115, 750, 295);
		}
		else
		{
			// 绘制右侧按键区
			setlinecolor(BLACK);
			setlinestyle(PS_SOLID, 2);
			roundrect(460, 120, 560, 170, 12, 12);
			roundrect(608, 120, 708, 170, 12, 12);
			roundrect(460, 180, 560, 230, 12, 12);
			roundrect(608, 180, 708, 230, 12, 12);
			roundrect(460, 240, 560, 290, 12, 12);
			roundrect(608, 240, 708, 290, 12, 12);
			putimage(465, 125, &firstLocation, SRCCOPY);
			putimage(613, 125, &secondLocation, SRCCOPY);
			putimage(465, 185, &thirdLocation, SRCCOPY);
			putimage(613, 185, &fourthLocation, SRCCOPY);
			putimage(465, 245, &fifthLocation, SRCCOPY);
			putimage(617, 244, &cancelSave, SRCCOPY);
		}
	}

	// 规则页面右下按钮的显示状态更改
	void RulePageControlButtonShowStatusChange(bool erase, int pageNum)
	{
		if (erase)
		{
			setlinecolor(WHITE);
			setfillcolor(WHITE);
			fillrectangle(590, 420, 670, 470);
		}
		else
		{
			setlinecolor(WHITE);
			setfillcolor(WHITE);
			fillrectangle(590, 420, 670, 470);
			setlinecolor(BLACK);
			setlinestyle(PS_SOLID, 2);
			roundrect(600, 425, 665, 465, 10, 10);
			if (pageNum == 1)
				putimage(605, 430, &nextPage, SRCCOPY);
			else
				putimage(605, 430, &returnBack, SRCCOPY);
		}
	}

	// 显示规则
	void ShowRulePage(bool erase, int pageNum)
	{
		if (erase)
		{
			if (pageNum == 1)
			{
				setlinecolor(WHITE);
				setfillcolor(WHITE);
				fillrectangle(220, 120, 540, 190);
				fillrectangle(170, 245, 280, 355);
				fillrectangle(20, 285, 160, 320);
				fillrectangle(290, 230, 340, 280);
				fillrectangle(290, 325, 340, 375);
				fillrectangle(370, 180, 480, 290);
				fillrectangle(370, 310, 480, 420);
				fillrectangle(488, 220, 628, 255);
				fillrectangle(488, 350, 628, 385);
			}
			else
			{
				setlinecolor(WHITE);
				setfillcolor(WHITE);
				fillrectangle(20, 250, 160, 335);
				fillrectangle(170, 180, 320, 290);
				fillrectangle(170, 310, 320, 420);
				fillrectangle(330, 230, 380, 260);
				fillrectangle(330, 360, 380, 390);
				fillrectangle(390, 180, 540, 290);
				fillrectangle(390, 310, 540, 420);
			}
		}
		else
		{
			if (pageNum == 1)
			{
				putimage(220, 120, &clickToGo, SRCCOPY); // 单击以行走
				putimage(170, 245, &goIndicator, SRCCOPY); // 行走指示
				putimage(20, 285, &whiteGoesFirst, SRCCOPY); // 白色先手
				putimage(290, 230, &rightUp, SRCCOPY); // 右上
				putimage(290, 325, &rightDown, SRCCOPY); // 右下
				putimage(370, 180, &greenToCopy, SRCCOPY);
				putimage(370, 310, &yellowToJump, SRCCOPY);
				putimage(488, 220, &greenToCopy_text, SRCCOPY);
				putimage(488, 350, &yellowToJump_text, SRCCOPY);
			}
			else
			{
				putimage(20, 250, &neighborAssimilate_greaterWin, SRCCOPY);
				putimage(170, 180, &copyToAssimilate, SRCCOPY);
				putimage(170, 310, &jumpToAssimilate, SRCCOPY);
				putimage(330, 230, &right, SRCCOPY);
				putimage(330, 360, &right, SRCCOPY);
				putimage(390, 180, &copyAssimilationComplete, SRCCOPY);
				putimage(390, 310, &jumpAssimilationComplete, SRCCOPY);
			}
		}
	}

	// 显示规则页面
	void ShowRule()
	{
		// 读取图片素材
		if (!ruleImageRead)
			ReadRuleElements();

		RulePageControlButtonShowStatusChange(false, 1);

		ShowRulePage(false, 1);

		// 获取鼠标操作
		MOUSEMSG mouseMessage;

		while (true)
		{
			// 获取鼠标消息
			mouseMessage = GetMouseMsg();

			// #调试语句# 显示鼠标坐标
			//ShowMouseCoordinate(mouseMessage.x, mouseMessage.y);

			// 动态追踪鼠标位置
			DynamicRuleAndAboutMousePositionCheck(mouseMessage.x, mouseMessage.y);

			// Logo旋转加速
			MainMenuLogoRotateSpeedUp(mouseMessage.x, mouseMessage.y);

			// 对鼠标按下位置做判断
			if (mouseMessage.uMsg == WM_LBUTTONDOWN)
			{
				// 清除动态追踪图案
				DynamicRuleAndAboutMousePositionDraw(ruleAndAboutDynamicTrackNum, true);
				ruleAndAboutDynamicTrackNum = -1;

				if (mouseMessage.x >= 600 && mouseMessage.x < 665)
					if (mouseMessage.y >= 425 && mouseMessage.y < 465)
						break;
			}
		}

		RulePageControlButtonShowStatusChange(false, 2);

		ShowRulePage(true, 1);
		ShowRulePage(false, 2);

		while (true)
		{
			// 获取鼠标消息
			mouseMessage = GetMouseMsg();

			// #调试语句# 显示鼠标坐标
			//ShowMouseCoordinate(mouseMessage.x, mouseMessage.y);

			// 动态追踪鼠标位置
			DynamicRuleAndAboutMousePositionCheck(mouseMessage.x, mouseMessage.y);

			// Logo旋转加速
			MainMenuLogoRotateSpeedUp(mouseMessage.x, mouseMessage.y);

			// 对鼠标按下位置做判断
			if (mouseMessage.uMsg == WM_LBUTTONDOWN)
			{
				// 清除动态追踪图案
				DynamicRuleAndAboutMousePositionDraw(ruleAndAboutDynamicTrackNum, true);
				ruleAndAboutDynamicTrackNum = -1;

				if (mouseMessage.x >= 600 && mouseMessage.x < 665)
					if (mouseMessage.y >= 425 && mouseMessage.y < 465)
					{
						RulePageControlButtonShowStatusChange(true, 2);
						ShowRulePage(true, 2);
						return;
					}
			}
		}
	}

	// 显示关于
	void ShowAboutPage(bool erase)
	{
		if (erase)
		{
			setlinecolor(WHITE);
			setfillcolor(WHITE);
			fillrectangle(200, 110, 575, 420);
		}
		else
		{
			putimage(200, 110, &aboutPage, SRCCOPY);
		}
	}

	// 显示关于页面
	bool ShowAbout()
	{
		// 读取图片素材
		if (!ruleImageRead)
			ReadRuleElements();

		// 显示关于
		ShowAboutPage(false);

		// 显示右下角按钮
		RulePageControlButtonShowStatusChange(false, 2);

		// 获取鼠标操作
		MOUSEMSG mouseMessage;

		while (true)
		{
			// 获取鼠标消息
			mouseMessage = GetMouseMsg();

			// #调试语句# 显示鼠标坐标
			//ShowMouseCoordinate(mouseMessage.x, mouseMessage.y);

			// 动态追踪鼠标位置
			DynamicRuleAndAboutMousePositionCheck(mouseMessage.x, mouseMessage.y);

			// Logo旋转加速
			MainMenuLogoRotateSpeedUp(mouseMessage.x, mouseMessage.y);

			// 对鼠标按下位置做判断
			if (mouseMessage.uMsg == WM_LBUTTONDOWN)
			{
				// 清除动态追踪图案
				DynamicRuleAndAboutMousePositionDraw(ruleAndAboutDynamicTrackNum, true);
				ruleAndAboutDynamicTrackNum = -1;

				if (mouseMessage.x >= 600 && mouseMessage.x < 665)
					if (mouseMessage.y >= 425 && mouseMessage.y < 465)
					{
						ShowAboutPage(true);
						RulePageControlButtonShowStatusChange(true, 2);

						if (mouseMessage.mkCtrl)
							return 1;
						else
							return 0;
					}
			}
		}
	}
	

public:

	// ################
	// #    函数区    #
	// ################

	// 构造函数
	Screen(char* argv[])
	{
		// 存储地址更新
		strcpy_s(basicAddress, argv[0]);
		for (int i = strlen(basicAddress); i >= 0; i--)
		{
			if (basicAddress[i] == '\\')
			{
				basicAddress[i + 1] = '\0';
				break;
			}
		}
	}

	// 生成图形界面
	void InitiateGUI()
	{
		initgraph(768, 480);
		setbkcolor(WHITE);
		cleardevice();
	}

	// 清空图形界面
	void ClearGUI()
	{
		cleardevice();
	}

	// 关闭图形界面
	void CloseGUI()
	{
		closegraph();
	}

	// 显示主菜单
	int ShowGUIMainMenu(ChessBoard *toBoard)
	{
		/*
			返回值定义：
			0：程序结束
			1：单人游戏（玩家先手）
			2：单人游戏（AI先手）
			3：双人游戏
			4：读取记录
			8：显示帮助
			9：显示关于
		**/

		// 切换到CLI手动控制台
		bool gotoCLI;

		// 读取图片元素
		if (!mainMenuImageRead)
			ReadMainMenuElements();

		// 绘制四个主按钮
		MainMenuMainButtonShowStatusChange(false);

		// 绘制难度选择按钮
		AILevelButtonShowStatusChange(false);

		// 右下角选项
		RightBottomButtonShowStatusChange(false);

		// 绘制装饰
		AdornmentShowStatusChange(false);

		// 版本信息
		RightBottomVersionShowStatusChange(false);

		// Logo旋转
		HANDLE mainMenuLogoRotateThread;
		unsigned threadID;
		mainMenuLogoRotateSpeed = 1;
		mainMenuLogoRotateThread = (HANDLE)_beginthreadex(NULL, 0, &MainMenuLogoRotate, NULL, 0, &threadID);

		// 获取鼠标操作
		MOUSEMSG mouseMessage;
		MOUSEMSG secondMouseMessage;

		while (true)
		{
			// 获取鼠标消息
			mouseMessage = GetMouseMsg();

			// #调试语句# 显示鼠标坐标
			//ShowMouseCoordinate(mouseMessage.x, mouseMessage.y);

			// 鼠标动态追踪
			DynamicMainMenuMousePositionCheck(mouseMessage.x, mouseMessage.y);

			// Logo旋转加速
			MainMenuLogoRotateSpeedUp(mouseMessage.x, mouseMessage.y);

			// 对鼠标按下位置做判断
			if (mouseMessage.uMsg == WM_LBUTTONDOWN)
			{
				// 清除动态追踪图案
				DynamicMainMenuMousePositionDraw(mainMenuDynamicTrackNum, true);
				mainMenuDynamicTrackNum = -1;

				// 1、3按键
				if (mouseMessage.x >= 180 && mouseMessage.x <= 350)
				{
					// AI先手模式
					if (mouseMessage.y >= 180 && mouseMessage.y <= 250)
					{
						mainMenuLogoRotateSpeed = 0;
						WaitForSingleObjectEx(mainMenuLogoRotateThread, INFINITE, FALSE);
						return 1;
					}
					// 双人对战模式
					if (mouseMessage.y >= 300 && mouseMessage.y <= 370)
					{
						mainMenuLogoRotateSpeed = 0;
						WaitForSingleObjectEx(mainMenuLogoRotateThread, INFINITE, FALSE);
						return 3;
					}
				}
				// 2、4按键
				if (mouseMessage.x >= 418 && mouseMessage.x <= 588)
				{
					// 玩家先手模式
					if (mouseMessage.y >= 180 && mouseMessage.y <= 250)
					{
						mainMenuLogoRotateSpeed = 0;
						WaitForSingleObjectEx(mainMenuLogoRotateThread, INFINITE, FALSE);
						return 2;
					}
					// 记录读取
					if (mouseMessage.y >= 300 && mouseMessage.y <= 370)
					{
						// 中心区域清空
						MainMenuMainButtonShowStatusChange(true);

						// 左侧区域清空
						AILevelButtonShowStatusChange(true);

						// 右下区域清空
						RightBottomButtonShowStatusChange(true);

						// 检测记录状态
						CheckSaveDataStatus(toBoard);

						// 绘制记录选择按钮
						DataLocationChooseButtonShowStatusChange(false);

						while (true)
						{
							// 获取鼠标消息
							secondMouseMessage = GetMouseMsg();

							// #调试语句# 显示鼠标坐标
							//ShowMouseCoordinate(secondMouseMessage.x, secondMouseMessage.y);

							// 记录读取动态鼠标追踪
							DynamicReadDataMousePositionCheck(secondMouseMessage.x, secondMouseMessage.y);

							// Logo旋转加速
							MainMenuLogoRotateSpeedUp(secondMouseMessage.x, secondMouseMessage.y);

							// 对鼠标按下位置做判断
							if (secondMouseMessage.uMsg == WM_LBUTTONDOWN)
							{
								// 清除动态追踪图案
								DynamicReadDataMousePositionDraw(readDataDynamicTrackNum, true);
								readDataDynamicTrackNum = -1;

								if (secondMouseMessage.x >= 180 && secondMouseMessage.x < 380)
								{
									// 1号位
									if (secondMouseMessage.y >= 180 && secondMouseMessage.y < 230 && firstLocationIndicator == 1)
									{
										toBoard->ReadData(1);
										mainMenuLogoRotateSpeed = 0;
										WaitForSingleObjectEx(mainMenuLogoRotateThread, INFINITE, FALSE);
										return 4;
									}
									// 3号位
									else if (secondMouseMessage.y >= 260 && secondMouseMessage.y < 310 && thirdLocationIndicator == 1)
									{
										toBoard->ReadData(3);
										mainMenuLogoRotateSpeed = 0;
										WaitForSingleObjectEx(mainMenuLogoRotateThread, INFINITE, FALSE);
										return 4;
									}
									// 5号位
									else if (secondMouseMessage.y >= 340 && secondMouseMessage.y < 390 && fifthLocationIndicator == 1)
									{
										toBoard->ReadData(5);
										mainMenuLogoRotateSpeed = 0;
										WaitForSingleObjectEx(mainMenuLogoRotateThread, INFINITE, FALSE);
										return 4;
									}
								}
								else if (secondMouseMessage.x >= 388 && secondMouseMessage.y < 588)
								{
									// 2号位
									if (secondMouseMessage.y >= 180 && secondMouseMessage.y < 230 && secondLocationIndicator == 1)
									{
										toBoard->ReadData(2);
										mainMenuLogoRotateSpeed = 0;
										WaitForSingleObjectEx(mainMenuLogoRotateThread, INFINITE, FALSE);
										return 4;
									}
									// 4号位
									else if (secondMouseMessage.y >= 260 && secondMouseMessage.y < 310 && fourthLocationIndicator == 1)
									{
										toBoard->ReadData(4);
										mainMenuLogoRotateSpeed = 0;
										WaitForSingleObjectEx(mainMenuLogoRotateThread, INFINITE, FALSE);
										return 4;
									}
									// 取消读取
									else if (secondMouseMessage.y >= 340 && secondMouseMessage.y < 390)
									{
										// 中心区域清空
										DataLocationChooseButtonShowStatusChange(true);

										// 四个主按钮恢复
										MainMenuMainButtonShowStatusChange(false);

										// 左侧按钮恢复
										AILevelButtonShowStatusChange(false);

										// 右下角选项恢复
										RightBottomButtonShowStatusChange(false);

										break;
									}
								}
							}
						}
					}
				}
				// AI难度选择
				if (mouseMessage.x >= 33 && mouseMessage.x < 128)
				{
					// 简单
					if (mouseMessage.y >= 235 && mouseMessage.y < 290)
					{
						easyAIOn = true;
						normalAIOn = false;
						hardAIOn = false;
						toBoard->SetAIDepth(0);
						AILevelButtonShowStatusChange(true);
						AILevelButtonShowStatusChange(false);
					}
					// 中等
					else if (mouseMessage.y >= 295 && mouseMessage.y < 350)
					{
						easyAIOn = false;
						normalAIOn = true;
						hardAIOn = false;
						toBoard->SetAIDepth(1);
						AILevelButtonShowStatusChange(true);
						AILevelButtonShowStatusChange(false);
					}
					// 困难
					else if (mouseMessage.y >= 355 && mouseMessage.y < 410)
					{
						easyAIOn = false;
						normalAIOn = false;
						hardAIOn = true;
						toBoard->SetAIDepth(4);
						AILevelButtonShowStatusChange(true);
						AILevelButtonShowStatusChange(false);
					}
				}
				// 规则页面
				if (mouseMessage.y >= 425 && mouseMessage.y < 465)
				{
					if (mouseMessage.x >= 555 && mouseMessage.x < 620)
					{
						// 清除各种图标
						MainMenuMainButtonShowStatusChange(true);
						AILevelButtonShowStatusChange(true);
						RightBottomButtonShowStatusChange(true);

						// 显示规则页面
						ShowRule();

						// 恢复各种图标
						MainMenuMainButtonShowStatusChange(false);
						AILevelButtonShowStatusChange(false);
						RightBottomButtonShowStatusChange(false);
					}
				}
				// 关于页面
				/*
				roundrect(630, 425, 695, 465, 10, 10);
				roundrect(555, 425, 620, 465, 10, 10);*/
				if (mouseMessage.y >= 425 && mouseMessage.y < 465)
				{
					if (mouseMessage.x >= 630 && mouseMessage.x < 695)
					{
						// 清除各种图标
						MainMenuMainButtonShowStatusChange(true);
						AILevelButtonShowStatusChange(true);
						RightBottomButtonShowStatusChange(true);

						// 显示规则页面
						gotoCLI = ShowAbout();

						// 恢复各种图标
						MainMenuMainButtonShowStatusChange(false);
						AILevelButtonShowStatusChange(false);
						RightBottomButtonShowStatusChange(false);

						if (gotoCLI)
							return 0;
					}
				}
			}
		}
	}

	// 初始化棋盘
	void InitiateGUIBoard()
	{
		cleardevice();

		// 画边框
		setlinestyle(PS_SOLID, 4);
		rectangle(50, 40, 400, 390);


		setlinestyle(PS_SOLID, 2);
		for (int i = 91; i < 390; i += 50) // 画横线
			line(50, i, 400, i);
		for (int i = 101; i < 410; i += 50) // 画竖线
			line(i, 40, i, 390);

		/*
		// # 调试 #
		MOUSEMSG mouseMessage;
		while (true)
		{
			// 获取鼠标消息
			mouseMessage = GetMouseMsg();

			// #调试语句# 显示鼠标坐标
			ShowMouseCoordinate(mouseMessage.x, mouseMessage.y);
		}*/
	}

	// 棋盘操作探测
	int GUIBoardMove(moveVector &vec, ChessBoard* toBoard, bool isAntiAI)
	{
		/*
		返回值定义：
		0：程序结束
		1：棋子移动
		2：悔棋
		8：进入存储界面
		9：返回主菜单
		**/

		// 读取图片
		if (!playBoardImageRead)
			ReadPlayBoardElements();

		// 绘制行动指示图
		PlayBoardDirectionShowStatusChange(false, isAntiAI, toBoard);

		// 绘制右侧按钮（保存、返回主菜单）
		PlayBoardContronButtonShowStatusChange(false);

		// 绘制悔棋按钮
		RegretControlButtonShowStatusChange(false);

		// 显示比分
		ShowPoints(toBoard, false);

		// 绘制logo
		//PlayBoardLogoShowStatusChange(false);

		// Logo旋转
		HANDLE playBoardLogoRotateThread;
		unsigned threadID;
		playBoardLogoRotateSpeed = 1;
		playBoardLogoRotateThread = (HANDLE)_beginthreadex(NULL, 0, &PlayBoardLogoRotate, NULL, 0, &threadID);

		// 鼠标操作
		MOUSEMSG firstMouseMessage;
		MOUSEMSG secondMouseMessage;

		// 有效棋盘内两次点击
		bool flag = false;

		while (true)
		{
			// 获取鼠标消息
			firstMouseMessage = GetMouseMsg();

			// #调试语句# 显示鼠标坐标
			//ShowMouseCoordinate(firstMouseMessage.x, firstMouseMessage.y);

			// 动态追踪鼠标
			DynamicPlayBoardMousePositionCheck(firstMouseMessage.x, firstMouseMessage.y, toBoard, true);

			// Logo旋转加速
			PlayBoardLogoRotateSpeedUp(firstMouseMessage.x, firstMouseMessage.y);

			// 对鼠标按下位置做判断
			if (firstMouseMessage.uMsg == WM_LBUTTONDOWN)
			{
				// 清除动态追踪图案
				DynamicPlayBoardMousePositionDraw(playBoardDynamicTrackNum, true);
				playBoardDynamicTrackNum = -1;

				// 第一次点击落在棋盘范围内
				if (firstMouseMessage.x >= 50 && firstMouseMessage.x < 400)
				{
					if (firstMouseMessage.y >= 40 && firstMouseMessage.y < 390)
					{
						// 第一次棋子点击有效
						if (IsValidFirstChessClick(firstMouseMessage.x, firstMouseMessage.y, toBoard))
						{
							// 绘制提示落子点
							GenerateSuggestionQueue(firstMouseMessage.x, firstMouseMessage.y, toBoard, false);
							ProcessDrawQueue();

							while (true)
							{
								// 获取鼠标消息
								secondMouseMessage = GetMouseMsg();

								// #调试语句# 显示鼠标坐标
								//ShowMouseCoordinate(secondMouseMessage.x, secondMouseMessage.y);

								// 绘制鼠标追踪红十字
								DynamicDrawNextStepMouse(secondMouseMessage.x, secondMouseMessage.y);

								// Logo旋转加速
								PlayBoardLogoRotateSpeedUp(secondMouseMessage.x, secondMouseMessage.y);

								if (secondMouseMessage.uMsg == WM_LBUTTONDOWN)
								{
									if (secondMouseMessage.x >= 50 && secondMouseMessage.x < 400)
									{
										if (secondMouseMessage.y >= 40 && secondMouseMessage.y < 390)
										{
											vec.startHori = firstMouseMessage.y;
											vec.startVert = firstMouseMessage.x;
											vec.endHori = secondMouseMessage.y;
											vec.endVert = secondMouseMessage.x;
											flag = true;
											break;
										}
										else
											break;
									}
									else
										break;
								}
							}
						}
					}
					// 清除提示落子点
					GenerateSuggestionQueue(firstMouseMessage.x, firstMouseMessage.y, toBoard, true);
					ProcessDrawQueue();

					// 重置动态鼠标追踪记录值
					dynamicRow = -1;
					dynamicColomn = -1;
				}
				// 悔棋
				if (firstMouseMessage.x >= 475 && firstMouseMessage.x < 565)
				{
					if (firstMouseMessage.y >= 140 && firstMouseMessage.y < 190)
					{
						screenIsRegretOK = false;
						playBoardLogoRotateSpeed = 0;
						WaitForSingleObjectEx(playBoardLogoRotateThread, INFINITE, FALSE);
						return 2;
					}
				}
				// 返回主菜单
				if (firstMouseMessage.x >= 475 && firstMouseMessage.x < 693)
				{
					if (firstMouseMessage.y >= 210 && firstMouseMessage.y <= 260)
					{
						// 更改为确认按键
						ReturnConfirmButtonShowStatusChange(false);

						while (true)
						{
							// 获取鼠标消息
							secondMouseMessage = GetMouseMsg();

							// #调试语句# 显示鼠标坐标
							//ShowMouseCoordinate(secondMouseMessage.x, secondMouseMessage.y);

							// 动态追踪鼠标
							DynamicPlayBoardMousePositionCheck(secondMouseMessage.x, secondMouseMessage.y, toBoard, false);

							// Logo旋转加速
							PlayBoardLogoRotateSpeedUp(secondMouseMessage.x, secondMouseMessage.y);

							if (secondMouseMessage.uMsg == WM_LBUTTONDOWN)
							{
								if (secondMouseMessage.x >= 475 && secondMouseMessage.x < 693)
								{
									if (secondMouseMessage.y >= 210 && secondMouseMessage.y < 260)
									{
										playBoardLogoRotateSpeed = 0;
										WaitForSingleObjectEx(playBoardLogoRotateThread, INFINITE, FALSE);
										return 9;
									}
									else
										break;
								}
								else
									break;
							}
						}
						// 恢复主菜单返回按键
						ReturnConfirmButtonShowStatusChange(true);
					}
				}
				// 保存
				if (firstMouseMessage.x >= 603 && firstMouseMessage.x < 693)
				{
					if (firstMouseMessage.y >= 140 && firstMouseMessage.y < 190)
					{
						// 清空右侧按键区
						PlayBoardContronButtonShowStatusChange(true);

						// 记录位按钮绘制
						PlayBoardSaveDataButtonShowStatusChange(false);

						while (true)
						{
							// 获取鼠标消息
							secondMouseMessage = GetMouseMsg();

							// #调试语句# 显示鼠标坐标
							//ShowMouseCoordinate(secondMouseMessage.x, secondMouseMessage.y);

							// 动态追踪保存记录鼠标位置
							DynamicSaveDataMousePositionCheck(secondMouseMessage.x, secondMouseMessage.y);

							// Logo旋转加速
							PlayBoardLogoRotateSpeedUp(secondMouseMessage.x, secondMouseMessage.y);

							if (secondMouseMessage.uMsg == WM_LBUTTONDOWN)
							{
								// 清除动态追踪图案
								DynamicSaveDataMousePositionDraw(saveDataDynamicTrackNum, true);
								saveDataDynamicTrackNum = -1;

								if (secondMouseMessage.x >= 460 && secondMouseMessage.x < 560)
								{
									// 1号位
									if (secondMouseMessage.y >= 120 && secondMouseMessage.y < 170)
									{
										toBoard->SaveData(1);
										playBoardLogoRotateSpeed = 0;
										WaitForSingleObjectEx(playBoardLogoRotateThread, INFINITE, FALSE);
										return 9;
									}
									// 3号位
									else if (secondMouseMessage.y >= 180 && secondMouseMessage.y < 230)
									{
										toBoard->SaveData(3);
										playBoardLogoRotateSpeed = 0;
										WaitForSingleObjectEx(playBoardLogoRotateThread, INFINITE, FALSE);
										return 9;
									}
									// 5号位
									else if (secondMouseMessage.y >= 240 && secondMouseMessage.y <= 290)
									{
										toBoard->SaveData(5);
										playBoardLogoRotateSpeed = 0;
										WaitForSingleObjectEx(playBoardLogoRotateThread, INFINITE, FALSE);
										return 9;
									}
								}
								else if (secondMouseMessage.x >= 608 && secondMouseMessage.x < 708)
								{
									// 2号位
									if (secondMouseMessage.y >= 120 && secondMouseMessage.y < 170)
									{
										toBoard->SaveData(2);
										playBoardLogoRotateSpeed = 0;
										WaitForSingleObjectEx(playBoardLogoRotateThread, INFINITE, FALSE);
										return 9;
									}
									// 4号位
									else if (secondMouseMessage.y >= 180 && secondMouseMessage.y < 230)
									{
										toBoard->SaveData(4);
										playBoardLogoRotateSpeed = 0;
										WaitForSingleObjectEx(playBoardLogoRotateThread, INFINITE, FALSE);
										return 9;
									}
									// 取消
									else if (secondMouseMessage.y >= 240 && secondMouseMessage.y <= 290)
									{
										// 清空右侧按键区
										PlayBoardSaveDataButtonShowStatusChange(true);

										// 恢复绘制右侧按钮（保存、返回主菜单）
										PlayBoardContronButtonShowStatusChange(false);

										// 恢复绘制悔棋按钮
										RegretControlButtonShowStatusChange(false);

										break;
									}
								}
							}
						}
					}
				}
			}

			// 标准化位移坐标
			if (flag)
				flag = StandarizeVector(vec);
			if (flag)
			{
				playBoardLogoRotateSpeed = 0;
				WaitForSingleObjectEx(playBoardLogoRotateThread, INFINITE, FALSE);
				return 1;
			}
		}
	}

	// 将绘制棋盘清空
	void ClearDrawedChessBoard()
	{
		memset(drawedBoard, ' ', sizeof(drawedBoard));
	}

	// 将游戏棋盘同步到绘制棋盘
	void SyncToDrawChessBoard(ChessBoard *toBoard)
	{
		memcpy(drawedBoard, toBoard->GetBoard(), sizeof(drawedBoard));
	}

	// 绘制棋子
	void DrawChess(ChessBoard *toBoard)
	{
		GenerateDrawQueue(toBoard);
		ProcessDrawQueue();
	}

	// 设定屏幕允许悔棋
	void SetScreenRegretAllowed(bool allow)
	{
		if (allow)
			screenIsRegretOK = true;
		else
			screenIsRegretOK = false;
	}

	// 绘制人机对战行动指示图
	void DrawAntiAIDirection(ChessBoard *toBoard, bool AIFirst)
	{
		if (AIFirst)
		{
			// 绘制行动指示图
			if (toBoard->GetPlayer() == 0)
			{
				setlinecolor(WHITE);
				setfillcolor(WHITE);
				fillrectangle(425, 25, 500, 125);
				putimage(465, 25, &AIGo, SRCCOPY);
			}
			else
			{
				putimage(425, 25, &playerGo, SRCCOPY);
			}
		}
		else
		{
			// 绘制行动指示图
			if (toBoard->GetPlayer() == 0)
			{
				putimage(425, 25, &playerGo, SRCCOPY);
			}
			else
			{
				setlinecolor(WHITE);
				setfillcolor(WHITE);
				fillrectangle(425, 25, 500, 125);
				putimage(465, 25, &AIGo, SRCCOPY);
			}
		}
	}

	// 显示赢家
	void GUIShowWinner(ChessBoard *toBoard, bool isAntiAI, bool AIFirst)
	{
		if (isAntiAI)
		{
			if (AIFirst)
			{
				if (toBoard->IsTie())
					putimage(425, 25, &tie, SRCCOPY);
				else if (toBoard->IsWhiteWin())
				{
					setlinecolor(WHITE);
					setfillcolor(WHITE);
					fillrectangle(425, 25, 500, 125);
					putimage(465, 25, &AIWin, SRCCOPY);
				}
				else
					putimage(425, 25, &playerWin, SRCCOPY);
			}
			else
			{
				if (toBoard->IsTie())
					putimage(425, 25, &tie, SRCCOPY);
				else if (toBoard->IsWhiteWin())
					putimage(425, 25, &playerWin, SRCCOPY);
				else
				{
					setlinecolor(WHITE);
					setfillcolor(WHITE);
					fillrectangle(425, 25, 500, 125);
					putimage(465, 25, &AIWin, SRCCOPY);
				}
			}
		}
		else
		{
			if (toBoard->IsTie())
				putimage(425, 25, &tie, SRCCOPY);
			else if (toBoard->IsWhiteWin())
				putimage(425, 25, &whiteWin, SRCCOPY);
			else
				putimage(425, 25, &blackWin, SRCCOPY);
		}
	}

	// 显示计分
	void ShowPoints(ChessBoard *toBoard, bool erase)
	{
		if (erase)
		{
			setfillcolor(WHITE);
			setlinecolor(WHITE);
			fillrectangle(57, 405, 400, 480);
		}
		else
		{
			int pieceNum[2];
			memcpy(pieceNum, toBoard->GetTotalPieceNum(), sizeof(int) * 2);

			// 字符串转换
			char outNum[8];
			outNum[1] = char(pieceNum[0] % 10 + '0');
			outNum[0] = char(pieceNum[0] / 10 + '0');
			outNum[3] = ':';
			outNum[2] = outNum[4] = ' ';
			outNum[6] = char(pieceNum[1] % 10 + '0');
			outNum[5] = char(pieceNum[1] / 10 + '0');
			outNum[7] = '\0';

			LOGFONT defaultFont;
			gettextstyle(&defaultFont);

			LOGFONT numberFont;
			gettextstyle(&numberFont);
			settextcolor(BLACK);

			// 数字输出设置
			numberFont.lfHeight = 60;
			_tcscpy_s(numberFont.lfFaceName, _T("Arial"));
			settextstyle(&numberFont);

			putimage(57, 405, &white, SRCCOPY);
			putimage(322, 400, &black, SRCCOPY);
			outtextxy(140, 405, outNum);

			settextstyle(&defaultFont);
		}
	}
};