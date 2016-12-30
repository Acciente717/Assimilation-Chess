#pragma once
#include<iostream>
#include<cmath>
#include<cstring>
#include<fstream>
#include<cstdlib>
#include<ctime>

using namespace std;



// 带权值的向量
struct WeightedVector
{
	int startHori;
	int startVert;
	int endHori;
	int endVert;
	int weight;
};

// 权值常值
int COPY_WEIGHT = 100000; // 8行走自我复制权值
int ASSILIMATION_WEIGHT = 200000; // 同化权值
double biasTable[6] = { 1,1,1,1,1,1 }; // 偏移值

// 不稳定权值常值
int UNSTABLE_COPY_WEIGHT = 100000; // 8行走自我复制权值
int UNSTABLE_ASSILIMATION_WEIGHT = 200000; // 同化权值
double UNSTABLE_biasTable[6] = { 1,1,1,1,1,1 }; // 偏移值

// AI搜索深度
int AISearchDepth = 2;

class PlainBoard
{
public:
	// ####################
	// ###  数据成员区  ###
	// ####################

	// 棋盘矩阵 (当前矩阵、上一步矩阵)
	char board[2][11][11];
	char memBoard[2][11][11];

	// 切换器 （切换棋盘和落子方）
	int switcher;
	int memSwitcher;

	// 棋子数 [状态][白、黑]
	int pieceNum[2][2];
	int memPieceNum[2][2];

	// 总步数
	int stepNum;
	int memStepNum;


	// ####################
	// ###  函数成员区  ###
	// ####################

	
};


// AI决策
WeightedVector AIJudge(PlainBoard tempBoard, int tempDepth, int setDepth, int cutThresh)
{
	WeightedVector tempVector = { 0,0,0,0,INT_MIN }; // 当前层的带权值向量
	WeightedVector deepVector = { 0,0,0,0,INT_MIN }; // 下一层的带权值向量
	PlainBoard nextBoard;
	int tempWeight = 0;

	// 随机数初始化
	srand(time(0));

	// 排除8行走重复计算
	bool judgeTable[11][11];
	memset(judgeTable, 0, sizeof(judgeTable));

	// 排除无意义16行走
	bool useful16;

	// 枚举向量
	for (int i = 2; i <= 8; i++)
	{
		for (int j = 2; j <= 8; j++)
		{
			// 找到一个我方棋子
			if (tempBoard.board[tempBoard.switcher][i][j] == (tempBoard.switcher == 0 ? 'O' : '#'))
			{
				for (int p = -2; p <= 2; p++)
				{
					for (int q = -2; q <= 2; q++)
					{
						// 找到一个棋子落点
						if (tempBoard.board[tempBoard.switcher][i + p][j + q] == ' ')
						{
							// 复制当前棋盘到下一迭代的棋盘
							memcpy(&nextBoard.board[!tempBoard.switcher][0][0], &tempBoard.board[tempBoard.switcher][0][0], sizeof(char) * 121);
							nextBoard.switcher = !tempBoard.switcher;
							memcpy(nextBoard.pieceNum, tempBoard.pieceNum, sizeof(tempBoard.pieceNum));

							// 权值置0
							tempWeight = 0;

							// 目标棋盘更新 + 计算权值
							// 8行走
							if (p != -2 && p != 2 && q != -2 && q != 2)
							{	
								// 防止8行走重复计算
								if (judgeTable[i + p][j + q])
									continue;
								judgeTable[i + p][j + q] = true;

								tempWeight += COPY_WEIGHT;

								// 更新下一棋盘的属性和盘面
								nextBoard.pieceNum[!tempBoard.switcher][tempBoard.switcher]++;
								nextBoard.board[!tempBoard.switcher][i + p][j + q] = (tempBoard.switcher == 0 ? 'O' : '#');
							}
							// 16行走
							else
							{
								// 排除无意义16行走
								useful16 = false;
								for (int r = -1; r <= 1; r++)
								{
									for (int s = -1; s <= 1; s++)
									{
										if (tempBoard.board[tempBoard.switcher][i + p + r][j + q + s] == (tempBoard.switcher == 0 ? '#' : 'O'))
										{
											useful16 = true;
											break;
										}
									}
									if (useful16)
										break;
								}
								if (!useful16)
									continue;

								// 更新下一棋盘的属性和盘面
								nextBoard.board[!tempBoard.switcher][i + p][j + q] = (tempBoard.switcher == 0 ? 'O' : '#');
								nextBoard.board[!tempBoard.switcher][i][j] = ' ';
							}

							// 同化操作
							for (int r = -1; r <= 1; r++)
							{
								for (int s = -1; s <= 1; s++)
								{
									if (nextBoard.board[!tempBoard.switcher][i + p + r][j + q + s] == (tempBoard.switcher == 0 ? '#' : 'O'))
									{
										nextBoard.board[!tempBoard.switcher][i + p + r][j + q + s] = (tempBoard.switcher == 0 ? 'O' : '#');
										nextBoard.pieceNum[!tempBoard.switcher][tempBoard.switcher]++;
										nextBoard.pieceNum[!tempBoard.switcher][!tempBoard.switcher]--;
										tempWeight += ASSILIMATION_WEIGHT;
									}
								}
							}

							// 深度优化
							if (tempDepth <= setDepth)
							{
								if (tempVector.weight == INT_MIN)
									deepVector = AIJudge(nextBoard, tempDepth + 1, setDepth, INT_MAX);
								else
									deepVector = AIJudge(nextBoard, tempDepth + 1, setDepth, tempWeight - tempVector.weight);
								tempWeight -= deepVector.weight * biasTable[tempDepth - 1];
							}

							// 随机化
							//if (tempDepth == 0)
							//	tempWeight += rand() / 100;

							// 比较此次枚举向量与已有最佳向量的优劣
							if (tempWeight > tempVector.weight)
							{
								tempVector.weight = tempWeight;
								tempVector.startHori = i;
								tempVector.startVert = j;
								tempVector.endHori = i + p;
								tempVector.endVert = j + q;
							}

							// 检测αβ剪枝条件
							if (tempVector.weight > cutThresh)
								return tempVector;
						}
					}
				}
			}
		}
	}

	if (tempVector.weight == INT_MIN)
		tempVector.weight = -1000000;
	return tempVector;
}

