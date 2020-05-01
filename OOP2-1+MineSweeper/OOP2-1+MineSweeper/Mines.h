#pragma once
#include "Map.h"

class Mines
{
private:
	int num_mines;

	Map *screen;
	Mines();
public:
	void LayMines(int num_mines);
	void DigMine(short x, short y);
};

