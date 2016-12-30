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

void GUIDualMode(ChessBoard* toBoard, Screen* screenManage, char* argv[]);
void GUIPlayerFirstMode(ChessBoard* toBoard, Screen* screenManage, char* argv[]);
void GUIAIFirstMode(ChessBoard* toBoard, Screen* screenManage, char* argv[], bool isResume);


bool GUIMenuPending(ChessBoard* toBoard, Screen* screenManage, char* argv[])
{
	screenManage->InitiateGUI();
	
	while (true)
	{
		switch (screenManage->ShowGUIMainMenu(toBoard))
		{
		case 1:
			toBoard->SetMode(1);
			toBoard->RandomAIStyle();
			screenManage->SetScreenRegretAllowed(false);
			GUIAIFirstMode(toBoard, screenManage, argv, false);
			break;
		case 2:
			toBoard->SetMode(2);
			toBoard->RandomAIStyle();
			screenManage->SetScreenRegretAllowed(false);
			GUIPlayerFirstMode(toBoard, screenManage, argv);
			break;
		case 3:
			toBoard->SetMode(3);
			toBoard->RandomAIStyle();
			screenManage->SetScreenRegretAllowed(false);
			GUIDualMode(toBoard, screenManage, argv);
			break;
		case 4:
			toBoard->RandomAIStyle();
			if (toBoard->GetMode() == 1)
				GUIAIFirstMode(toBoard, screenManage, argv, true);
			else if (toBoard->GetMode() == 2)
				GUIPlayerFirstMode(toBoard, screenManage, argv);
			else
				GUIDualMode(toBoard, screenManage, argv);
			break;
		case 0:
			screenManage->CloseGUI();
			return true;
		}
	}
	
}

// 双人对战模式
void GUIDualMode(ChessBoard* toBoard, Screen* screenManage, char* argv[])
{
	// 玩家行动向量
	moveVector vec = { 0,0,0,0 };

	screenManage->ClearGUI();
	screenManage->ClearDrawedChessBoard();
	screenManage->InitiateGUIBoard();

	// 玩家行动完毕旗标
	bool playerOKFlag = false;

	for (; !toBoard->IsOver();)
	{
		playerOKFlag = false;

		for (; !playerOKFlag;)
		{
			screenManage->DrawChess(toBoard);
			screenManage->SyncToDrawChessBoard(toBoard);

			switch (screenManage->GUIBoardMove(vec, toBoard, false))
			{
			case 1:
				toBoard->SaveStatus();
				playerOKFlag = toBoard->PutPiece(vec.startHori, vec.startVert, vec.endHori, vec.endVert);
				break;
			case 2:
				toBoard->PickPiece();
				break;
			case 9:
				toBoard->ClearBoard();
				screenManage->ClearDrawedChessBoard();
				screenManage->ClearGUI();
				return;
			}
		}
		screenManage->SetScreenRegretAllowed(true);
	}
	screenManage->DrawChess(toBoard);
	screenManage->ShowPoints(toBoard, false);
	screenManage->GUIShowWinner(toBoard, false, false);
	Sleep(3000);
	screenManage->ClearGUI();
	toBoard->ClearBoard();
}

// 玩家先手模式
void GUIPlayerFirstMode(ChessBoard* toBoard, Screen* screenManage, char* argv[])
{
	// 玩家行动向量
	moveVector vec = { 0,0,0,0 };

	// 玩家行动完毕旗标
	bool playerOKFlag;

	screenManage->ClearGUI();
	screenManage->ClearDrawedChessBoard();
	screenManage->InitiateGUIBoard();

	for (; !toBoard->IsOver();)
	{
		playerOKFlag = false;

		for (; !playerOKFlag;)
		{
			screenManage->DrawChess(toBoard);
			screenManage->SyncToDrawChessBoard(toBoard);

			switch (screenManage->GUIBoardMove(vec, toBoard, true))
			{
			case 1:
				toBoard->SaveStatus();
				playerOKFlag = toBoard->PutPiece(vec.startHori, vec.startVert, vec.endHori, vec.endVert);
				break;
			case 2:
				toBoard->PickPiece();
				break;
			case 9:
				toBoard->ClearBoard();
				screenManage->ClearDrawedChessBoard();
				screenManage->ClearGUI();
				return;
			}
		}

		if (toBoard->IsOver())
			break;

		screenManage->SetScreenRegretAllowed(true);
		screenManage->DrawChess(toBoard);
		screenManage->ShowPoints(toBoard, false);
		screenManage->SyncToDrawChessBoard(toBoard);
		screenManage->DrawAntiAIDirection(toBoard, false);
		Sleep(750);
		toBoard->AIMove();
	}
	screenManage->DrawChess(toBoard);
	screenManage->ShowPoints(toBoard, false);
	screenManage->GUIShowWinner(toBoard, true, false);
	Sleep(3000);
	screenManage->ClearGUI();
	toBoard->ClearBoard();
}

// AI先手模式
void GUIAIFirstMode(ChessBoard* toBoard, Screen* screenManage, char* argv[], bool isResume)
{
	// 玩家行动向量
	moveVector vec = { 0,0,0,0 };

	// 玩家行动完毕旗标
	bool playerOKFlag;
	bool firstStep = true;

	screenManage->ClearGUI();
	screenManage->ClearDrawedChessBoard();
	screenManage->InitiateGUIBoard();

	for (; !toBoard->IsOver();)
	{
		if (!isResume)
		{
			screenManage->DrawChess(toBoard);
			screenManage->ShowPoints(toBoard, false);
			screenManage->SyncToDrawChessBoard(toBoard);
			screenManage->DrawAntiAIDirection(toBoard, true);
			if (!firstStep)
			{
				Sleep(750);
			}
			firstStep = false;

			toBoard->AIMove();

			if (toBoard->IsOver())
				break;
		}
		
		isResume = false;
		playerOKFlag = false;

		for (; !playerOKFlag;)
		{
			screenManage->DrawChess(toBoard);
			screenManage->SyncToDrawChessBoard(toBoard);

			switch (screenManage->GUIBoardMove(vec, toBoard, true))
			{
			case 1:
				toBoard->SaveStatus();
				playerOKFlag = toBoard->PutPiece(vec.startHori, vec.startVert, vec.endHori, vec.endVert);
				break;
			case 2:
				toBoard->PickPiece();
				break;
			case 9:
				toBoard->ClearBoard();
				screenManage->ClearDrawedChessBoard();
				screenManage->ClearGUI();
				return;
			}
		}
		screenManage->SetScreenRegretAllowed(true);
	}
	screenManage->DrawChess(toBoard);
	screenManage->ShowPoints(toBoard, false);
	screenManage->GUIShowWinner(toBoard, true, true);
	Sleep(3000);
	screenManage->ClearGUI();
	toBoard->ClearBoard();
}