// 不稳定AI决策
WeightedVector UnstableAIJudge(PlainBoard tempBoard, int tempDepth, int setDepth, int cutThresh)
{
	WeightedVector tempVector = { 0,0,0,0,INT_MIN }; // 当前层的带权值向量
	WeightedVector deepVector = { 0,0,0,0,INT_MIN }; // 下一层的带权值向量
	PlainBoard nextBoard;
	int tempWeight = 0;

	// 随机数初始化
	srand(time(0));

	// 排除8行走重复计算
	bool judgeTable[11][11];
	memset(judgeTable, 0, sizeof(judgeTable));

	// 排除无意义16行走
	bool useful16;

	// 枚举向量
	for (int i = 2; i <= 8; i++)
	{
		for (int j = 2; j <= 8; j++)
		{
			// 找到一个我方棋子
			if (tempBoard.board[tempBoard.switcher][i][j] == (tempBoard.switcher == 0 ? 'O' : '#'))
			{
				for (int p = -2; p <= 2; p++)
				{
					for (int q = -2; q <= 2; q++)
					{
						// 找到一个棋子落点
						if (tempBoard.board[tempBoard.switcher][i + p][j + q] == ' ')
						{
							// 复制当前棋盘到下一迭代的棋盘
							memcpy(&nextBoard.board[!tempBoard.switcher][0][0], &tempBoard.board[tempBoard.switcher][0][0], sizeof(char) * 121);
							nextBoard.switcher = !tempBoard.switcher;
							memcpy(nextBoard.pieceNum, tempBoard.pieceNum, sizeof(tempBoard.pieceNum));

							// 权值置0
							tempWeight = 0;

							// 目标棋盘更新 + 计算权值
							// 8行走
							if (p != -2 && p != 2 && q != -2 && q != 2)
							{
								// 防止8行走重复计算
								if (judgeTable[i + p][j + q])
									continue;
								judgeTable[i + p][j + q] = true;

								tempWeight += UNSTABLE_COPY_WEIGHT;

								// 更新下一棋盘的属性和盘面
								nextBoard.pieceNum[!tempBoard.switcher][tempBoard.switcher]++;
								nextBoard.board[!tempBoard.switcher][i + p][j + q] = (tempBoard.switcher == 0 ? 'O' : '#');
							}
							// 16行走
							else
							{
								// 排除无意义16行走
								useful16 = false;
								for (int r = -1; r <= 1; r++)
								{
									for (int s = -1; s <= 1; s++)
									{
										if (tempBoard.board[tempBoard.switcher][i + p + r][j + q + s] == (tempBoard.switcher == 0 ? '#' : 'O'))
										{
											useful16 = true;
											break;
										}
									}
									if (useful16)
										break;
								}
								if (!useful16)
									continue;

								// 更新下一棋盘的属性和盘面
								nextBoard.board[!tempBoard.switcher][i + p][j + q] = (tempBoard.switcher == 0 ? 'O' : '#');
								nextBoard.board[!tempBoard.switcher][i][j] = ' ';
							}

							// 同化操作
							for (int r = -1; r <= 1; r++)
							{
								for (int s = -1; s <= 1; s++)
								{
									if (nextBoard.board[!tempBoard.switcher][i + p + r][j + q + s] == (tempBoard.switcher == 0 ? '#' : 'O'))
									{
										nextBoard.board[!tempBoard.switcher][i + p + r][j + q + s] = (tempBoard.switcher == 0 ? 'O' : '#');
										nextBoard.pieceNum[!tempBoard.switcher][tempBoard.switcher]++;
										nextBoard.pieceNum[!tempBoard.switcher][!tempBoard.switcher]--;
										tempWeight += UNSTABLE_ASSILIMATION_WEIGHT;
									}
								}
							}

							// 深度优化
							if (tempDepth <= setDepth)
							{
								if (tempVector.weight == INT_MIN)
									deepVector = UnstableAIJudge(nextBoard, tempDepth + 1, setDepth, INT_MAX);
								else
									deepVector = UnstableAIJudge(nextBoard, tempDepth + 1, setDepth, tempWeight - tempVector.weight);
								tempWeight -= deepVector.weight * UNSTABLE_biasTable[tempDepth];
							}

							// 随机化
							if (tempDepth == 0)
								tempWeight += rand() / 100;


							// 比较此次枚举向量与已有最佳向量的优劣
							if (tempWeight > tempVector.weight)
							{
								tempVector.weight = tempWeight;
								tempVector.startHori = i;
								tempVector.startVert = j;
								tempVector.endHori = i + p;
								tempVector.endVert = j + q;
							}

							// 检测αβ剪枝条件
							if (tempVector.weight > cutThresh)
								return tempVector;
						}
					}
				}
			}
		}
	}

	if (tempVector.weight == INT_MIN)
		tempVector.weight = -1000000;
	return tempVector;
}

