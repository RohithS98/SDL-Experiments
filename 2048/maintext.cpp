#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>

#include "gameboard.h"

using namespace std;

void showBoard(Board gameBoard){
	for(int i = 0; i < BOARD_BLOCK_NUM ; i++){
		for(int j = 0; j < BOARD_BLOCK_NUM ; j++){
			int k = gameBoard.getBlock(i,j);
			if(k == 1){
				printf("-\t");
			}
			else{
				printf("%d\t",k);
			}
		}
		printf("\n");
	}
}

int main(){
	Board gameBoard;
	gameBoard.init();
	char opt;
	
	while(!gameBoard.isGameOver()){
		printf("Score : %d\n",gameBoard.getScore());
		showBoard(gameBoard);
		printf("Enter move (W,A,S,D) : ");
		cin>>opt;
		switch(opt){
			case 'w':
				gameBoard.move(MOVE_UP);
				break;
			case 'a':
				gameBoard.move(MOVE_LEFT);
				break;
			case 's':
				gameBoard.move(MOVE_DOWN);
				break;
			case 'd':
				gameBoard.move(MOVE_RIGHT);
				break;
			default:
				printf("Invalid Move\n");
		}
		
		printf("\n\n-------------------------------------------\n\n");
	}
	
	printf("GAME OVER!\n");
	showBoard(gameBoard);
	printf("Final Score: %d",gameBoard.getScore());
	return 0;
}
