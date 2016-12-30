#pragma once
#include<iostream>
#include<cmath>
#include<cstring>
#include<fstream>
#include<cstdlib>
#include<ctime>

using namespace std;



// ��Ȩֵ������
struct WeightedVector
{
	int startHori;
	int startVert;
	int endHori;
	int endVert;
	int weight;
};

// Ȩֵ��ֵ
int COPY_WEIGHT = 100000; // 8�������Ҹ���Ȩֵ
int ASSILIMATION_WEIGHT = 200000; // ͬ��Ȩֵ
double biasTable[6] = { 1,1,1,1,1,1 }; // ƫ��ֵ

// ���ȶ�Ȩֵ��ֵ
int UNSTABLE_COPY_WEIGHT = 100000; // 8�������Ҹ���Ȩֵ
int UNSTABLE_ASSILIMATION_WEIGHT = 200000; // ͬ��Ȩֵ
double UNSTABLE_biasTable[6] = { 1,1,1,1,1,1 }; // ƫ��ֵ

// AI�������
int AISearchDepth = 2;

class PlainBoard
{
public:
	// ####################
	// ###  ���ݳ�Ա��  ###
	// ####################

	// ���̾��� (��ǰ������һ������)
	char board[2][11][11];
	char memBoard[2][11][11];

	// �л��� ���л����̺����ӷ���
	int switcher;
	int memSwitcher;

	// ������ [״̬][�ס���]
	int pieceNum[2][2];
	int memPieceNum[2][2];

	// �ܲ���
	int stepNum;
	int memStepNum;


	// ####################
	// ###  ������Ա��  ###
	// ####################

	
};