class ChessBoard : protected PlainBoard
{
protected:
	// ####################
	// ###  数据成员区  ###
	// ####################

	// 棋盘矩阵 (当前矩阵、上一步矩阵)
	// 继承

	// 可行位置权指标
	int weightTable[11][11];

	// 是否允许悔棋
	int ifRegretOK;

	// 游戏模式
	int mode;

	// 程序地址
	char address[300];

	// 数据缓冲区
	char buffer[2000];

	// ####################
	// ###  私有函数区  ###
	// ####################

	// 清空函数
	void DataReset()
	{
		// 初始化
		memset(board, int(' '), sizeof(board));
		memset(weightTable, 0, sizeof(weightTable));
		switcher = 0;
		pieceNum[0][0] = 2;
		pieceNum[0][1] = 2;
		pieceNum[1][0] = 3;
		pieceNum[1][1] = 3;
		ifRegretOK = false;
		stepNum = 0;
		mode = -1;		

		// 初始4子
		board[0][2][2] = '#';
		board[0][8][8] = '#';
		board[0][2][8] = 'O';
		board[0][8][2] = 'O';

		// 不可行走区域设定
		for (int i = 0; i <= 10; i++)
		{
			board[0][i][0] = 'X';
			board[0][i][1] = 'X';
			board[0][i][9] = 'X';
			board[0][i][10] = 'X';
			board[1][i][0] = 'X';
			board[1][i][1] = 'X';
			board[1][i][9] = 'X';
			board[1][i][10] = 'X';
		}
		for (int j = 0; j <= 10; j++)
		{
			board[0][0][j] = 'X';
			board[0][1][j] = 'X';
			board[0][9][j] = 'X';
			board[0][10][j] = 'X';
			board[1][0][j] = 'X';
			board[1][1][j] = 'X';
			board[1][9][j] = 'X';
			board[1][10][j] = 'X';
		}

		// 记忆数据初始化
		memcpy(memBoard, board, sizeof(board));
		memSwitcher = switcher;
		memcpy(memPieceNum, pieceNum, sizeof(pieceNum));
		memStepNum = stepNum;
	}

	// 生成写数据缓冲区（整型变量）
	void BufferBuild_Int(char* &bufferPointer, char* &tempPointer, char &totalHash, int &fileLen)
	{
		char fourHash = 0;
		for (int i = 0; i < sizeof(int); i++)
		{
			fourHash = fourHash ^ *tempPointer;
			totalHash = totalHash ^ *tempPointer;
			memcpy(bufferPointer++, tempPointer++, sizeof(char));
			fileLen++;
		}
		memcpy(bufferPointer++, &fourHash, sizeof(char));
		fileLen++;
	}

	// 从数据缓冲区恢复数据（整型变量）
	bool BufferRead_Int(char* &bufferPointer, char* &tempPointer, char &totalHash, int &fileLen, bool trueRead = true)
	{
		char fourHash = 0;
		for (int i = 0; i < sizeof(int); i++)
		{
			fourHash = fourHash ^ *bufferPointer;
			totalHash = totalHash ^ *bufferPointer;
			if (trueRead)
				memcpy(tempPointer++, bufferPointer++, sizeof(char));
			else
			{
				tempPointer++;
				bufferPointer++;
			}
			fileLen++;
		}
		fileLen++;
		if (fourHash != *(bufferPointer++))
			return false;
		else
			return true;
	}

	// 路径重置
	void AddressReset()
	{
		address[strlen(address) - 22] = '\0';
	}

public:
	// ####################
	// ###  公有函数区  ###
	// ####################

	// 构造函数
	ChessBoard(char* argv[])
	{
		// 初始化
		memset(board, int(' '), sizeof(board));
		memset(weightTable, 0, sizeof(weightTable));
		switcher = 0;
		pieceNum[0][0] = 2;
		pieceNum[0][1] = 2;
		pieceNum[1][0] = 3;
		pieceNum[1][1] = 3;
		ifRegretOK = false;
		stepNum = 0;
		mode = -1;

		// 初始4子
		board[0][2][2] = '#';
		board[0][8][8] = '#';
		board[0][2][8] = 'O';
		board[0][8][2] = 'O';

		// 不可行走区域设定
		for (int i = 0; i <= 10; i++)
		{
			board[0][i][0] = 'X';
			board[0][i][1] = 'X';
			board[0][i][9] = 'X';
			board[0][i][10] = 'X';
			board[1][i][0] = 'X';
			board[1][i][1] = 'X';
			board[1][i][9] = 'X';
			board[1][i][10] = 'X';
		}
		for (int j = 0; j <= 10; j++)
		{
			board[0][0][j] = 'X';
			board[0][1][j] = 'X';
			board[0][9][j] = 'X';
			board[0][10][j] = 'X';
			board[1][0][j] = 'X';
			board[1][1][j] = 'X';
			board[1][9][j] = 'X';
			board[1][10][j] = 'X';
		}

		// 存储地址更新
		strcpy_s(address, argv[0]);
		address[strlen(address) - 22] = '\0';

		// 记忆数据初始化
		memcpy(memBoard, board, sizeof(board));
		memSwitcher = switcher;
		memcpy(memPieceNum, pieceNum, sizeof(pieceNum));
		memStepNum = stepNum;
	}

