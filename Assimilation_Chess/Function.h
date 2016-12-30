#pragma once
#include<iostream>
#include<cstdlib>
#include<ctime>
#include<windows.h>
#include<fstream>
#include"ChessBoard.h"
#include"ScreenManip.h"
using namespace std;

// 函数原型声明区

void DualMode(ChessBoard* toBoard, Screen* screenManage, char* argv[]);
void SavePending(ChessBoard* toBoard, Screen* screenManage, char* argv[]);
void ReadPending(ChessBoard* toBoard, Screen* screenManage, char* argv[]);
void DataTouch(ChessBoard* toBoard, Screen* screenManage, char* argv[]);
void PlayerFirstMode(ChessBoard* toBoard, Screen* screenManage, char* argv[]);
void AIFirstMode(ChessBoard* toBoard, Screen* screenManage, char* argv[], bool isResume);
void TrainingMode(ChessBoard* toBoard, Screen* screenManage, char* argv[]);

// 主菜单等待选择
void MenuPending(ChessBoard* toBoard, Screen* screenManage, char* argv[])
{
	int mode; // 模式

	screenManage->ShowMenu();
		
	while (true)
	{
		cin >> mode;
		switch (mode)
		{
		case 1:
			toBoard->SetMode(1);
			toBoard->RandomAIStyle();
			AIFirstMode(toBoard, screenManage, argv, false);
			break;
		case 2:
			toBoard->SetMode(2);
			toBoard->RandomAIStyle();
			PlayerFirstMode(toBoard, screenManage, argv);
			break;
		case 3:
			toBoard->SetMode(3);
			toBoard->RandomAIStyle();
			DualMode(toBoard, screenManage, argv);
			break;
		case 4:
			ReadPending(toBoard, screenManage, argv);
			if (toBoard->GetMode() == 1)
				AIFirstMode(toBoard, screenManage, argv, true);
			else if (toBoard->GetMode() == 2)
				PlayerFirstMode(toBoard, screenManage, argv);
			else
				DualMode(toBoard, screenManage, argv);
			break;
		case 0:
			return;
		case 142857:
			TrainingMode(toBoard, screenManage, argv);
			break;
		default:
			cout << "输入错误！" << endl;
			break;

		}

		screenManage->ShowMenu();
	}
}

// 训练模式
void TrainingMode(ChessBoard* toBoard, Screen* screenManage, char* argv[])
{
	srand(time(0));
	cout << "请输入训练序号（大于等于0，小于等于99）：" << endl;
	int groupNum;
	cin >> groupNum;
	int trainNum;
	cout << "请输入训练组数：" << endl;
	cin >> trainNum;
	int evolveThresh;
	cout << "请输入演化阈值：" << endl;
	cin >> evolveThresh;
	char appendix[10];
	appendix[1] = char(groupNum % 10 + '0');
	groupNum /= 10;
	appendix[0] = char(groupNum % 10 + '0');
	appendix[2] = '\0';
	strcat_s(appendix, ".dat");
	cout << "请输入保存文件夹路径：" << endl;
	char saveDir[300];
	cin >> saveDir;
	strcat_s(saveDir, "\\group");
	strcat_s(saveDir, appendix);

	cout << "训练开始！" << endl;

	// 主AI随机初始化，以求得不同吸引域
	COPY_WEIGHT = COPY_WEIGHT + (rand() - 16384) / 4;
	ASSILIMATION_WEIGHT = ASSILIMATION_WEIGHT + (rand() - 16384) / 4;
	biasTable[0] = biasTable[0] + (rand() / double(RAND_MAX) - 0.5) / 20;
	biasTable[1] = biasTable[1] + (rand() / double(RAND_MAX) - 0.5) / 20;
	biasTable[2] = biasTable[2] + (rand() / double(RAND_MAX) - 0.5) / 20;
	biasTable[3] = biasTable[3] + (rand() / double(RAND_MAX) - 0.5) / 20;

	for (int i = 0; i < trainNum; i++)
	{
		// 随机初始化
		UNSTABLE_COPY_WEIGHT = COPY_WEIGHT + (i % 6 == 0 ? (rand() - 16384) / 4 : 0);
		UNSTABLE_ASSILIMATION_WEIGHT = ASSILIMATION_WEIGHT + (i % 6 == 1 ? (rand() - 16384) / 4 : 0);
		UNSTABLE_biasTable[0] = biasTable[0] + (i % 6 == 2 ? (rand() / double(RAND_MAX) - 0.5) / 20 : 0);
		UNSTABLE_biasTable[1] = biasTable[1] + (i % 6 == 3 ? (rand() / double(RAND_MAX) - 0.5) / 20 : 0);
		UNSTABLE_biasTable[2] = biasTable[2] + (i % 6 == 4 ? (rand() / double(RAND_MAX) - 0.5) / 20 : 0);
		UNSTABLE_biasTable[3] = biasTable[3] + (i % 6 == 5 ? (rand() / double(RAND_MAX) - 0.5) / 20 : 0);

		for (;;)
		{
			if (toBoard->IsOver())
				break;
			//screenManage->ShowBoard(toBoard);
			toBoard->AIMove();

			if (toBoard->IsOver())
				break;
			//screenManage->ShowBoard(toBoard);
			toBoard->UnstableAIMove();
		}

		// 训练矫正
		if (!toBoard->IsTie()) // 平手
		{
			if (!toBoard->IsWhiteWin() && (toBoard->GetDifference() >= evolveThresh)) // 大比分输
			{
				COPY_WEIGHT = UNSTABLE_COPY_WEIGHT;
				ASSILIMATION_WEIGHT = UNSTABLE_ASSILIMATION_WEIGHT;
				biasTable[0] = UNSTABLE_biasTable[0];
				biasTable[1] = UNSTABLE_biasTable[1];
				biasTable[2] = UNSTABLE_biasTable[2];
				biasTable[3] = UNSTABLE_biasTable[3];
			}
		}
		screenManage->ShowBoard(toBoard);
		screenManage->ShowWinner(toBoard);

		ofstream trainingData;
		trainingData.open(saveDir, ios_base::out | ios_base::app);
		trainingData << "第" << i << "轮训练结束" << endl;
		trainingData << "获胜方为：" << (toBoard->IsWhiteWin() ? "白  " : "黑  ") << "棋子差值为：" << toBoard->GetDifference();
		trainingData << "  AI演化：" << (!toBoard->IsTie() && !toBoard->IsWhiteWin() && (toBoard->GetDifference() >= evolveThresh) ? "是" : "否") << endl;
		trainingData << "当前权值：" << COPY_WEIGHT << " " << ASSILIMATION_WEIGHT << " " << biasTable[0] << " " << biasTable[1] << " " << biasTable[2] << " " << biasTable[3];
		trainingData << endl << endl;
		trainingData.close();
		//Sleep(2000);

		toBoard->ClearBoard();
	}

	cout << "训练完毕！" << endl;
}

