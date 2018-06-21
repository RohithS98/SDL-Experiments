#include "gameboard.h"

void copyBoard(int a[BOARD_BLOCK_NUM][BOARD_BLOCK_NUM], int b[BOARD_BLOCK_NUM][BOARD_BLOCK_NUM]){
	for(int i = 0; i < BOARD_BLOCK_NUM ; i++){
		for(int j = 0; j < BOARD_BLOCK_NUM ; j++){
			b[i][j] = a[i][j];
		}
	}
}

long int fastExp(int a){
	if(a == 0){
		return 1;
	}
	else if(a == 1){
		return 2;
	}
	else if(a%2){
		long int k = fastExp(a/2);
		return k*k*2;
	}
	else{
		long int k = fastExp(a/2);
		return k*k;
	}
}

Board::Board(){
	EMPTY = 0;
}

void Board::init(){
	srand(time(NULL));
	for(int i = 0; i < BOARD_BLOCK_NUM ; i++){
		for(int j = 0; j < BOARD_BLOCK_NUM ; j++){
			board[i][j] = EMPTY;
		}
	}
	
	score = 0;
	board[getRand(0,1)][getRand(0,3)] = 1;
	board[getRand(2,3)][getRand(0,3)] = 1;
	/*
	board[0][0] = 1, board[0][1] = 2, board[0][2] = 3, board[0][3] = 4, board[1][0] = 5; 
	board[1][1] = 6, board[1][2] = 7, board[1][3] = 8, board[2][0] = 9, board[2][1] = 10;
	board[2][2] = 11, board[2][3] = 12;
	*/
	maxNewNum = 2;
}

long int Board::getScore(){
	return score;
}

int Board::getBlock(int x, int y){
	return fastExp(board[x][y]);
}

bool Board::isFull(){
	for(int i = 0; i < BOARD_BLOCK_NUM ; i++){
		for(int j = 0; j < BOARD_BLOCK_NUM ; j++){
			if(board[i][j] == EMPTY){
				return false;
			}
		}
	}
	return true;
}

int Board::getRand(int low, int high){
	return (rand()%(high-low+1)) + low;
}

bool Board::move(int dir){
	bool anyChange = false;
	switch(dir){
		case MOVE_UP:
			anyChange = moveUp();
			break;
		case MOVE_DOWN:
			anyChange = moveDown();
			break;
		case MOVE_LEFT:
			anyChange = moveLeft();
			break;
		case MOVE_RIGHT:
			anyChange = moveRight();
			break;
	}
	if(anyChange){
		putNewBlock();
	}
	return anyChange;
}

void Board::putNewBlock(){
	int freeSlots[BOARD_BLOCK_NUM*BOARD_BLOCK_NUM];
	int freeNum = 0;
	for(int i = 0; i < BOARD_BLOCK_NUM ; i++){
		for(int j = 0; j < BOARD_BLOCK_NUM ; j++){
			if(board[i][j] == EMPTY){
				freeSlots[freeNum++] = i*BOARD_BLOCK_NUM+j;
			}
		}
	}
	int sel = getRand(0,freeNum-1);
	board[freeSlots[sel]/BOARD_BLOCK_NUM][freeSlots[sel]%BOARD_BLOCK_NUM] = getRand(1,maxNewNum);
}

bool Board::isGameOver(){
	if(!isFull()){
		return false;
	}
	int temp[BOARD_BLOCK_NUM][BOARD_BLOCK_NUM];
	copyBoard(board,temp);
	if(moveLeft()){
		copyBoard(temp,board);
		return false;
	}
	if(moveRight()){
		copyBoard(temp,board);
		return false;
	}
	if(moveUp()){
		copyBoard(temp,board);
		return false;
	}
	if(moveDown()){
		copyBoard(temp,board);
		return false;
	}
	return true;	
}

long int Board::mergeScore(int n){
	return (n-1)*fastExp(n);
}

bool Board::moveLeft(){
	bool moved = false;
	for(int i = 0; i < BOARD_BLOCK_NUM ; i++){
		int k = 0, j = 1;
		while(j < BOARD_BLOCK_NUM){
			if(board[i][j]==EMPTY){
				j++;
			}
			else if(board[i][k] == EMPTY){
				board[i][k] = board[i][j];
				board[i][j++] = EMPTY;
				moved = true;
			}
			else if(board[i][j] == board[i][k]){
				board[i][k++]++;
				score += mergeScore(board[i][k-1]);
				board[i][j++] = EMPTY;
				moved = true;
			}
			else if(j!=k+1){
				board[i][++k] = board[i][j];
				board[i][j++] = EMPTY;
				moved = true;
			}
			else{
				j++;
				k++;
			}
		}
	}
	return moved;
}

bool Board::moveRight(){
	bool moved = false;
	for(int i = 0; i < BOARD_BLOCK_NUM ; i++){
		int k = BOARD_BLOCK_NUM-1;
		int j = k-1;
		while(j > -1){
			if(board[i][j]==EMPTY){
				j--;
			}
			else if(board[i][k] == EMPTY){
				board[i][k] = board[i][j];
				board[i][j--] = EMPTY;
				moved = true;
			}
			else if(board[i][j] == board[i][k]){
				board[i][k--]++;
				score += mergeScore(board[i][k+1]);
				board[i][j--] = EMPTY;
				moved = true;
			}
			else if(j!=k-1){
				board[i][--k] = board[i][j];
				board[i][j--] = EMPTY;
				moved = true;
			}
			else{
				j--;
				k--;
			}
		}
	}
	return moved;
}

bool Board::moveDown(){
	bool moved = false;
	for(int i = 0; i < BOARD_BLOCK_NUM ; i++){
		int k = BOARD_BLOCK_NUM-1;
		int j = k-1;
		while(j > -1){
			if(board[j][i]==EMPTY){
				j--;
			}
			else if(board[k][i] == EMPTY){
				board[k][i] = board[j][i];
				board[j--][i] = EMPTY;
				moved = true;
			}
			else if(board[j][i] == board[k][i]){
				board[k--][i]++;
				score += mergeScore(board[k+1][i]);
				board[j--][i] = EMPTY;
				moved = true;
			}
			else if(j!=k-1){
				board[--k][i] = board[j][i];
				board[j--][i] = EMPTY;
				moved = true;
			}
			else{
				j--;
				k--;
			}
		}
	}
	return moved;
}

bool Board::moveUp(){
	bool moved = false;
	for(int i = 0; i < BOARD_BLOCK_NUM ; i++){
		int k = 0, j = 1;
		while(j < BOARD_BLOCK_NUM){
			if(board[j][i]==EMPTY){
				j++;
			}
			else if(board[k][i] == EMPTY){
				board[k][i] = board[j][i];
				board[j++][i] = EMPTY;
				moved = true;
			}
			else if(board[j][i] == board[k][i]){
				board[k++][i]++;
				score += mergeScore(board[k-1][i]);
				board[j++][i] = EMPTY;
				moved = true;
			}
			else if(j!=k+1){
				board[++k][i] = board[j][i];
				board[j++][i] = EMPTY;
				moved = true;
			}
			else{
				j++;
				k++;
			}
		}
	}
	return moved;
}