	// 析构函数 （使用默认）

	// 设定游戏模式
	void SetMode(int newMode)
	{
		mode = newMode;
		return;
	}

	// 获取游戏模式
	int GetMode()
	{
		return mode;
	}

	// 落子函数
	bool PutPiece(int baseHori, int baseVert, int newHori, int newVert)
	{
		// 行走目标位置非空
		if (board[switcher][newHori + 1][newVert + 1] != ' ')
		{
			cout << "行走目标位置有子，无法行走！" << endl;
			return false;
		}
		// 行走起始位置无效
		else if (board[switcher][baseHori + 1][baseVert + 1] != (switcher == 0 ? 'O' : '#'))
		{
			cout << "行走起始位置无效，无法行走！" << endl;
			return false;
		}
		// 行走位置有效
		else
		{
			// 8邻行走
			if (abs(newHori - baseHori) <= 1 && abs(newVert - baseVert) <= 1)
			{
				// 目标棋盘更新
				for (int i = 2; i <= 8; i++)
					for (int j = 2; j <= 8; j++)
						board[!switcher][i][j] = board[switcher][i][j];

				// 目标计数更新
				pieceNum[!switcher][0] = pieceNum[switcher][0];
				pieceNum[!switcher][1] = pieceNum[switcher][1];

				// 棋子复制
				board[!switcher][newHori + 1][newVert + 1] = (switcher == 0 ? 'O' : '#');
				pieceNum[!switcher][switcher]++;

				// 同化操作
				for (int i = 0; i <= 2; i++)
				{
					if (board[!switcher][newHori + i][newVert + 2] == (switcher == 0 ? '#' : 'O'))
					{
						board[!switcher][newHori + i][newVert + 2] = switcher == 0 ? 'O' : '#';
						pieceNum[!switcher][switcher]++;
						pieceNum[!switcher][!switcher]--;

					}
					if (board[!switcher][newHori + i][newVert] == (switcher == 0 ? '#' : 'O'))
					{
						board[!switcher][newHori + i][newVert] = switcher == 0 ? 'O' : '#';
						pieceNum[!switcher][switcher]++;
						pieceNum[!switcher][!switcher]--;
					}
				}
				if (board[!switcher][newHori][newVert + 1] == (switcher == 0 ? '#' : 'O'))
				{
					board[!switcher][newHori][newVert + 1] = (switcher == 0 ? 'O' : '#');
					pieceNum[!switcher][switcher]++;
					pieceNum[!switcher][!switcher]--;
				}
				if (board[!switcher][newHori + 2][newVert + 1] == (switcher == 0 ? '#' : 'O'))
				{
					board[!switcher][newHori + 2][newVert + 1] = (switcher == 0 ? 'O' : '#');
					pieceNum[!switcher][switcher]++;
					pieceNum[!switcher][!switcher]--;
				}

				// 棋盘属性更新
				switcher = !switcher;
				stepNum++;

				return true;
			}
			// 16邻行走
			else if (abs(newHori - baseHori) <= 2 && abs(newVert - baseVert) <= 2)
			{
				// 目标棋盘更新
				for (int i = 2; i <= 8; i++)
					for (int j = 2; j <= 8; j++)
						board[!switcher][i][j] = board[switcher][i][j];

				// 目标计数更新
				pieceNum[!switcher][0] = pieceNum[switcher][0];
				pieceNum[!switcher][1] = pieceNum[switcher][1];

				// 棋子转移
				board[!switcher][newHori + 1][newVert + 1] = (switcher == 0 ? 'O' : '#');
				board[!switcher][baseHori + 1][baseVert + 1] = ' ';

				// 同化操作
				for (int i = 0; i <= 2; i++)
				{
					if (board[!switcher][newHori + i][newVert + 2] == (switcher == 0 ? '#' : 'O'))
					{
						board[!switcher][newHori + i][newVert + 2] = switcher == 0 ? 'O' : '#';
						pieceNum[!switcher][switcher]++;
						pieceNum[!switcher][!switcher]--;
					}
					if (board[!switcher][newHori + i][newVert] == (switcher == 0 ? '#' : 'O'))
					{
						board[!switcher][newHori + i][newVert] = switcher == 0 ? 'O' : '#';
						pieceNum[!switcher][switcher]++;
						pieceNum[!switcher][!switcher]--;
					}
				}
				if (board[!switcher][newHori][newVert + 1] == (switcher == 0 ? '#' : 'O'))
				{
					board[!switcher][newHori][newVert + 1] = (switcher == 0 ? 'O' : '#');
					pieceNum[!switcher][switcher]++;
					pieceNum[!switcher][!switcher]--;
				}
				if (board[!switcher][newHori + 2][newVert + 1] == (switcher == 0 ? '#' : 'O'))
				{
					board[!switcher][newHori + 2][newVert + 1] = (switcher == 0 ? 'O' : '#');
					pieceNum[!switcher][switcher]++;
					pieceNum[!switcher][!switcher]--;
				}

				// 棋盘属性更新
				switcher = !switcher;
				stepNum++;

				return true;
			}
			// 超出行走范围
			else
			{
				cout << "超出行走范围，无法行走！" << endl;
				return false;
			}
		}
	}

	// 状态保存函数（供悔棋用）
	void SaveStatus()
	{
		// 记忆数据记录
		memcpy(memBoard, board, sizeof(board));
		memSwitcher = switcher;
		memcpy(memPieceNum, pieceNum, sizeof(pieceNum));
		memStepNum = stepNum;
		ifRegretOK = true;
	}

