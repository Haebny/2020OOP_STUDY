#pragma once
#include "Utils.h"


enum ScreenState {
	Close = 0, // 닫힘
	Open = 1, // 열림
	Flag = 2 // 깃발 꽂음
};

class Map
{
private:
	int width;			// 맵의 가로길이
	int height;			// 맵의 세로길이
	int size;			// 맵 크기

	int num_mines;		// 심어진 지뢰 개수

	char *screen;	// 플레이어에게 보여지는 화면
	bool *m_map;	// 매설지뢰의 위치

	Position buttonPos[81]; // x, y좌표값
	Position mousePos;

	static Map *instance;

	Map();
	Map(int w = 9, int h = 9); // 멤버 이니셜라이저
	virtual ~Map();

public:
	void ClearScreen();
	void DrawScreen();
	int CountMines(int posX, int posY);
	int GetIndex(int x, int y);
	int GetRemainMines();
};