// AI先手模式
void AIFirstMode(ChessBoard* toBoard, Screen* screenManage, char* argv[], bool isResume)
{
	int baseHori, baseVert, newHori, newVert;
	for (;;)
	{
		if (isResume)
			isResume = false;
		else
		{
			if (toBoard->IsOver())
				break;
			screenManage->ShowBoard(toBoard);
			//cout << "接下来AI行动。   switcher = " << toBoard->GetPlayer() << endl;
			toBoard->AIMove();
		}

		if (toBoard->IsOver())
			break;
		
		// 玩家行动
		for (;;)
		{
			screenManage->ShowBoard(toBoard);
			cin >> baseHori >> baseVert >> newHori >> newVert;
			if (baseHori > 0)
			{
				toBoard->SaveStatus();
				if (toBoard->PutPiece(baseHori, baseVert, newHori, newVert))
					break;
			}
			else if (baseHori < 0)
			{
				if (toBoard->PickPiece())
					continue;
			}
			else
			{
				SavePending(toBoard, screenManage, argv);
				return;
			}
		}
	}

	screenManage->ShowBoard(toBoard);
	screenManage->ShowWinner(toBoard);
	toBoard->ClearBoard();
}

// 玩家先手模式
void PlayerFirstMode(ChessBoard* toBoard, Screen* screenManage, char* argv[])
{
	int baseHori, baseVert, newHori, newVert;

	for (;;)
	{
		if (toBoard->IsOver())
			break;

		// 玩家行动
		for (;;)
		{
			screenManage->ShowBoard(toBoard);
			cin >> baseHori >> baseVert >> newHori >> newVert;
			if (baseHori > 0)
			{
				toBoard->SaveStatus();
				if (toBoard->PutPiece(baseHori, baseVert, newHori, newVert))
					break;
			}
			else if (baseHori < 0)
			{
				if (toBoard->PickPiece())
					continue;
			}
			else
			{
				SavePending(toBoard, screenManage, argv);
				return;
			}
		}

		if (toBoard->IsOver())
			break;
		screenManage->ShowBoard(toBoard);
		//cout << "接下来AI行动。   switcher = " << toBoard->GetPlayer() << endl;
		toBoard->AIMove();
	}

	screenManage->ShowBoard(toBoard);
	screenManage->ShowWinner(toBoard);
	toBoard->ClearBoard();
}

// 双人对战模式
void DualMode(ChessBoard* toBoard, Screen* screenManage, char* argv[])
{
	int baseHori, baseVert, newHori, newVert;

	for (; !toBoard->IsOver();)
	{
		screenManage->ShowBoard(toBoard);
		cin >> baseHori >> baseVert >> newHori >> newVert;

		if (baseHori > 0)
		{
			toBoard->SaveStatus();
			toBoard->PutPiece(baseHori, baseVert, newHori, newVert);
		}
		else if (baseHori < 0)
			toBoard->PickPiece();
		else
		{
			SavePending(toBoard, screenManage, argv);
			return;
		}
	}
	screenManage->ShowBoard(toBoard);
	screenManage->ShowWinner(toBoard);
	toBoard->ClearBoard();
}

// 记录存储选择
void SavePending(ChessBoard* toBoard, Screen* screenManage, char* argv[])
{
	DataTouch(toBoard, screenManage, argv);
	cout << "请输入存储位置：" << endl;
	int saveNum;

	for (;;)
	{
		cin >> saveNum;
		if (toBoard->SaveData(saveNum))
			break;
	}
	return;
}

// 记录读取选择
void ReadPending(ChessBoard* toBoard, Screen* screenManage, char* argv[])
{
	DataTouch(toBoard, screenManage, argv);
	cout << "请输入读取位置：" << endl;
	int readNum;
	for (;;)
	{
		cin >> readNum;
		if (toBoard->ReadData(readNum))
			break;
	}
	return;
}

// 记录探测
void DataTouch(ChessBoard* toBoard, Screen* screenManage, char* argv[])
{
	for (int i = 1; i <= 5; i++)
	{
		if (toBoard->TouchData(i) == 1)
			cout << "记录" << i << "正常" << endl;
		else if (toBoard->TouchData(i) == 0)
			cout << "记录" << i << "不存在" << endl;
		else
			cout << "记录" << i << "损坏" << endl;
	}
}