	// 悔棋函数
	bool PickPiece()
	{
		// 允许悔棋
		if (ifRegretOK)
		{
			// 棋盘属性变更（弃用）
			/*switcher = !switcher;
			stepNum--;
			ifRegretOK = false;*/

			// 记忆数据恢复
			memcpy(board, memBoard, sizeof(board));
			switcher = memSwitcher;
			memcpy(pieceNum, memPieceNum, sizeof(pieceNum));
			stepNum = memStepNum;
			ifRegretOK = false;

			return true;
		}
		// 不可悔棋
		else if (pieceNum[switcher][switcher] == 2)
			cout << "还没落子，何来悔棋？" << endl;
		else
			cout << "悔棋只可一次，不可贪心！" << endl;
		return false;
	}

	// AI落子
	void AIMove()
	{
		WeightedVector moveVector;
		if (IsCritical())
			moveVector = AIJudge(*this, 1, 0, INT_MAX);
		else
			moveVector = AIJudge(*this, 1, AISearchDepth, INT_MAX);
		PutPiece(moveVector.startHori - 1, moveVector.startVert - 1, moveVector.endHori - 1, moveVector.endVert - 1);
	}

	// 不稳定AI落子
	void UnstableAIMove()
	{
		WeightedVector moveVector;
		if (IsCritical())
			moveVector = UnstableAIJudge(*this, 1, 0, INT_MAX);
		else
			moveVector = UnstableAIJudge(*this, 1, 4, INT_MAX);
		PutPiece(moveVector.startHori - 1, moveVector.startVert - 1, moveVector.endHori - 1, moveVector.endVert - 1);
	}

	// 打印函数
	void Print()
	{
		cout << "┌─┬─┬─┬─┬─┬─┬─┐" << endl;
		for (int i = 2; i <= 7; i++)
		{
			cout << "│";
			for (int j = 2; j <= 8; j++)
			{
				cout << (board[switcher][i][j] == '#' ? "○" : (board[switcher][i][j] == 'O' ? "●" : "  ")) << "│";
			}
			cout << endl << "├─┼─┼─┼─┼─┼─┼─┤" << endl;
		}
		cout << "│";
		for (int j = 2; j <= 8; j++)
		{
			cout << (board[switcher][8][j] == '#' ? "○" : (board[switcher][8][j] == 'O' ? "●" : "  ")) << "│";
		}
		cout << endl << "└─┴─┴─┴─┴─┴─┴─┘" << endl << endl << "白：黑" << pieceNum[switcher][0] << "：" << pieceNum[switcher][1] << endl << endl;
	}

	// 局点判断函数
	bool IsCritical()
	{
		if (pieceNum[switcher][0] + pieceNum[switcher][1] == 48)
			return true;
		else
			return false;
	}

	// 结局判断函数 （不带有棋盘重置）
	bool IsOver()
	{
		if (pieceNum[switcher][0] + pieceNum[switcher][1] == 49)
		{
			// DataReset();
			return true;
		}
		else if (stepNum == 200)
			return true;
		else
		{
			for (int i = 2; i <= 8; i++)
			{
				for (int j = 2; j <= 8; j++)
				{
					// 找到一个空位，检查是否是可行位
					if (board[switcher][i][j] == ' ')
					{
						for (int m = -2; m <= 2; m++)
							for (int n = -2; n <= 2; n++)
								if (board[switcher][i + m][j + n] == (switcher == 0 ? 'O' : '#'))
									return false;
					}
				}
			}
			// DataReset();
			return true;
		}
	}

	// 取棋盘
	char *GetBoard()
	{
		return &board[switcher][0][0];
	}

	// 取全部棋子数
	int *GetTotalPieceNum()
	{
		return pieceNum[switcher];
	}

	// 清空棋盘
	void ClearBoard()
	{
		DataReset();
		return;
	}

	// 平手判断
	bool IsTie()
	{
		if (pieceNum[switcher][0] == pieceNum[switcher][1])
			return true;
		else if (stepNum == 200)
			return true;
		else
			return false;
	}

	// 取棋盘落子数
	int GetPieceNum()
	{
		return pieceNum[switcher][0] + pieceNum[switcher][1];
	}

	// 取当前落子者
	int GetPlayer()
	{
		return switcher;
	}

	// 取赢者
	bool IsWhiteWin()
	{
		if (pieceNum[switcher][0] > pieceNum[switcher][1])
			return true;
		else
			return false;
	}

	// 取棋子数差
	int GetDifference()
	{
		return abs(pieceNum[switcher][0] - pieceNum[switcher][1]);
	}

