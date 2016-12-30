#pragma once
#include<iostream>
#include<cstdlib>
#include<ctime>
#include<windows.h>
#include<fstream>
#include"ChessBoard.h"
#include"ScreenManip.h"
using namespace std;

// ����ԭ��������

void DualMode(ChessBoard* toBoard, Screen* screenManage, char* argv[]);
void SavePending(ChessBoard* toBoard, Screen* screenManage, char* argv[]);
void ReadPending(ChessBoard* toBoard, Screen* screenManage, char* argv[]);
void DataTouch(ChessBoard* toBoard, Screen* screenManage, char* argv[]);
void PlayerFirstMode(ChessBoard* toBoard, Screen* screenManage, char* argv[]);
void AIFirstMode(ChessBoard* toBoard, Screen* screenManage, char* argv[], bool isResume);
void TrainingMode(ChessBoard* toBoard, Screen* screenManage, char* argv[]);

// ���˵��ȴ�ѡ��
void MenuPending(ChessBoard* toBoard, Screen* screenManage, char* argv[])
{
	int mode; // ģʽ

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
			cout << "�������" << endl;
			break;

		}

		screenManage->ShowMenu();
	}
}

// ѵ��ģʽ
void TrainingMode(ChessBoard* toBoard, Screen* screenManage, char* argv[])
{
	srand(time(0));
	cout << "������ѵ����ţ����ڵ���0��С�ڵ���99����" << endl;
	int groupNum;
	cin >> groupNum;
	int trainNum;
	cout << "������ѵ��������" << endl;
	cin >> trainNum;
	int evolveThresh;
	cout << "�������ݻ���ֵ��" << endl;
	cin >> evolveThresh;
	char appendix[10];
	appendix[1] = char(groupNum % 10 + '0');
	groupNum /= 10;
	appendix[0] = char(groupNum % 10 + '0');
	appendix[2] = '\0';
	strcat_s(appendix, ".dat");
	cout << "�����뱣���ļ���·����" << endl;
	char saveDir[300];
	cin >> saveDir;
	strcat_s(saveDir, "\\group");
	strcat_s(saveDir, appendix);

	cout << "ѵ����ʼ��" << endl;

	// ��AI�����ʼ��������ò�ͬ������
	COPY_WEIGHT = COPY_WEIGHT + (rand() - 16384) / 4;
	ASSILIMATION_WEIGHT = ASSILIMATION_WEIGHT + (rand() - 16384) / 4;
	biasTable[0] = biasTable[0] + (rand() / double(RAND_MAX) - 0.5) / 20;
	biasTable[1] = biasTable[1] + (rand() / double(RAND_MAX) - 0.5) / 20;
	biasTable[2] = biasTable[2] + (rand() / double(RAND_MAX) - 0.5) / 20;
	biasTable[3] = biasTable[3] + (rand() / double(RAND_MAX) - 0.5) / 20;

	for (int i = 0; i < trainNum; i++)
	{
		// �����ʼ��
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

		// ѵ������
		if (!toBoard->IsTie()) // ƽ��
		{
			if (!toBoard->IsWhiteWin() && (toBoard->GetDifference() >= evolveThresh)) // ��ȷ���
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
		trainingData << "��" << i << "��ѵ������" << endl;
		trainingData << "��ʤ��Ϊ��" << (toBoard->IsWhiteWin() ? "��  " : "��  ") << "���Ӳ�ֵΪ��" << toBoard->GetDifference();
		trainingData << "  AI�ݻ���" << (!toBoard->IsTie() && !toBoard->IsWhiteWin() && (toBoard->GetDifference() >= evolveThresh) ? "��" : "��") << endl;
		trainingData << "��ǰȨֵ��" << COPY_WEIGHT << " " << ASSILIMATION_WEIGHT << " " << biasTable[0] << " " << biasTable[1] << " " << biasTable[2] << " " << biasTable[3];
		trainingData << endl << endl;
		trainingData.close();
		//Sleep(2000);

		toBoard->ClearBoard();
	}

	cout << "ѵ����ϣ�" << endl;
}

// AI����ģʽ
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
			//cout << "������AI�ж���   switcher = " << toBoard->GetPlayer() << endl;
			toBoard->AIMove();
		}

		if (toBoard->IsOver())
			break;
		
		// ����ж�
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

// �������ģʽ
void PlayerFirstMode(ChessBoard* toBoard, Screen* screenManage, char* argv[])
{
	int baseHori, baseVert, newHori, newVert;

	for (;;)
	{
		if (toBoard->IsOver())
			break;

		// ����ж�
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
		//cout << "������AI�ж���   switcher = " << toBoard->GetPlayer() << endl;
		toBoard->AIMove();
	}

	screenManage->ShowBoard(toBoard);
	screenManage->ShowWinner(toBoard);
	toBoard->ClearBoard();
}

// ˫�˶�սģʽ
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

// ��¼�洢ѡ��
void SavePending(ChessBoard* toBoard, Screen* screenManage, char* argv[])
{
	DataTouch(toBoard, screenManage, argv);
	cout << "������洢λ�ã�" << endl;
	int saveNum;

	for (;;)
	{
		cin >> saveNum;
		if (toBoard->SaveData(saveNum))
			break;
	}
	return;
}

// ��¼��ȡѡ��
void ReadPending(ChessBoard* toBoard, Screen* screenManage, char* argv[])
{
	DataTouch(toBoard, screenManage, argv);
	cout << "�������ȡλ�ã�" << endl;
	int readNum;
	for (;;)
	{
		cin >> readNum;
		if (toBoard->ReadData(readNum))
			break;
	}
	return;
}

// ��¼̽��
void DataTouch(ChessBoard* toBoard, Screen* screenManage, char* argv[])
{
	for (int i = 1; i <= 5; i++)
	{
		if (toBoard->TouchData(i) == 1)
			cout << "��¼" << i << "����" << endl;
		else if (toBoard->TouchData(i) == 0)
			cout << "��¼" << i << "������" << endl;
		else
			cout << "��¼" << i << "��" << endl;
	}
}