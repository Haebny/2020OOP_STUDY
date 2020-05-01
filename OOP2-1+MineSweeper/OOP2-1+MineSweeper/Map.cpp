#include "Map.h"
#include <iostream>
using namespace std;


// ������ �Լ�
Map::Map() {
	width = 6;
	height = 6;
	num_mines = 10;
	//num_flags = num_mines;
	size = width * height;

	// �����Ҵ� -> �ּ��� �Ҹ����Լ����� delete �ʿ�!
	screen = new char[width*height + 1];
	m_map = new bool[width*height + 1];

	Borland::initialize();
	// ��ũ�� �ʱ�ȭ -> ��ư�� ��� '����'���·� ����
	memset(screen, Close, size);
	screen[size] = '\0'; // ���ڿ��� �������� NULL���ڸ� �־��ֱ�!

	LayMines(num_mines);
	GameLoop();
}

Map::Map(int w, int h)
	: width(w), height(h), num_mines(12), size(w*h), screen(new char[w*h + 1]), m_map(new bool[w*h + 1]) // ��� �̴ϼȶ�����
{
	Borland::initialize();
	memset(screen, Close, size); //��ũ�� ���� : ��� ����
	screen[size] = '\0'; // ���ڿ��� ������
	
	LayMines(num_mines);
	GameLoop();
}

// ���� �Ҹ����Լ�
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

// ȭ�� �����
void Map::ClearScreen()
{
	COORD Coor = { 0, 0 }; // ���ϴ� Ŀ����ǥ
	DWORD dw; // ���̸� ��ȯ�� �� = ���ۿ� ��ϵ� ���� ���� ����

	// Ư�� ���ڷ� ȭ���� ä��� = �����
	// ���ϴ� ��ġ��, ���ϴ� ���ڷ�, ���ϴ� ���̸�ŭ, ���ϴ� (Ŀ��)��ǥ����, ���̸� ��ȯ�� ������
	FillConsoleOutputCharacter(GetStdHandle(STD_OUTPUT_HANDLE), ' ', 80 * 25, Coor, &dw);

	return;
}

// ȭ�� �׸���
void Map::DrawScreen()
{
	ClearScreen(); // ����� ������ ȭ�鿡 �׸��ô�
	Borland::gotoxy(buttonPos[0].x, buttonPos[0].y); // �÷��̾ ���� �� �ִ� 0��° ���� ��ǥ
	int idx = 0; // �ش� ���� ��ȣ�� �ֱ�

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			idx = j + i * width;
			// �ش� ���� ��ǥ�� �̵�
			buttonPos[idx].x = Borland::wherex();
			buttonPos[idx].y = Borland::wherey();
			buttonPos[idx].index = idx;

			// ��ũ���� ��ȣ�� ǥ���ϱ� (��ȣ ������� �ϼ���!)
			if (screen[idx] == Close)
				printf("��");			// ����
			else if (screen[idx] == Flag)
				printf("��");			// ���
			else {						// ����
				if (m_map[idx] == false)
					printf("%2d", CountMines(buttonPos[idx].x, buttonPos[idx].y));	// ����(8�濡 �ִ� ������ ����)
				else
				{
					printf("��");					// ����
					isGameOver = true;				// ���ӿ���
				}
			}
		}
		printf("\n");
	}

	Borland::gotoxy(buttonPos[0].x, buttonPos[width*height + 1].y); // Ŀ�� ��ġ �̵�
	cout << "������ �� : " << GetRemainMines() << endl; // �÷���Ÿ�� �־��!

	Sleep(80);
	return;
}

// �ֺ� 8�濡 �ż��� ������ ���� ����
int Map::CountMines(int posX, int posY)
{
	int count = 0; // 8�� ���� ������ ��
	int idx, x, y;

	for (int row = -1; row < 2; row++) {
		for (int col = -1; col < 2; col++) {
			x = posX + col * 2;
			y = posY + row;

			idx = GetIndex(x, y);

			if (idx < 0 || idx > size)	continue;	// ���� ���� ������� ����

			if (m_map[idx] == true)			count++;	// ���� �߰�!
		}
	}

	return count;
}

// ��ǥ���� �Ѱ� �ε����� �ޱ�
int Map::GetIndex(int x, int y) {
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {

			if (x == buttonPos[j + (i*width)].x	&& y == buttonPos[j + (i*width)].y)
				return buttonPos[j + (i*width)].index;
		}
	}

	return -1;
}

// ���� ������ ��
int Map::GetRemainMines() 
{
	int count = num_mines;

	// ��ũ������ ����� ������ŭ ���ֱ�
	for (int i = 0; i < height*width; i++) {
		if (screen[i] == Flag)	count--;
	}

	return count;
}