	// 存记录
	bool SaveData(int saveNum)
	{
		char* bufferPointer = &buffer[0]; // 缓冲区指针
		char* tempPointer = NULL; // 当前待存储数据指针
		int fileLen = 0; // 文件长度
		char fourHash = 0; // 四字节校验数
		char totalHash = 0; // 全文件校验数
		char complement = 0; // 补足

		// 改动目标路径
		switch (saveNum)
		{
		case 1:
			strcat_s(address, "savefile\\savefile1.dat");
			break;
		case 2:
			strcat_s(address, "savefile\\savefile2.dat");
			break;
		case 3:
			strcat_s(address, "savefile\\savefile3.dat");
			break;
		case 4:
			strcat_s(address, "savefile\\savefile4.dat");
			break;
		case 5:
			strcat_s(address, "savefile\\savefile5.dat");
			break;
		default:
			cout << "存储位置为1至5！" << endl;
			return false;
		}


		// 将棋盘存入缓存
		tempPointer = &board[0][0][0];
		for (int i = 0; i < 242; i++)
		{
			fourHash = fourHash ^ *tempPointer;
			totalHash = totalHash ^ *tempPointer;
			memcpy(bufferPointer++, tempPointer++, sizeof(char));
			fileLen++;

			// 四字节一校验
			if (i % 4 == 3)
			{
				memcpy(bufferPointer++, &fourHash, sizeof(char));
				fileLen++;
				fourHash = 0;
			}
		}
		// 补足两字节 增加校验
		memcpy(bufferPointer++, &complement, sizeof(char));
		memcpy(bufferPointer++, &complement, sizeof(char));
		memcpy(bufferPointer++, &fourHash, sizeof(char));
		fileLen += 3;
		fourHash = 0;

		// 将记忆棋盘存入缓存
		tempPointer = &memBoard[0][0][0];
		for (int i = 0; i < 242; i++)
		{
			fourHash = fourHash ^ *tempPointer;
			totalHash = totalHash ^ *tempPointer;
			memcpy(bufferPointer++, tempPointer++, sizeof(char));
			fileLen++;

			// 四字节一校验
			if (i % 4 == 3)
			{
				memcpy(bufferPointer++, &fourHash, sizeof(char));
				fileLen++;
				fourHash = 0;
			}
		}
		// 补足两字节 增加校验
		memcpy(bufferPointer++, &complement, sizeof(char));
		memcpy(bufferPointer++, &complement, sizeof(char));
		memcpy(bufferPointer++, &fourHash, sizeof(char));
		fileLen += 3;
		fourHash = 0;

		// 将可行位置权指标存入缓存
		tempPointer = (char*)&weightTable[0][0];
		for (int i = 0; i < 121; i++)
		{
			BufferBuild_Int(bufferPointer, tempPointer, totalHash, fileLen);
		}

		// 将棋子数存入缓存
		tempPointer = (char*)&pieceNum[0][0];
		for (int i = 0; i < 4; i++)
		{
			BufferBuild_Int(bufferPointer, tempPointer, totalHash, fileLen);
		}

		// 将记忆棋子数存入缓存
		tempPointer = (char*)&memPieceNum[0][0];
		for (int i = 0; i < 4; i++)
		{
			BufferBuild_Int(bufferPointer, tempPointer, totalHash, fileLen);
		}

		// 将切换器、悔棋旗标、游戏模式、总步数存入缓存
		tempPointer = (char*)&switcher;
		BufferBuild_Int(bufferPointer, tempPointer, totalHash, fileLen);
		tempPointer = (char*)&ifRegretOK;
		BufferBuild_Int(bufferPointer, tempPointer, totalHash, fileLen);
		tempPointer = (char*)&mode;
		BufferBuild_Int(bufferPointer, tempPointer, totalHash, fileLen);
		tempPointer = (char*)&stepNum;
		BufferBuild_Int(bufferPointer, tempPointer, totalHash, fileLen);

		// 将记忆切换器、记忆总步数存入缓存
		tempPointer = (char*)&memSwitcher;
		BufferBuild_Int(bufferPointer, tempPointer, totalHash, fileLen);
		tempPointer = (char*)&memStepNum;
		BufferBuild_Int(bufferPointer, tempPointer, totalHash, fileLen);

		// 将全文件校验值存入缓存
		memcpy(bufferPointer++, &totalHash, sizeof(char));
		fileLen++;


		ofstream outFile(address, ios_base::out | ios_base::binary);
		if (!outFile)
			cout << "文件打开失败！" << endl;
		else
		{
			outFile.write(&buffer[0], fileLen);
		}

		outFile.close();
		DataReset();

		// 恢复初始路径
		AddressReset();

		return true;
	}

