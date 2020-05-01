#include "Map.h"
#include <iostream>
using namespace std;


// 생성자 함수
Map::Map() {
	width = 6;
	height = 6;
	num_mines = 10;
	//num_flags = num_mines;
	size = width * height;

	// 동적할당 -> 최소한 소멸자함수에서 delete 필요!
	screen = new char[width*height + 1];
	m_map = new bool[width*height + 1];

	Borland::initialize();
	// 스크린 초기화 -> 버튼을 모두 '닫힘'상태로 세팅
	memset(screen, Close, size);
	screen[size] = '\0'; // 문자열의 마지막은 NULL문자를 넣어주기!

	LayMines(num_mines);
	GameLoop();
}

Map::Map(int w, int h)
	: width(w), height(h), num_mines(12), size(w*h), screen(new char[w*h + 1]), m_map(new bool[w*h + 1]) // 멤버 이니셜라이저
{
	Borland::initialize();
	memset(screen, Close, size); //스크린 상태 : 모두 닫힘
	screen[size] = '\0'; // 문자열의 마지막
	
	LayMines(num_mines);
	GameLoop();
}

// 가상 소멸자함수
Map::~Map()
{
	if (screen != nullptr) {
		delete[] screen;
		screen = nullptr;
	}

	if (m_map != nullptr) {
		delete[] m_map;
		m_map = nullptr;
	}
}

// 화면 지우기
void Map::ClearScreen()
{
	COORD Coor = { 0, 0 }; // 원하는 커서좌표
	DWORD dw; // 넓이를 반환할 곳 = 버퍼에 기록된 문자 수를 받음

	// 특정 문자로 화면을 채우기 = 지우기
	// 원하는 위치에, 원하는 문자로, 원하는 넓이만큼, 원하는 (커서)좌표부터, 넓이를 반환할 곳까지
	FillConsoleOutputCharacter(GetStdHandle(STD_OUTPUT_HANDLE), ' ', 80 * 25, Coor, &dw);

	return;
}

// 화면 그리기
void Map::DrawScreen()
{
	ClearScreen(); // 지우고 깨끗한 화면에 그립시다
	Borland::gotoxy(buttonPos[0].x, buttonPos[0].y); // 플레이어가 누를 수 있는 0번째 땅의 좌표
	int idx = 0; // 해당 땅에 번호를 주기

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			idx = j + i * width;
			// 해당 땅의 좌표로 이동
			buttonPos[idx].x = Borland::wherex();
			buttonPos[idx].y = Borland::wherey();
			buttonPos[idx].index = idx;

			// 스크린에 기호로 표현하기 (기호 마음대로 하세요!)
			if (screen[idx] == Close)
				printf("■");			// 닫힘
			else if (screen[idx] == Flag)
				printf("♨");			// 깃발
			else {						// 열림
				if (m_map[idx] == false)
					printf("%2d", CountMines(buttonPos[idx].x, buttonPos[idx].y));	// 숫자(8방에 있는 지뢰의 개수)
				else
				{
					printf("※");					// 지뢰
					isGameOver = true;				// 게임오버
				}
			}
		}
		printf("\n");
	}

	Borland::gotoxy(buttonPos[0].x, buttonPos[width*height + 1].y); // 커서 위치 이동
	cout << "지뢰의 수 : " << GetRemainMines() << endl; // 플레이타임 넣어보기!

	Sleep(80);
	return;
}

// 주변 8방에 매설된 지뢰의 개수 세기
int Map::CountMines(int posX, int posY)
{
	int count = 0; // 8방 내의 지뢰의 수
	int idx, x, y;

	for (int row = -1; row < 2; row++) {
		for (int col = -1; col < 2; col++) {
			x = posX + col * 2;
			y = posY + row;

			idx = GetIndex(x, y);

			if (idx < 0 || idx > size)	continue;	// 범위 밖은 계산하지 않음

			if (m_map[idx] == true)			count++;	// 지뢰 발견!
		}
	}

	return count;
}

// 좌표값을 넘겨 인덱스값 받기
int Map::GetIndex(int x, int y) {
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {

			if (x == buttonPos[j + (i*width)].x	&& y == buttonPos[j + (i*width)].y)
				return buttonPos[j + (i*width)].index;
		}
	}

	return -1;
}

// 남은 지뢰의 수
int Map::GetRemainMines() 
{
	int count = num_mines;

	// 스크린에서 깃발의 개수만큼 빼주기
	for (int i = 0; i < height*width; i++) {
		if (screen[i] == Flag)	count--;
	}

	return count;
}