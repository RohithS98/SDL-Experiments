#ifndef __GAMEBOARD_H__
#define __GAMEBOARD_H__

#include <stdlib.h>
#include <time.h>

const int BOARD_BLOCK_NUM = 4;

enum move_type {
	MOVE_UP = 0,
	MOVE_LEFT = 1,
	MOVE_DOWN = 2,
	MOVE_RIGHT = 3,
	NO_MOVE
};

void copyBoard(int a[BOARD_BLOCK_NUM][BOARD_BLOCK_NUM], int b[BOARD_BLOCK_NUM][BOARD_BLOCK_NUM]);
long int fastExp(int);

class Board{
	
	public:
	
	Board();
	void init();
	bool move(int dir);
	int getBlock(int x, int y);
	bool isFull();
	bool isGameOver();
	long int getScore();
	
	private:
	
	int board[BOARD_BLOCK_NUM][BOARD_BLOCK_NUM];
	int EMPTY;
	int maxNewNum;
	long int score;
	
	bool moveLeft();
	bool moveUp();
	bool moveRight();
	bool moveDown();
	long int mergeScore(int n);
	int getRand(int low, int high);
	void putNewBlock();
	
};

#endif