	// 读记录
	bool ReadData(int saveNum)
	{
		// 改动目标路径
		switch (saveNum)
		{
		case 1:
			strcat_s(address, "savefile\\savefile1.dat");
			break;
		case 2:
			strcat_s(address, "savefile\\savefile2.dat");
			break;
		case 3:
			strcat_s(address, "savefile\\savefile3.dat");
			break;
		case 4:
			strcat_s(address, "savefile\\savefile4.dat");
			break;
		case 5:
			strcat_s(address, "savefile\\savefile5.dat");
			break;
		default:
			cout << "存储位置为1至5！" << endl;
			return false;
		}

		ifstream inFile(address, ios_base::in | ios_base::binary);
		if (!inFile)
		{
			cout << "文件打开失败！" << endl;
			AddressReset();
			return false;
		}
		else
		{
			inFile.read(&buffer[0], 2000);

			char* bufferPointer = &buffer[0]; // 缓冲区指针
			char* tempPointer = NULL; // 当前待存储数据指针
			int fileLen = 0; // 文件长度
			char fourHash = 0; // 四字节校验数
			char totalHash = 0; // 全文件校验数
			char complement = 0; // 补足

			// 恢复棋盘
			tempPointer = &board[0][0][0];
			for (int i = 0; i < 242; i++)
			{
				fourHash = fourHash ^ *bufferPointer;
				totalHash = totalHash ^ *bufferPointer;
				memcpy(tempPointer++, bufferPointer++, sizeof(char));
				fileLen++;

				// 四字节一校验
				if (i % 4 == 3)
				{
					// 校验失败
					if (fourHash != *(bufferPointer++))
					{
						cout << "存储文件校验失败！" << endl;
						DataReset();
						AddressReset();
						return false;
					}
					fourHash = 0;
					fileLen++;
				}
			}
			// 补足两字节 计算校验
			bufferPointer += 2;
			if (fourHash != *(bufferPointer++))
			{
				cout << "存储文件校验失败！" << endl;
				DataReset();
				AddressReset();
				return false;
			}
			fileLen += 3;

			// 恢复记忆棋盘
			tempPointer = &memBoard[0][0][0];
			for (int i = 0; i < 242; i++)
			{
				fourHash = fourHash ^ *bufferPointer;
				totalHash = totalHash ^ *bufferPointer;
				memcpy(tempPointer++, bufferPointer++, sizeof(char));
				fileLen++;

				// 四字节一校验
				if (i % 4 == 3)
				{
					// 校验失败
					if (fourHash != *(bufferPointer++))
					{
						cout << "存储文件校验失败！" << endl;
						DataReset();
						AddressReset();
						return false;
					}
					fourHash = 0;
					fileLen++;
				}
			}
			// 补足两字节 计算校验
			bufferPointer += 2;
			if (fourHash != *(bufferPointer++))
			{
				cout << "存储文件校验失败！" << endl;
				DataReset();
				AddressReset();
				return false;
			}
			fileLen += 3;

			// 将可行位置权指标恢复
			tempPointer = (char*)&weightTable[0][0];
			for (int i = 0; i < 121; i++)
			{
				if (!BufferRead_Int(bufferPointer, tempPointer, totalHash, fileLen))
				{
					cout << "存储文件校验失败！" << endl;
					DataReset();
					AddressReset();
					return false;
				}
			}

			// 将棋子数恢复
			tempPointer = (char*)&pieceNum[0][0];
			for (int i = 0; i < 4; i++)
			{
				if (!BufferRead_Int(bufferPointer, tempPointer, totalHash, fileLen))
				{
					cout << "存储文件校验失败！" << endl;
					DataReset();
					AddressReset();
					return false;
				}
			}

			// 将记忆棋子数恢复
			tempPointer = (char*)&memPieceNum[0][0];
			for (int i = 0; i < 4; i++)
			{
				if (!BufferRead_Int(bufferPointer, tempPointer, totalHash, fileLen))
				{
					cout << "存储文件校验失败！" << endl;
					DataReset();
					AddressReset();
					return false;
				}
			}

			// 将切换器、棋子数、悔棋旗标、游戏模式、总步数恢复
			tempPointer = (char*)&switcher;
			if (!BufferRead_Int(bufferPointer, tempPointer, totalHash, fileLen))
			{
				cout << "存储文件校验失败！" << endl;
				DataReset();
				AddressReset();
				return false;
			}
			tempPointer = (char*)&ifRegretOK;
			if (!BufferRead_Int(bufferPointer, tempPointer, totalHash, fileLen))
			{
				cout << "存储文件校验失败！" << endl;
				DataReset();
				AddressReset();
				return false;
			}
			tempPointer = (char*)&mode;
			if (!BufferRead_Int(bufferPointer, tempPointer, totalHash, fileLen))
			{
				cout << "存储文件校验失败！" << endl;
				DataReset();
				AddressReset();
				return false;
			}
			tempPointer = (char*)&stepNum;
			if (!BufferRead_Int(bufferPointer, tempPointer, totalHash, fileLen))
			{
				cout << "存储文件校验失败！" << endl;
				DataReset();
				AddressReset();
				return false;
			}

			// 将记忆切换器、记忆总步数恢复
			tempPointer = (char*)&memSwitcher;
			if (!BufferRead_Int(bufferPointer, tempPointer, totalHash, fileLen))
			{
				cout << "存储文件校验失败！" << endl;
				DataReset();
				AddressReset();
				return false;
			}
			tempPointer = (char*)&memStepNum;
			if (!BufferRead_Int(bufferPointer, tempPointer, totalHash, fileLen))
			{
				cout << "存储文件校验失败！" << endl;
				DataReset();
				AddressReset();
				return false;
			}

			// 将全文件校验值存入缓存
			if (totalHash != *(bufferPointer++))
			{
				cout << "存储文件校验失败！" << endl;
				DataReset();
				AddressReset();
				return false;
			}
			fileLen++;

			inFile.close();

			// 恢复初始路径
			AddressReset();

			return true;
		}
	}