// AI����
WeightedVector AIJudge(PlainBoard tempBoard, int tempDepth, int setDepth, int cutThresh)
{
	WeightedVector tempVector = { 0,0,0,0,INT_MIN }; // ��ǰ��Ĵ�Ȩֵ����
	WeightedVector deepVector = { 0,0,0,0,INT_MIN }; // ��һ��Ĵ�Ȩֵ����
	PlainBoard nextBoard;
	int tempWeight = 0;

	// �������ʼ��
	srand(time(0));

	// �ų�8�����ظ�����
	bool judgeTable[11][11];
	memset(judgeTable, 0, sizeof(judgeTable));

	// �ų�������16����
	bool useful16;

	// ö������
	for (int i = 2; i <= 8; i++)
	{
		for (int j = 2; j <= 8; j++)
		{
			// �ҵ�һ���ҷ�����
			if (tempBoard.board[tempBoard.switcher][i][j] == (tempBoard.switcher == 0 ? 'O' : '#'))
			{
				for (int p = -2; p <= 2; p++)
				{
					for (int q = -2; q <= 2; q++)
					{
						// �ҵ�һ���������
						if (tempBoard.board[tempBoard.switcher][i + p][j + q] == ' ')
						{
							// ���Ƶ�ǰ���̵���һ����������
							memcpy(&nextBoard.board[!tempBoard.switcher][0][0], &tempBoard.board[tempBoard.switcher][0][0], sizeof(char) * 121);
							nextBoard.switcher = !tempBoard.switcher;
							memcpy(nextBoard.pieceNum, tempBoard.pieceNum, sizeof(tempBoard.pieceNum));

							// Ȩֵ��0
							tempWeight = 0;

							// Ŀ�����̸��� + ����Ȩֵ
							// 8����
							if (p != -2 && p != 2 && q != -2 && q != 2)
							{	
								// ��ֹ8�����ظ�����
								if (judgeTable[i + p][j + q])
									continue;
								judgeTable[i + p][j + q] = true;

								tempWeight += COPY_WEIGHT;

								// ������һ���̵����Ժ�����
								nextBoard.pieceNum[!tempBoard.switcher][tempBoard.switcher]++;
								nextBoard.board[!tempBoard.switcher][i + p][j + q] = (tempBoard.switcher == 0 ? 'O' : '#');
							}
							// 16����
							else
							{
								// �ų�������16����
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

								// ������һ���̵����Ժ�����
								nextBoard.board[!tempBoard.switcher][i + p][j + q] = (tempBoard.switcher == 0 ? 'O' : '#');
								nextBoard.board[!tempBoard.switcher][i][j] = ' ';
							}

							// ͬ������
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

							// ����Ż�
							if (tempDepth <= setDepth)
							{
								if (tempVector.weight == INT_MIN)
									deepVector = AIJudge(nextBoard, tempDepth + 1, setDepth, INT_MAX);
								else
									deepVector = AIJudge(nextBoard, tempDepth + 1, setDepth, tempWeight - tempVector.weight);
								tempWeight -= deepVector.weight * biasTable[tempDepth - 1];
							}

							// �����
							//if (tempDepth == 0)
							//	tempWeight += rand() / 100;

							// �Ƚϴ˴�ö�������������������������
							if (tempWeight > tempVector.weight)
							{
								tempVector.weight = tempWeight;
								tempVector.startHori = i;
								tempVector.startVert = j;
								tempVector.endHori = i + p;
								tempVector.endVert = j + q;
							}

							// �����¼�֦����
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

// ���ȶ�AI����
WeightedVector UnstableAIJudge(PlainBoard tempBoard, int tempDepth, int setDepth, int cutThresh)
{
	WeightedVector tempVector = { 0,0,0,0,INT_MIN }; // ��ǰ��Ĵ�Ȩֵ����
	WeightedVector deepVector = { 0,0,0,0,INT_MIN }; // ��һ��Ĵ�Ȩֵ����
	PlainBoard nextBoard;
	int tempWeight = 0;

	// �������ʼ��
	srand(time(0));

	// �ų�8�����ظ�����
	bool judgeTable[11][11];
	memset(judgeTable, 0, sizeof(judgeTable));

	// �ų�������16����
	bool useful16;

	// ö������
	for (int i = 2; i <= 8; i++)
	{
		for (int j = 2; j <= 8; j++)
		{
			// �ҵ�һ���ҷ�����
			if (tempBoard.board[tempBoard.switcher][i][j] == (tempBoard.switcher == 0 ? 'O' : '#'))
			{
				for (int p = -2; p <= 2; p++)
				{
					for (int q = -2; q <= 2; q++)
					{
						// �ҵ�һ���������
						if (tempBoard.board[tempBoard.switcher][i + p][j + q] == ' ')
						{
							// ���Ƶ�ǰ���̵���һ����������
							memcpy(&nextBoard.board[!tempBoard.switcher][0][0], &tempBoard.board[tempBoard.switcher][0][0], sizeof(char) * 121);
							nextBoard.switcher = !tempBoard.switcher;
							memcpy(nextBoard.pieceNum, tempBoard.pieceNum, sizeof(tempBoard.pieceNum));

							// Ȩֵ��0
							tempWeight = 0;

							// Ŀ�����̸��� + ����Ȩֵ
							// 8����
							if (p != -2 && p != 2 && q != -2 && q != 2)
							{
								// ��ֹ8�����ظ�����
								if (judgeTable[i + p][j + q])
									continue;
								judgeTable[i + p][j + q] = true;

								tempWeight += UNSTABLE_COPY_WEIGHT;

								// ������һ���̵����Ժ�����
								nextBoard.pieceNum[!tempBoard.switcher][tempBoard.switcher]++;
								nextBoard.board[!tempBoard.switcher][i + p][j + q] = (tempBoard.switcher == 0 ? 'O' : '#');
							}
							// 16����
							else
							{
								// �ų�������16����
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

								// ������һ���̵����Ժ�����
								nextBoard.board[!tempBoard.switcher][i + p][j + q] = (tempBoard.switcher == 0 ? 'O' : '#');
								nextBoard.board[!tempBoard.switcher][i][j] = ' ';
							}

							// ͬ������
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

							// ����Ż�
							if (tempDepth <= setDepth)
							{
								if (tempVector.weight == INT_MIN)
									deepVector = UnstableAIJudge(nextBoard, tempDepth + 1, setDepth, INT_MAX);
								else
									deepVector = UnstableAIJudge(nextBoard, tempDepth + 1, setDepth, tempWeight - tempVector.weight);
								tempWeight -= deepVector.weight * UNSTABLE_biasTable[tempDepth];
							}

							// �����
							if (tempDepth == 0)
								tempWeight += rand() / 100;


							// �Ƚϴ˴�ö�������������������������
							if (tempWeight > tempVector.weight)
							{
								tempVector.weight = tempWeight;
								tempVector.startHori = i;
								tempVector.startVert = j;
								tempVector.endHori = i + p;
								tempVector.endVert = j + q;
							}

							// �����¼�֦����
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
	// ###  ���ݳ�Ա��  ###
	// ####################

	// ���̾��� (��ǰ������һ������)
	// �̳�

	// ����λ��Ȩָ��
	int weightTable[11][11];

	// �Ƿ��������
	int ifRegretOK;

	// ��Ϸģʽ
	int mode;

	// �����ַ
	char address[300];

	// ���ݻ�����
	char buffer[2000];

	// ####################
	// ###  ˽�к�����  ###
	// ####################

	// ��պ���
	void DataReset()
	{
		// ��ʼ��
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

		// ��ʼ4��
		board[0][2][2] = '#';
		board[0][8][8] = '#';
		board[0][2][8] = 'O';
		board[0][8][2] = 'O';

		// �������������趨
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

		// �������ݳ�ʼ��
		memcpy(memBoard, board, sizeof(board));
		memSwitcher = switcher;
		memcpy(memPieceNum, pieceNum, sizeof(pieceNum));
		memStepNum = stepNum;
	}

	// ����д���ݻ����������ͱ�����
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

	// �����ݻ������ָ����ݣ����ͱ�����
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

	// ·������
	void AddressReset()
	{
		address[strlen(address) - 22] = '\0';
	}

public:
	// ####################
	// ###  ���к�����  ###
	// ####################

	// ���캯��
	ChessBoard(char* argv[])
	{
		// ��ʼ��
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

		// ��ʼ4��
		board[0][2][2] = '#';
		board[0][8][8] = '#';
		board[0][2][8] = 'O';
		board[0][8][2] = 'O';

		// �������������趨
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

		// �洢��ַ����
		strcpy_s(address, argv[0]);
		address[strlen(address) - 22] = '\0';

		// �������ݳ�ʼ��
		memcpy(memBoard, board, sizeof(board));
		memSwitcher = switcher;
		memcpy(memPieceNum, pieceNum, sizeof(pieceNum));
		memStepNum = stepNum;
	}

	// �������� ��ʹ��Ĭ�ϣ�

	// �趨��Ϸģʽ
	void SetMode(int newMode)
	{
		mode = newMode;
		return;
	}

	// ��ȡ��Ϸģʽ
	int GetMode()
	{
		return mode;
	}

	// ���Ӻ���
	bool PutPiece(int baseHori, int baseVert, int newHori, int newVert)
	{
		// ����Ŀ��λ�÷ǿ�
		if (board[switcher][newHori + 1][newVert + 1] != ' ')
		{
			cout << "����Ŀ��λ�����ӣ��޷����ߣ�" << endl;
			return false;
		}
		// ������ʼλ����Ч
		else if (board[switcher][baseHori + 1][baseVert + 1] != (switcher == 0 ? 'O' : '#'))
		{
			cout << "������ʼλ����Ч���޷����ߣ�" << endl;
			return false;
		}
		// ����λ����Ч
		else
		{
			// 8������
			if (abs(newHori - baseHori) <= 1 && abs(newVert - baseVert) <= 1)
			{
				// Ŀ�����̸���
				for (int i = 2; i <= 8; i++)
					for (int j = 2; j <= 8; j++)
						board[!switcher][i][j] = board[switcher][i][j];

				// Ŀ���������
				pieceNum[!switcher][0] = pieceNum[switcher][0];
				pieceNum[!switcher][1] = pieceNum[switcher][1];

				// ���Ӹ���
				board[!switcher][newHori + 1][newVert + 1] = (switcher == 0 ? 'O' : '#');
				pieceNum[!switcher][switcher]++;

				// ͬ������
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

				// �������Ը���
				switcher = !switcher;
				stepNum++;

				return true;
			}
			// 16������
			else if (abs(newHori - baseHori) <= 2 && abs(newVert - baseVert) <= 2)
			{
				// Ŀ�����̸���
				for (int i = 2; i <= 8; i++)
					for (int j = 2; j <= 8; j++)
						board[!switcher][i][j] = board[switcher][i][j];

				// Ŀ���������
				pieceNum[!switcher][0] = pieceNum[switcher][0];
				pieceNum[!switcher][1] = pieceNum[switcher][1];

				// ����ת��
				board[!switcher][newHori + 1][newVert + 1] = (switcher == 0 ? 'O' : '#');
				board[!switcher][baseHori + 1][baseVert + 1] = ' ';

				// ͬ������
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

				// �������Ը���
				switcher = !switcher;
				stepNum++;

				return true;
			}
			// �������߷�Χ
			else
			{
				cout << "�������߷�Χ���޷����ߣ�" << endl;
				return false;
			}
		}
	}

	// ״̬���溯�����������ã�
	void SaveStatus()
	{
		// �������ݼ�¼
		memcpy(memBoard, board, sizeof(board));
		memSwitcher = switcher;
		memcpy(memPieceNum, pieceNum, sizeof(pieceNum));
		memStepNum = stepNum;
		ifRegretOK = true;
	}

	// ���庯��
	bool PickPiece()
	{
		// �������
		if (ifRegretOK)
		{
			// �������Ա�������ã�
			/*switcher = !switcher;
			stepNum--;
			ifRegretOK = false;*/

			// �������ݻָ�
			memcpy(board, memBoard, sizeof(board));
			switcher = memSwitcher;
			memcpy(pieceNum, memPieceNum, sizeof(pieceNum));
			stepNum = memStepNum;
			ifRegretOK = false;

			return true;
		}
		// ���ɻ���
		else if (pieceNum[switcher][switcher] == 2)
			cout << "��û���ӣ��������壿" << endl;
		else
			cout << "����ֻ��һ�Σ�����̰�ģ�" << endl;
		return false;
	}

	// AI����
	void AIMove()
	{
		WeightedVector moveVector;
		if (IsCritical())
			moveVector = AIJudge(*this, 1, 0, INT_MAX);
		else
			moveVector = AIJudge(*this, 1, AISearchDepth, INT_MAX);
		PutPiece(moveVector.startHori - 1, moveVector.startVert - 1, moveVector.endHori - 1, moveVector.endVert - 1);
	}

	// ���ȶ�AI����
	void UnstableAIMove()
	{
		WeightedVector moveVector;
		if (IsCritical())
			moveVector = UnstableAIJudge(*this, 1, 0, INT_MAX);
		else
			moveVector = UnstableAIJudge(*this, 1, 4, INT_MAX);
		PutPiece(moveVector.startHori - 1, moveVector.startVert - 1, moveVector.endHori - 1, moveVector.endVert - 1);
	}

	// ��ӡ����
	void Print()
	{
		cout << "�����Щ��Щ��Щ��Щ��Щ��Щ���" << endl;
		for (int i = 2; i <= 7; i++)
		{
			cout << "��";
			for (int j = 2; j <= 8; j++)
			{
				cout << (board[switcher][i][j] == '#' ? "��" : (board[switcher][i][j] == 'O' ? "��" : "  ")) << "��";
			}
			cout << endl << "�����੤�੤�੤�੤�੤�੤��" << endl;
		}
		cout << "��";
		for (int j = 2; j <= 8; j++)
		{
			cout << (board[switcher][8][j] == '#' ? "��" : (board[switcher][8][j] == 'O' ? "��" : "  ")) << "��";
		}
		cout << endl << "�����ة��ة��ة��ة��ة��ة���" << endl << endl << "�ף���" << pieceNum[switcher][0] << "��" << pieceNum[switcher][1] << endl << endl;
	}

	// �ֵ��жϺ���
	bool IsCritical()
	{
		if (pieceNum[switcher][0] + pieceNum[switcher][1] == 48)
			return true;
		else
			return false;
	}

	// ����жϺ��� ���������������ã�
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
					// �ҵ�һ����λ������Ƿ��ǿ���λ
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

	// ȡ����
	char *GetBoard()
	{
		return &board[switcher][0][0];
	}

	// ȡȫ��������
	int *GetTotalPieceNum()
	{
		return pieceNum[switcher];
	}

	// �������
	void ClearBoard()
	{
		DataReset();
		return;
	}

	// ƽ���ж�
	bool IsTie()
	{
		if (pieceNum[switcher][0] == pieceNum[switcher][1])
			return true;
		else if (stepNum == 200)
			return true;
		else
			return false;
	}

	// ȡ����������
	int GetPieceNum()
	{
		return pieceNum[switcher][0] + pieceNum[switcher][1];
	}

	// ȡ��ǰ������
	int GetPlayer()
	{
		return switcher;
	}

	// ȡӮ��
	bool IsWhiteWin()
	{
		if (pieceNum[switcher][0] > pieceNum[switcher][1])
			return true;
		else
			return false;
	}

	// ȡ��������
	int GetDifference()
	{
		return abs(pieceNum[switcher][0] - pieceNum[switcher][1]);
	}

	// ���¼
	bool SaveData(int saveNum)
	{
		char* bufferPointer = &buffer[0]; // ������ָ��
		char* tempPointer = NULL; // ��ǰ���洢����ָ��
		int fileLen = 0; // �ļ�����
		char fourHash = 0; // ���ֽ�У����
		char totalHash = 0; // ȫ�ļ�У����
		char complement = 0; // ����

		// �Ķ�Ŀ��·��
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
			cout << "�洢λ��Ϊ1��5��" << endl;
			return false;
		}


		// �����̴��뻺��
		tempPointer = &board[0][0][0];
		for (int i = 0; i < 242; i++)
		{
			fourHash = fourHash ^ *tempPointer;
			totalHash = totalHash ^ *tempPointer;
			memcpy(bufferPointer++, tempPointer++, sizeof(char));
			fileLen++;

			// ���ֽ�һУ��
			if (i % 4 == 3)
			{
				memcpy(bufferPointer++, &fourHash, sizeof(char));
				fileLen++;
				fourHash = 0;
			}
		}
		// �������ֽ� ����У��
		memcpy(bufferPointer++, &complement, sizeof(char));
		memcpy(bufferPointer++, &complement, sizeof(char));
		memcpy(bufferPointer++, &fourHash, sizeof(char));
		fileLen += 3;
		fourHash = 0;

		// ���������̴��뻺��
		tempPointer = &memBoard[0][0][0];
		for (int i = 0; i < 242; i++)
		{
			fourHash = fourHash ^ *tempPointer;
			totalHash = totalHash ^ *tempPointer;
			memcpy(bufferPointer++, tempPointer++, sizeof(char));
			fileLen++;

			// ���ֽ�һУ��
			if (i % 4 == 3)
			{
				memcpy(bufferPointer++, &fourHash, sizeof(char));
				fileLen++;
				fourHash = 0;
			}
		}
		// �������ֽ� ����У��
		memcpy(bufferPointer++, &complement, sizeof(char));
		memcpy(bufferPointer++, &complement, sizeof(char));
		memcpy(bufferPointer++, &fourHash, sizeof(char));
		fileLen += 3;
		fourHash = 0;

		// ������λ��Ȩָ����뻺��
		tempPointer = (char*)&weightTable[0][0];
		for (int i = 0; i < 121; i++)
		{
			BufferBuild_Int(bufferPointer, tempPointer, totalHash, fileLen);
		}

		// �����������뻺��
		tempPointer = (char*)&pieceNum[0][0];
		for (int i = 0; i < 4; i++)
		{
			BufferBuild_Int(bufferPointer, tempPointer, totalHash, fileLen);
		}

		// ���������������뻺��
		tempPointer = (char*)&memPieceNum[0][0];
		for (int i = 0; i < 4; i++)
		{
			BufferBuild_Int(bufferPointer, tempPointer, totalHash, fileLen);
		}

		// ���л�����������ꡢ��Ϸģʽ���ܲ������뻺��
		tempPointer = (char*)&switcher;
		BufferBuild_Int(bufferPointer, tempPointer, totalHash, fileLen);
		tempPointer = (char*)&ifRegretOK;
		BufferBuild_Int(bufferPointer, tempPointer, totalHash, fileLen);
		tempPointer = (char*)&mode;
		BufferBuild_Int(bufferPointer, tempPointer, totalHash, fileLen);
		tempPointer = (char*)&stepNum;
		BufferBuild_Int(bufferPointer, tempPointer, totalHash, fileLen);

		// �������л����������ܲ������뻺��
		tempPointer = (char*)&memSwitcher;
		BufferBuild_Int(bufferPointer, tempPointer, totalHash, fileLen);
		tempPointer = (char*)&memStepNum;
		BufferBuild_Int(bufferPointer, tempPointer, totalHash, fileLen);

		// ��ȫ�ļ�У��ֵ���뻺��
		memcpy(bufferPointer++, &totalHash, sizeof(char));
		fileLen++;


		ofstream outFile(address, ios_base::out | ios_base::binary);
		if (!outFile)
			cout << "�ļ���ʧ�ܣ�" << endl;
		else
		{
			outFile.write(&buffer[0], fileLen);
		}

		outFile.close();
		DataReset();

		// �ָ���ʼ·��
		AddressReset();

		return true;
	}

	// ����¼
	bool ReadData(int saveNum)
	{
		// �Ķ�Ŀ��·��
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
			cout << "�洢λ��Ϊ1��5��" << endl;
			return false;
		}

		ifstream inFile(address, ios_base::in | ios_base::binary);
		if (!inFile)
		{
			cout << "�ļ���ʧ�ܣ�" << endl;
			AddressReset();
			return false;
		}
		else
		{
			inFile.read(&buffer[0], 2000);

			char* bufferPointer = &buffer[0]; // ������ָ��
			char* tempPointer = NULL; // ��ǰ���洢����ָ��
			int fileLen = 0; // �ļ�����
			char fourHash = 0; // ���ֽ�У����
			char totalHash = 0; // ȫ�ļ�У����
			char complement = 0; // ����

			// �ָ�����
			tempPointer = &board[0][0][0];
			for (int i = 0; i < 242; i++)
			{
				fourHash = fourHash ^ *bufferPointer;
				totalHash = totalHash ^ *bufferPointer;
				memcpy(tempPointer++, bufferPointer++, sizeof(char));
				fileLen++;

				// ���ֽ�һУ��
				if (i % 4 == 3)
				{
					// У��ʧ��
					if (fourHash != *(bufferPointer++))
					{
						cout << "�洢�ļ�У��ʧ�ܣ�" << endl;
						DataReset();
						AddressReset();
						return false;
					}
					fourHash = 0;
					fileLen++;
				}
			}
			// �������ֽ� ����У��
			bufferPointer += 2;
			if (fourHash != *(bufferPointer++))
			{
				cout << "�洢�ļ�У��ʧ�ܣ�" << endl;
				DataReset();
				AddressReset();
				return false;
			}
			fileLen += 3;

			// �ָ���������
			tempPointer = &memBoard[0][0][0];
			for (int i = 0; i < 242; i++)
			{
				fourHash = fourHash ^ *bufferPointer;
				totalHash = totalHash ^ *bufferPointer;
				memcpy(tempPointer++, bufferPointer++, sizeof(char));
				fileLen++;

				// ���ֽ�һУ��
				if (i % 4 == 3)
				{
					// У��ʧ��
					if (fourHash != *(bufferPointer++))
					{
						cout << "�洢�ļ�У��ʧ�ܣ�" << endl;
						DataReset();
						AddressReset();
						return false;
					}
					fourHash = 0;
					fileLen++;
				}
			}
			// �������ֽ� ����У��
			bufferPointer += 2;
			if (fourHash != *(bufferPointer++))
			{
				cout << "�洢�ļ�У��ʧ�ܣ�" << endl;
				DataReset();
				AddressReset();
				return false;
			}
			fileLen += 3;

			// ������λ��Ȩָ��ָ�
			tempPointer = (char*)&weightTable[0][0];
			for (int i = 0; i < 121; i++)
			{
				if (!BufferRead_Int(bufferPointer, tempPointer, totalHash, fileLen))
				{
					cout << "�洢�ļ�У��ʧ�ܣ�" << endl;
					DataReset();
					AddressReset();
					return false;
				}
			}

			// ���������ָ�
			tempPointer = (char*)&pieceNum[0][0];
			for (int i = 0; i < 4; i++)
			{
				if (!BufferRead_Int(bufferPointer, tempPointer, totalHash, fileLen))
				{
					cout << "�洢�ļ�У��ʧ�ܣ�" << endl;
					DataReset();
					AddressReset();
					return false;
				}
			}

			// �������������ָ�
			tempPointer = (char*)&memPieceNum[0][0];
			for (int i = 0; i < 4; i++)
			{
				if (!BufferRead_Int(bufferPointer, tempPointer, totalHash, fileLen))
				{
					cout << "�洢�ļ�У��ʧ�ܣ�" << endl;
					DataReset();
					AddressReset();
					return false;
				}
			}

			// ���л�������������������ꡢ��Ϸģʽ���ܲ����ָ�
			tempPointer = (char*)&switcher;
			if (!BufferRead_Int(bufferPointer, tempPointer, totalHash, fileLen))
			{
				cout << "�洢�ļ�У��ʧ�ܣ�" << endl;
				DataReset();
				AddressReset();
				return false;
			}
			tempPointer = (char*)&ifRegretOK;
			if (!BufferRead_Int(bufferPointer, tempPointer, totalHash, fileLen))
			{
				cout << "�洢�ļ�У��ʧ�ܣ�" << endl;
				DataReset();
				AddressReset();
				return false;
			}
			tempPointer = (char*)&mode;
			if (!BufferRead_Int(bufferPointer, tempPointer, totalHash, fileLen))
			{
				cout << "�洢�ļ�У��ʧ�ܣ�" << endl;
				DataReset();
				AddressReset();
				return false;
			}
			tempPointer = (char*)&stepNum;
			if (!BufferRead_Int(bufferPointer, tempPointer, totalHash, fileLen))
			{
				cout << "�洢�ļ�У��ʧ�ܣ�" << endl;
				DataReset();
				AddressReset();
				return false;
			}

			// �������л����������ܲ����ָ�
			tempPointer = (char*)&memSwitcher;
			if (!BufferRead_Int(bufferPointer, tempPointer, totalHash, fileLen))
			{
				cout << "�洢�ļ�У��ʧ�ܣ�" << endl;
				DataReset();
				AddressReset();
				return false;
			}
			tempPointer = (char*)&memStepNum;
			if (!BufferRead_Int(bufferPointer, tempPointer, totalHash, fileLen))
			{
				cout << "�洢�ļ�У��ʧ�ܣ�" << endl;
				DataReset();
				AddressReset();
				return false;
			}

			// ��ȫ�ļ�У��ֵ���뻺��
			if (totalHash != *(bufferPointer++))
			{
				cout << "�洢�ļ�У��ʧ�ܣ�" << endl;
				DataReset();
				AddressReset();
				return false;
			}
			fileLen++;

			inFile.close();

			// �ָ���ʼ·��
			AddressReset();

			return true;
		}
	}

	// ���Լ�¼
	int TouchData(int saveNum)
	{
		/*
			����ֵ���壺
			0 ��
			1 ����
			2 �޷���ȡ
		**/

		// �Ķ�Ŀ��·��
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

			char* bufferPointer = &buffer[0]; // ������ָ��
			char* tempPointer = NULL; // ��ǰ���洢����ָ��
			int fileLen = 0; // �ļ�����
			char fourHash = 0; // ���ֽ�У����
			char totalHash = 0; // ȫ�ļ�У����
			char complement = 0; // ����

			// �������
			tempPointer = &board[0][0][0];
			for (int i = 0; i < 242; i++)
			{
				fourHash = fourHash ^ *bufferPointer;
				totalHash = totalHash ^ *bufferPointer;
				tempPointer++;
				bufferPointer++;
				fileLen++;

				// ���ֽ�һУ��
				if (i % 4 == 3)
				{
					// У��ʧ��
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
			// �������ֽ� ����У��
			bufferPointer += 2;
			if (fourHash != *(bufferPointer++))
			{
				DataReset();
				AddressReset();
				return false;
			}
			fileLen += 3;

			// ����������
			tempPointer = &memBoard[0][0][0];
			for (int i = 0; i < 242; i++)
			{
				fourHash = fourHash ^ *bufferPointer;
				totalHash = totalHash ^ *bufferPointer;
				tempPointer++;
				bufferPointer++;
				fileLen++;

				// ���ֽ�һУ��
				if (i % 4 == 3)
				{
					// У��ʧ��
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
			// �������ֽ� ����У��
			bufferPointer += 2;
			if (fourHash != *(bufferPointer++))
			{
				DataReset();
				AddressReset();
				return false;
			}
			fileLen += 3;

			// ������λ��Ȩָ��
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

			// ���������
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

			// ������������
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

			// ����л�����������ꡢ��Ϸģʽ���ܲ���
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

			// �������л�����������Ϸģʽ
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

			// ���ȫ�ļ�У��ֵ
			if (totalHash != *(bufferPointer++))
			{
				DataReset();
				return false;
			}
			fileLen++;

			inFile.close();

			// �ָ���ʼ·��
			AddressReset();

			return true;
		}
	}

	// �趨AI�������
	void SetAIDepth(int depth)
	{
		AISearchDepth = depth;
	}

	// ��ȡAI�������
	int GetAIDepth()
	{
		return AISearchDepth;
	}

	// ����趨AI���
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