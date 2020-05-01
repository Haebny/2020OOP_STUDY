#pragma once
#include "Utils.h"


enum ScreenState {
	Close = 0, // ����
	Open = 1, // ����
	Flag = 2 // ��� ����
};

class Map
{
private:
	int width;			// ���� ���α���
	int height;			// ���� ���α���
	int size;			// �� ũ��

	int num_mines;		// �ɾ��� ���� ����

	char *screen;	// �÷��̾�� �������� ȭ��
	bool *m_map;	// �ż������� ��ġ

	Position buttonPos[81]; // x, y��ǥ��
	Position mousePos;

	static Map *instance;

	Map();
	Map(int w = 9, int h = 9); // ��� �̴ϼȶ�����
	virtual ~Map();

public:
	void ClearScreen();
	void DrawScreen();
	int CountMines(int posX, int posY);
	int GetIndex(int x, int y);
	int GetRemainMines();
};