	// 测试记录
	int TouchData(int saveNum)
	{
		/*
			返回值定义：
			0 损坏
			1 正常
			2 无法读取
		**/

		// 改动目标路径
		switch (saveNum)
		{
		case 1:
			strcat_s(address, "savefile\\savefile1.dat");
			break;
		case 2:
			strcat_s(address, "savefile\\savefile2.dat");
			break;
		case 3:
			strcat_s(address, "savefile\\savefile3.dat");
			break;
		case 4:
			strcat_s(address, "savefile\\savefile4.dat");
			break;
		case 5:
			strcat_s(address, "savefile\\savefile5.dat");
			break;
		default:
			return false;
		}

		ifstream inFile(address, ios_base::in | ios_base::binary);
		if (!inFile)
		{
			AddressReset();
			return 2;
		}
		else
		{
			inFile.read(&buffer[0], 2000);

			char* bufferPointer = &buffer[0]; // 缓冲区指针
			char* tempPointer = NULL; // 当前待存储数据指针
			int fileLen = 0; // 文件长度
			char fourHash = 0; // 四字节校验数
			char totalHash = 0; // 全文件校验数
			char complement = 0; // 补足

			// 检查棋盘
			tempPointer = &board[0][0][0];
			for (int i = 0; i < 242; i++)
			{
				fourHash = fourHash ^ *bufferPointer;
				totalHash = totalHash ^ *bufferPointer;
				tempPointer++;
				bufferPointer++;
				fileLen++;

				// 四字节一校验
				if (i % 4 == 3)
				{
					// 校验失败
					if (fourHash != *(bufferPointer++))
					{
						DataReset();
						AddressReset();
						return false;
					}
					fourHash = 0;
					fileLen++;
				}
			}
			// 补足两字节 计算校验
			bufferPointer += 2;
			if (fourHash != *(bufferPointer++))
			{
				DataReset();
				AddressReset();
				return false;
			}
			fileLen += 3;

			// 检查记忆棋盘
			tempPointer = &memBoard[0][0][0];
			for (int i = 0; i < 242; i++)
			{
				fourHash = fourHash ^ *bufferPointer;
				totalHash = totalHash ^ *bufferPointer;
				tempPointer++;
				bufferPointer++;
				fileLen++;

				// 四字节一校验
				if (i % 4 == 3)
				{
					// 校验失败
					if (fourHash != *(bufferPointer++))
					{
						DataReset();
						AddressReset();
						return false;
					}
					fourHash = 0;
					fileLen++;
				}
			}
			// 补足两字节 计算校验
			bufferPointer += 2;
			if (fourHash != *(bufferPointer++))
			{
				DataReset();
				AddressReset();
				return false;
			}
			fileLen += 3;

			// 检查可行位置权指标
			tempPointer = (char*)&weightTable[0][0];
			for (int i = 0; i < 121; i++)
			{
				if (!BufferRead_Int(bufferPointer, tempPointer, totalHash, fileLen, false))
				{
					DataReset();
					AddressReset();
					return false;
				}
			}

			// 检查棋子数
			tempPointer = (char*)&pieceNum[0][0];
			for (int i = 0; i < 4; i++)
			{
				if (!BufferRead_Int(bufferPointer, tempPointer, totalHash, fileLen, false))
				{
					DataReset();
					AddressReset();
					return false;
				}
			}

			// 检查记忆棋子数
			tempPointer = (char*)&memPieceNum[0][0];
			for (int i = 0; i < 4; i++)
			{
				if (!BufferRead_Int(bufferPointer, tempPointer, totalHash, fileLen, false))
				{
					DataReset();
					AddressReset();
					return false;
				}
			}

			// 检查切换器、悔棋旗标、游戏模式、总步数
			tempPointer = (char*)&switcher;
			if (!BufferRead_Int(bufferPointer, tempPointer, totalHash, fileLen, false))
			{
				DataReset();
				AddressReset();
				return false;
			}
			tempPointer = (char*)&ifRegretOK;
			if (!BufferRead_Int(bufferPointer, tempPointer, totalHash, fileLen, false))
			{
				DataReset();
				AddressReset();
				return false;
			}
			tempPointer = (char*)&mode;
			if (!BufferRead_Int(bufferPointer, tempPointer, totalHash, fileLen, false))
			{
				DataReset();
				AddressReset();
				return false;
			}
			tempPointer = (char*)&stepNum;
			if (!BufferRead_Int(bufferPointer, tempPointer, totalHash, fileLen, false))
			{
				DataReset();
				AddressReset();
				return false;
			}

			// 检查记忆切换器、记忆游戏模式
			tempPointer = (char*)&memSwitcher;
			if (!BufferRead_Int(bufferPointer, tempPointer, totalHash, fileLen, false))
			{
				DataReset();
				AddressReset();
				return false;
			}
			tempPointer = (char*)&memStepNum;
			if (!BufferRead_Int(bufferPointer, tempPointer, totalHash, fileLen, false))
			{
				DataReset();
				AddressReset();
				return false;
			}

			// 检查全文件校验值
			if (totalHash != *(bufferPointer++))
			{
				DataReset();
				return false;
			}
			fileLen++;

			inFile.close();

			// 恢复初始路径
			AddressReset();

			return true;
		}
	}

	// 设定AI搜索深度
	void SetAIDepth(int depth)
	{
		AISearchDepth = depth;
	}

	// 获取AI搜索深度
	int GetAIDepth()
	{
		return AISearchDepth;
	}

	// 随机设定AI风格
	void RandomAIStyle()
	{
		srand(time(0));
		int ran = rand() % 4;
		int copyStyle[6] = { 115694 , 123091 ,138167 ,100713 ,51506 ,23551 };
		int assimilationStyle[6] = { 207082 ,217662, 193060, 208450,178570,151395 };
		double biasStyle[6][6] = { { 0.981614, 1.09074, 1.0564, 1.08703, 1, 1}, { 0.925903, 1.14159, 1.04506, 1.07958, 1, 1} ,
								   { 0.950083, 1.10536, 1.04174, 1.1278, 1, 1 },{ 0.98816, 1.04445, 1.07204, 1.03039, 1, 1 },
								   { 0.540355, 0.742633, 0.653016, 0.719037, 1, 1 }, { 0.571544, 0.629665, 0.550777, 0.685479, 1, 1 } };
		ASSILIMATION_WEIGHT = assimilationStyle[ran];
		COPY_WEIGHT = copyStyle[ran];
		for (int i = 0; i < 6; i++)
			biasTable[i] = biasStyle[ran][i];
	}

};