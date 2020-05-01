// O2CUBE-OOP-STUDY_MineSweeper.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
// 2020.04.21 지뢰찾기 구현
// 2020.04.28 클래스 분할
#include <iostream>
#include <ctime>
#include "Utils.h"

using namespace std;


// 지뢰찾기 통합본
// 플레이어가 보는 화면의 상태
enum ScreenState {
	Close = 0, // 닫힘
	Open = 1, // 열림
	Flag = 2 // 깃발 꽂음
};

class MineSweeper
{
private:
	int width;			// 맵의 가로길이
	int height;			// 맵의 세로길이
	int num_mines;		// 심어진 지뢰 개수
	int num_flags;		// 놓을 수 있는 깃발 개수
	int size;			// 맵 크기

	bool isPlaying;		// 게임이 진행중인지 확인
	bool isGameOver;	// 지뢰를 팠을 경우 패배

	char *screen;	// 플레이어에게 보여지는 화면
	bool *m_map;	// 매설지뢰의 위치

	//int second; // 플레이타임(초) 넣어보기

	Position buttonPos[81]; // x, y좌표값
	Position mousePos;

public:
	// 기본 생성자 함수
	MineSweeper()
	{
		width = 6;
		height = 6;
		num_mines = 10;
		num_flags = num_mines;
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


	// 생성자 함수
	MineSweeper(int w = 9, int h = 9)
		: width(w), height(h), num_mines(12), size(w*h), screen(new char[w*h + 1]), m_map(new bool[w*h + 1]), num_flags(num_mines) // 멤버 이니셜라이저
	{
		Borland::initialize();
		memset(screen, Close, size); //스크린 상태 : 모두 닫힘
		screen[size] = '\0'; // 문자열의 마지막

		isPlaying = true;
		isGameOver = false;

		LayMines(num_mines);
		GameLoop();
	}


	// 가상 소멸자함수
	virtual ~MineSweeper() {
		if (screen != nullptr) {
			delete[] screen;
			screen = nullptr;
		}

		if (m_map != nullptr) {
			delete[] m_map;
			m_map = nullptr;
		}
	}


	// 지뢰 매설 함수
	void LayMines(int num_mines)
	{
		// 지뢰 매설
		// 1. 매설된 지뢰가 있으면 1(TRUE), 없으면 0(FALSE)
		// 2. 지뢰의 매설 위치는 랜덤

		// 지뢰밭 초기화
		memset(m_map, false, size);
		m_map[size] = '\0';

		int index;
		int mine = num_mines; // 심어야 할 지뢰의 수
		srand((unsigned)time(NULL));

		// 랜덤 매설
		do
		{
			index = rand() % size;
			// 지뢰가 안 깔려 있는 땅이면 지뢰 심기
			if (m_map[index] == false)
			{
				m_map[index] = true;
				mine--;
			}
		} while (mine > 0);

		return;
	}


	/// 화면 지우기
	void ClearScreen()
	{
		COORD Coor = { 0, 0 }; // 원하는 커서좌표
		DWORD dw; // 넓이를 반환할 곳 = 버퍼에 기록된 문자 수를 받음

		// 특정 문자로 화면을 채우기 = 지우기
		// 원하는 위치에, 원하는 문자로, 원하는 넓이만큼, 원하는 (커서)좌표부터, 넓이를 반환할 곳까지
		FillConsoleOutputCharacter(GetStdHandle(STD_OUTPUT_HANDLE), ' ', 80 * 25, Coor, &dw);

		return;
	}


	/// 화면 그리기
	void DrawScreen()
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
						printf("%2d", CountMine(buttonPos[idx].x, buttonPos[idx].y));	// 숫자(8방에 있는 지뢰의 개수)
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
	int CountMine(int posX, int posY)
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


	// 남은 지뢰의 수
	int GetRemainMines() {
		int count = num_mines;

		// 스크린에서 깃발의 개수만큼 빼주기
		for (int i = 0; i < height*width; i++) {
			if (screen[i] == Flag)	count--;
		}

		return count;
	}


	bool CheckState()
	{
		// 패배
		if (isGameOver) {
			Borland::gotoxy(buttonPos[width*height].x, buttonPos[width*height + 1].y); // 출력하고싶은 위치에 출력
			cout << "YOU LOSE !";
			isPlaying = false;
			return false;
		}

		int count = 0;
		bool isClear = false;

		// 승리조건
		// 지뢰위치에 모두 깃발이 꽂혀있다면 승리
		for (int i = 0; i < size; i++) {
			if (m_map[i] == true && screen[i] == Flag)
				count++;
		}

		// 또는 지뢰가 아닌 칸이 모두 열려있을 경우 승리
		for (int i = 0; i < size; i++) {
			if (m_map[i] == false && screen[i] == Open)
				isClear = true;
			else {
				isClear = false;
				break;
			}
		}

		// 승리
		if (count == num_mines || isClear)
		{
			Borland::gotoxy(buttonPos[width*height + width].x, buttonPos[width*height + 1].y); // 출력하고싶은 위치에 출력
			cout << "YOU WIN !!";
			isPlaying = false;
			return false;
		}

		return true;
	}


	// 마우스 입력
	void GetMouseButtonDown() {
		INPUT_RECORD ir;
		DWORD ic;
		SetConsoleMode(hin, ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT);

		while (1) {

			pos.X = -1;
			pos.Y = -1;
			ReadConsoleInput(hin, &ir, 1, &ic);

			if (ir.EventType == MOUSE_EVENT) {
				// 좌클릭
				if (ir.Event.MouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED)
				{
					pos.X = ir.Event.MouseEvent.dwMousePosition.X;
					pos.Y = ir.Event.MouseEvent.dwMousePosition.Y;
					SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);

					// 클릭좌표가 범위 벗어남
					if (pos.X < buttonPos[0].x || pos.X > buttonPos[size - 1].x || pos.Y < buttonPos[0].y || pos.Y>buttonPos[size - 1].y)
						return;

					// 좌표 보정
					mousePos.x = pos.X;
					mousePos.y = pos.Y;
					int tmp = mousePos.x % 2;
					if (tmp == 1) mousePos.x--;

					DigMine(mousePos.x, mousePos.y);
				}

				// 우클릭
				else if (ir.Event.MouseEvent.dwButtonState & RIGHTMOST_BUTTON_PRESSED)
				{
					pos.X = ir.Event.MouseEvent.dwMousePosition.X;
					pos.Y = ir.Event.MouseEvent.dwMousePosition.Y;
					SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);

					// 클릭좌표가 범위 벗어남
					if (pos.X < buttonPos[0].x || pos.X > buttonPos[size - 1].x || pos.Y < buttonPos[0].y || pos.Y>buttonPos[size - 1].y)
						return;

					//좌표 보정
					mousePos.x = pos.X;
					mousePos.y = pos.Y;
					int tmp = mousePos.x % 2;
					if (tmp == 1) mousePos.x--;

					PutFlag(mousePos.x, mousePos.y);
				}

				DrawScreen();
			}

			if (!CheckState())		break;
		}

