#include<iostream>
#include"ChessBoard.h"
#include"Function.h"
#include"ScreenManip.h"
#include"GUI_Function.h"
using namespace std;

int main(int argc, char *argv[])
{
	ChessBoard* toBoard = new ChessBoard(argv);
	Screen* screenManage = new Screen(argv);


	if (GUIMenuPending(toBoard, screenManage, argv))
		MenuPending(toBoard, screenManage, argv);

	return 0;
}