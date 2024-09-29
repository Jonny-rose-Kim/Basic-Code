#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include "block.h"
#include <conio.h> //방향키  
#pragma warning (disable:4996)
#define UP 72
#define DOWN 80
#define LEFT 75
#define RIGHT 77
#define SPACEBAR 32
#define main_X 25  //25 X 30 의 블록임 (0,0)부터 콘솔창에 출력함
#define main_Y 30
int SCORE = 95;
enum GAME_MODE { EXPERT = 30, ADVANCED = 60, BEGINNER = 200 };
int is_Collision(int r, int block_x, int block_y);
void fix_block(int r, int xpos, int ypos);
int frame[main_Y][main_X];
void drawBlock(int x, int y, int n) {
	HANDLE hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
	int colors[] = { 12, 9, 10, 14, 11, 13, 5 };  // 블록 유형별 색상 코드
	SetConsoleTextAttribute(hConsoleOut, colors[n % 7]);  // 색상 설정

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (blockModel[n][i][j] == 1) {
				COORD pos = { 2 * (x + i), y + j };
				SetConsoleCursorPosition(hConsoleOut, pos);
				printf("\u25A0");
			}
		}
	}
	SetConsoleTextAttribute(hConsoleOut, 7);  // 기본 색상으로 복원
}
void deleteBlock(int x, int y, int n) {
	HANDLE hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (blockModel[n][i][j] == 1) {
				COORD pos = { 2 * (x + i), y + j };
				SetConsoleCursorPosition(hConsoleOut, pos);
				printf(" ");
			}
		}
	}
}
void scan_arrow(int key, int const_r, int* r, int* xpos, int* ypos) {
	int check = 0; int first_r = *r;
	switch (key) {
	case UP: {
		*r += 1;
		if (*r > const_r + 3)
			*r = const_r;
		if (is_Collision(*r, *xpos, *ypos) >= 1) {
			drawBlock(*xpos, *ypos, first_r);
			*r = first_r;
			break;
		}
		else {
			deleteBlock(*xpos, *ypos, first_r);
			drawBlock(*xpos, *ypos, *r);
		}
		break;
	}
	case DOWN:
		if (is_Collision(*r, *xpos, *ypos + 1) >= 1)
			break;
		deleteBlock(*xpos, *ypos, *r);
		*ypos = *ypos + 1;
		drawBlock(*xpos, *ypos, *r);
		break;
	case LEFT:
		if (is_Collision(*r, *xpos - 1, *ypos) >= 1) {
			//	printf("충돌됐어요!\n");
			break;
		}
		deleteBlock(*xpos, *ypos, *r);
		*xpos = *xpos - 1;
		drawBlock(*xpos, *ypos, *r);
		break;
	case RIGHT:
		if (is_Collision(*r, *xpos + 1, *ypos) >= 1) {
			//printf("충돌됐어요!\n");
			break;
		}
		deleteBlock(*xpos, *ypos, *r);
		*xpos = *xpos + 1;
		drawBlock(*xpos, *ypos, *r);
		break;
	case SPACEBAR:
		deleteBlock(*xpos, *ypos, *r);
		while (1) {
			if (is_Collision(*r, *xpos, *ypos + 1) != 0) {  // 충돌 검사
				break;  // 충돌이 있으면 루프 종료
			}
			*ypos += 1;  // 충돌이 없다면 y 위치 증가
		}
		fix_block(*r, *xpos, *ypos);
		drawBlock(*xpos, *ypos, *r);


	}
}
//x,y방향으로 커서를 이동하는 함수
void gotoxy(int x, int y) {
	COORD pos = { x,y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}
//블록 쌓는 함수
void fix_block(int r, int xpos, int ypos) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (blockModel[r][j][i] == 1) {
				frame[ypos + i][2 * (xpos + j)] = 2;
				frame[ypos + i][2 * (xpos + j) + 1] = 2;
			}
		}
	}
}
//블록 충돌 처리 함수
int is_Collision(int r, int block_x, int block_y) {
	/*parameter: ar[main_Y][main_X] 는 (2,2)부터 값이 유효함*/
	//게임 벽에 닿았을때
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (blockModel[r][j][i] == 1) {
				//이 이하부터는 블록모델의 값이 1일때 실행됨
				//각 블록의 element에 대하여 충돌여부를 판단
				int x = 2 * (block_x + j);
				int y = block_y + i;
				//좌우벽에 충돌했을때
				if (frame[y][x] == 1)
					return 1;
				//블록에 충돌했을때
				if (frame[y][x] == 2) {
					//printf("블록과 충돌했습니다");
					return 2;
				}
				//바닥에 충돌했을때
				if (frame[y][x] == 3)
					return 3;
			}
		}
	}
	//충돌하지 않음
	return 0;
}
// 초기 게임 프레임을 만들고 출력하는 함수
void game_frame() {
	for (int y = 0; y < main_Y; y++)
		for (int x = 0; x < main_X; x++) {
			if (x == 0 && y == 0) {
				printf("┌");
			}
			else if (x < main_X - 1 && x > 0 && y == 0)//윗변
				printf("─");
			else if (y > 0 && y < main_Y - 1 && x == 0) //왼쪽 변
				printf("│");
			else if (x == main_X - 1 && y == 0)
				printf("┐\n");
			else if (x == main_X - 1 && y > 0 && y < main_Y - 1)//오른쪽 변
				printf("│\n");
			else if (y == main_Y - 1 && x == 0)
				printf("└");
			else if (y == main_Y - 1 && x > 0 && x < main_X - 1)//밑변
				printf("─");
			else if (x == main_X - 1 && y == main_Y - 1)
				printf("┘");
			else
				printf(" ");
		}

}
//가상 테트리스 정보를 배열에 넣고 출력하는 함수
void game_virtual_frame() {
	for (int y = 0; y < main_Y; y++)
		for (int x = 0; x < main_X; x++) {
			if (y == 0)
				frame[y][x] = 1;
			else if (y > 0 && (x == 0 || x == 1))//
				frame[y][x] = 1;
			else if (y > 0 && (x == main_X - 1)) //|| x == main_X - 2
				frame[y][x] = 1;
			else if (y == main_Y - 1)
				frame[y][x] = 3;
			else
				frame[y][x] = 0;
		}
}
//가상 게임 프레임 출력
void print_vr_game_frame() {
	for (int y = 0; y < main_Y; y++) {
		for (int x = 0; x < main_X; x++) {
			gotoxy(2 * main_X + x, y);
			printf("%d", frame[y][x]);
			if (x == main_X - 1) {
				gotoxy(main_X + main_X + 3, y);
				printf("1");
			}
		}
	}
}
//커서 깜빡이 제거
void delete_cursor_blink() {
	HANDLE hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO curCursorInfo;
	GetConsoleCursorInfo(hConsoleOut, &curCursorInfo);
	curCursorInfo.bVisible = 0;
	SetConsoleCursorInfo(hConsoleOut, &curCursorInfo);
}
//새로운 블록 생성
void create_new_block(int* r, int* const_r, int* next_r, int* xpos, int* ypos) {
	//다음 블록이 그려진 프레임에 있는 블록을 지워준다.
	deleteBlock((main_X + 7) / 2, 2, *next_r);
	*xpos = 4;
	*ypos = 1;
	*r = *next_r;
	*const_r = *r;
	*next_r = ((rand() % 7) * 4);
}
//다음 생성될 블록 표시
void visualize_next_block(int next_r) {
	drawBlock((main_X + 7) / 2, 2, next_r);
}
//다음 생성될 블록 틀 만들기
void print_next_block_frame() {
	gotoxy(main_X + 5, 1);
	for (int y = 0; y < 7; y++)
		for (int x = 0; x < 10; x++) {
			if (x == 0 && y == 0) {
				gotoxy(main_X + 5 + x, y);
				printf("┌");
			}
			else if (x < 9 && x > 0 && y == 0) {//윗변
				gotoxy(main_X + 5 + x, y);
				printf("─");
			}
			else if (y > 0 && y < 6 && x == 0) { //왼쪽 변
				gotoxy(main_X + 5 + x, y);
				printf("│");
			}
			else if (x == 9 && y == 0) {
				gotoxy(main_X + 5 + x, y);
				printf("┐");
			}
			else if (x == 9 && y > 0 && y < 6) {//오른쪽 변
				gotoxy(main_X + 5 + x, y);
				printf("│");
			}
			else if (y == 6 && x == 0) {
				gotoxy(main_X + 5, y);
				printf("└");
			}
			else if (y == 6 && x > 0 && x < 9) {//밑변
				gotoxy(main_X + 5 + x, y);
				printf("─");
			}
			else if (x == 9 && y == 6) {
				gotoxy(main_X + 5 + x, y);
				printf("┘");
			}
		}
}
//각행에 대하여 블록들로 가득 찼는지 확인 후 - 최신화
int is_row_full() {
	int count = 0;
	for (int y = main_Y - 2; y > 0; y--) {
		int isFull = 1;  //가득 찼다고 가정

		for (int x = 2; x < main_X - 2; x++) {
			if (frame[y][x] != 2) {  // 한개라도 2가 아니면 isFull 은 0
				isFull = 0;
				break;
			}
		}
		//각행이 가득찼다고 감지되면 
		if (isFull) {
			count++;
			for (int i = y; i > 0; i--) {
				for (int x = 2; x < main_X - 2; x += 2) {
					//위에 행에있는 정보를 받아서 온다.
					frame[i][x] = frame[i - 1][x];
					frame[i][x + 1] = frame[i - 1][x + 1];
					//받은 정보 대로 게임판에 출력해준다
					if (frame[i - 1][x] == 2) {
						gotoxy(x, i);
						printf("\u25A0");
					}
					if (frame[i - 1][x] == 0) {
						gotoxy(x, i);
						printf(" ");
					}
				}
			}
			//맨 상단 행 초기화
			for (int x = 2; x < main_X - 2; x++) {
				frame[1][x] = 0;  // Top row is now empty
			}
			y++;
		}
	}
	return count;
}
int calc_score(int cnt) {

	int score = 5;
	if (cnt == 0)
		return 0;
	for (int i = 1; i < cnt; i++) {
		score *= 5;
	}

	return score;
}
void print_score(int* Game_Mode) {
	//난이도 설정
	if (SCORE >= 100 && SCORE < 200)
		*Game_Mode = ADVANCED;
	if (SCORE >= 200)
		*Game_Mode = EXPERT;

	if (SCORE < 100) {
		gotoxy(main_X + 5, 13);
		printf("현재 난이도: BEGINNER");
	}
	else if (SCORE < 200) {
		gotoxy(main_X + 5, 13);
		printf("현재 난이도: ADVANCED");
	}
	else {
		gotoxy(main_X + 5, 13);
		printf("현재 난이도: EXPERT");
	}
	gotoxy(main_X + 5, 14);
	printf("S C O R E: %d", SCORE);
	gotoxy(main_X + 5, main_Y - 5);
	printf("----점수별 게임 난이도----");
	gotoxy(main_X + 5, main_Y - 4);
	printf("0점이상: BEGINNER 100점이상: ADVANCED 200점이상: EXPERT");
	gotoxy(main_X + 5, main_Y - 3);
	printf("한 줄을 없앨 때마다 5점씩, N줄을 동시에 없애면 5의 N점씩 얻습니다!");
}
int main() {
	int r = (rand() % 7) * 4;	//r은 블록의 종류를 결정짓는 변수
	int next_r = (rand() % 7) * 4;	//next_r은 다음 블록의 종류를 결정짓는 변수
	int const_r = r;
	int n = 0;
	int xpos = 4;
	int ypos = 1;
	int Game_Mode = BEGINNER;
	delete_cursor_blink();
	game_frame();
	game_virtual_frame();
	srand(time(NULL));
	print_next_block_frame();
	print_score(Game_Mode);
	visualize_next_block(next_r);

	while (1) {
		int key = 0;
		int count = 0;
		for (int i = 0; i < Game_Mode; i++) {
			if (_kbhit() != 0) {	//키보드 이벤트를 입력받았을때
				key = _getch();
				if (key == 224) {
					key = _getch();
					scan_arrow(key, const_r, &r, &xpos, &ypos);
				}
				else if (key == SPACEBAR) {
					scan_arrow(key, const_r, &r, &xpos, &ypos);
					while (i < Game_Mode)          //버그수정
						i++;
					continue;
				}
				Sleep(10);
			}
			if (i % (Game_Mode / 2) == 0) {
				//만약 블록이 바닥이나 다른 블룩에 닿으면 그 위치에 고정시킨다
				if (is_Collision(r, xpos, ypos + 1) == 2 || is_Collision(r, xpos, ypos + 1) == 3) {
					fix_block(r, xpos, ypos);
					//점수계산
					count = is_row_full();
					SCORE += calc_score(count);
					print_score(&Game_Mode);
					create_new_block(&r, &const_r, &next_r, &xpos, &ypos);	//새로운 블록 생성
					if (is_Collision(r, xpos, ypos) == 2) { exit(1); }
					drawBlock(xpos, ypos, r);				//동작을 자연스럽게 하기 위해 넣음
					visualize_next_block(next_r); //다음 블록이 어떤 블록인지 시각화한다
				}
				//그렇지 않다면 자동으로 내려오게 한다.
				else {
					deleteBlock(xpos, ypos, r);
					Sleep(10);
					ypos++;
					drawBlock(xpos, ypos, r);
				}
			}
			Sleep(1);
			//print_vr_game_frame();
		}
	}
}