		return;
	}


	// 좌표값을 넘겨 인덱스값 받기
	int GetIndex(int x, int y) {
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {

				if (x == buttonPos[j + (i*width)].x	&& y == buttonPos[j + (i*width)].y)
					return buttonPos[j + (i*width)].index;
			}
		}

		return -1;
	}


	// 지뢰 찾기
	void DigMine(short x, short y)
	{
		// 범위 밖이면 검사하지 않음
		if (x < buttonPos[0].x || x > buttonPos[size - 1].x || y < buttonPos[0].y || y > buttonPos[size - 1].y)
			return;

		// index값 저장	
		int idx = GetIndex(x, y);

		// 받아온 index값이 -1이면 오류	
		if (idx == -1)	return;

		// 이미 열린 땅이면 파지 않아도 됨
		if (screen[idx] == Open) return;

		// 땅 열어주기
		screen[idx] = Open;

		// 지뢰를 팠으면 패배상태로 전환
		if (m_map[idx] == true && screen[idx] == Open) {
			isPlaying = false;
			return;
		}

		//주변탐색
		int count;
		count = CountMine(x, y);

		// 주변에 지뢰가 없다면 왼쪽 위부터 오른쪽 아래까지 순차적으로 땅을 파기
		if (count == 0) {
			DigMine(x - 2, y - 1);	//	왼쪽 위
			DigMine(x, y - 1);		//	위
			DigMine(x + 2, y - 1);	//	오른쪽 위
			DigMine(x - 2, y);		//	왼쪽
			DigMine(x + 2, y);		//	오른쪽
			DigMine(x - 2, y + 1);	//	왼쪽 아래
			DigMine(x, y + 1);		//	아래
			DigMine(x + 2, y + 1);	//	오른쪽 아래
		}
		return;
	}


	// 깃발 놓기
	void PutFlag(short x, short y)
	{
		int idx = GetIndex(x, y);	// index값 저장
		if (idx == -1) 	return;		// 받아온 index값이 -1이면 오류

		if (screen[idx] == Open)		return;

		if (screen[idx] == Close)	screen[idx] = Flag;
		else						screen[idx] = Close; //깃발 취소
	}


	// 게임 진행
	void GameLoop()
	{
		isPlaying = true;
		isGameOver = false;

		while (isPlaying) {
			DrawScreen();
			GetMouseButtonDown();
		}
		return;
	}
};


int main(void)
{
	MineSweeper ms(9, 9);
	return 0;
}