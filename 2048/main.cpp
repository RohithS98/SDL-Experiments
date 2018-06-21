#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include "LText.h"
#include "gameboard.h"

const int SCREEN_WIDTH = 500;
const int SCREEN_HEIGHT = 500;
const int BLOCK_SIDE = 70;
const int BLOCK_GAP = 10;
const int BOARD_X_OFFSET = (SCREEN_WIDTH - BLOCK_SIDE*BOARD_BLOCK_NUM - BLOCK_GAP*(BOARD_BLOCK_NUM-1))/2;
const int TOP_GAP = 50;
const int BOARD_Y_OFFSET = (SCREEN_HEIGHT - BLOCK_SIDE*BOARD_BLOCK_NUM - BLOCK_GAP*(BOARD_BLOCK_NUM-1))/2 + TOP_GAP;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
TTF_Font *gFont = NULL;
LText Title;
LText GameOver;
LText blockText;
LText scoreTitle;
LText score;
LText bestTitle;
LText bestScore;

int PrevBestScore = 0;

const int PALETTE_SIZE = 12;
const int palette[PALETTE_SIZE][3] = {
										{0xEE, 0xE4, 0xDA},
										{0xE3, 0xD0, 0xC0},
										{0xF5, 0xAF, 0x78},
										{0xFE, 0x94, 0x61},
										{0xFC, 0x77, 0x65},
										{0xFB, 0x5B, 0x37},
										{0xEC, 0xCE, 0x75},
										{0xEC, 0xCE, 0x5F},
										{0xEC, 0xC6, 0x4E},
										{0xF0, 0xC6, 0x32},
										{0xEF, 0xC1, 0x2E},
										{0xAC, 0xB7, 0x7B},};
const int emptyCol[3]	= 		{0x96, 0x8C, 0x82};
const int boardCol[3]	= 		{0x86, 0x7C, 0x72};
const int bgCol[3]		=		{0x2F, 0x25, 0x1C};
const int menuCol[3]	= 		{0xB6, 0xAC, 0xA2};
const int bannerCol[3] 	=	 	{0xEF,0xEF,0xAA};
const SDL_Color titleColor	= 	{0xFF,0xFF,0xFF,0xFF};
const SDL_Color overColor	= 	{0x8F,0x8f,0x10,0xFF};

using namespace std;

bool init(){
	bool success = true;
	if(SDL_Init(SDL_INIT_VIDEO) < 0){
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else if(!SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
	{
		printf( "Warning: Linear texture filtering not enabled!" );
	}
	else{
		window = SDL_CreateWindow("2048", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
									SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		
		if(window == NULL){
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else{
			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
			if( renderer == NULL ){
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else{
				SDL_SetRenderDrawColor( renderer, 0x0, 0x0, 0x0, 0xFF );
				//SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) )
				{
					printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
					success = false;
				}

				if( TTF_Init() == -1 )
				{
					printf( "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError() );
					success = false;
				}
			}
		}
	}
	return success;
}

bool updateLocalBest(){
	bool success = true;
	SDL_RWops* file = SDL_RWFromFile("bestscore.bin","r+b");
	if( file == NULL){
		file = SDL_RWFromFile("bestscore.bin","w+b");
		if(file == NULL){
			success = false;
			printf("Cannot Create File\n");
		}
		else{
			PrevBestScore = 0;
			SDL_RWwrite(file, &PrevBestScore, sizeof(int), 1);
			SDL_RWclose(file);
		}
	}
	else{
		SDL_RWread(file, &PrevBestScore, sizeof(int), 1);
		SDL_RWclose(file);
	}
	return success;
}

bool loadMedia(){
	bool success = true;
	
	gFont = TTF_OpenFont( "digital-7.ttf", 72 );
	if( gFont == NULL ){
		printf( "Failed to load font! SDL_ttf Error: %s\n", TTF_GetError() );
		success = false;
	}
	else if( !Title.loadFromRenderedText(renderer, gFont, "2048", titleColor) ||
		!GameOver.loadFromRenderedText(renderer, gFont, "GAME OVER", overColor) ||
		!scoreTitle.loadFromRenderedText(renderer, gFont, "SCORE", titleColor ) || 
		!bestTitle.loadFromRenderedText(renderer,gFont, "BEST", titleColor) ){
		printf("Could not render font\n");
		success = false;
	}
	
	success = success && updateLocalBest();
	return success;
}

void close(){
	Title.free();
	GameOver.free();
	blockText.free();
	scoreTitle.free();
	score.free();
	bestTitle.free();
	bestScore.free();
	TTF_CloseFont(gFont);
	SDL_DestroyRenderer( renderer );
	SDL_DestroyWindow( window );
	
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

void getBlockSize(int a, int textSize[2]){
	if(a < 17){
		textSize[0] = 12, textSize[1] = 5;
	}
	else if(a < 100){
		textSize[0] = 5, textSize[1] = 10;
	}
	else if(a < 1000){
		textSize[0] = 5, textSize[1] = 11;
	}
	else{
		textSize[0] = 5, textSize[1] = 17;
	}
}

void getScoreSize(int score, int textSize[2]){
	textSize[1] = 35;
	if(score < 10){
			textSize[0] = 25;
	}
	else if(score<100){
		textSize[0] = 40;
	}
	else if(score<1000){
		textSize[0] = 60;
	}
	else if(score<10000){
		textSize[0] = 75;
	}
	else if(score<100000){
		textSize[0] = 90;
	}
	else{
		textSize[0] = 105;
	}
}

void renderBoard(Board gameBoard){
	SDL_SetRenderDrawColor(renderer, bgCol[0], bgCol[1], bgCol[2], 0xFF);
	SDL_RenderClear(renderer);
	int blockfill = BLOCK_SIDE+BLOCK_GAP;
	stringstream numText;
	int textSize[2];
	
	SDL_Rect drawRect = {230,15,120,80};
	SDL_SetRenderDrawColor(renderer, menuCol[0], menuCol[1], menuCol[2], 0xFF);
	SDL_RenderFillRect(renderer, &drawRect);
	
	drawRect = {370,15,120,80};
	SDL_SetRenderDrawColor(renderer, menuCol[0], menuCol[1], menuCol[2], 0xFF);
	SDL_RenderFillRect(renderer, &drawRect);
	
	Title.render(renderer, 30,17,170,72);
	scoreTitle.render(renderer, 255,20,75,25);
	bestTitle.render(renderer, 395,20,75,25);
	
	drawRect = {BOARD_X_OFFSET - BLOCK_GAP, BOARD_Y_OFFSET - BLOCK_GAP, 4*blockfill+BLOCK_GAP, 4*blockfill+BLOCK_GAP};
	SDL_SetRenderDrawColor(renderer, boardCol[0], boardCol[1], boardCol[2], 0xFF);
	SDL_RenderFillRect(renderer, &drawRect);
	
	numText.str("");
	numText<<gameBoard.getScore();
	getScoreSize(gameBoard.getScore(),textSize);
	score.loadFromRenderedText(renderer,gFont,numText.str().c_str(), titleColor);
	score.render(renderer, 292.5-textSize[0]/2,67.5-textSize[1]/2,textSize[0],textSize[1]);
	
	numText.str("");
	int temp = PrevBestScore > gameBoard.getScore() ? PrevBestScore : gameBoard.getScore();
	numText<<temp;
	getScoreSize(temp,textSize);
	bestScore.loadFromRenderedText(renderer,gFont,numText.str().c_str(), titleColor);
	bestScore.render(renderer, 432.5-textSize[0]/2,67.5-textSize[1]/2,textSize[0],textSize[1]);
	
	for(int i = 0; i < BOARD_BLOCK_NUM ; i++){
		for(int j = 0; j < BOARD_BLOCK_NUM; j++){
			int k = log2(gameBoard.getBlock(i,j));
			int kcol = (k-1)%PALETTE_SIZE;
			drawRect = { BOARD_X_OFFSET + (j*blockfill), BOARD_Y_OFFSET + (i*blockfill), BLOCK_SIDE, BLOCK_SIDE };
			if(k != 0){
				SDL_SetRenderDrawColor(renderer, palette[kcol][0], palette[kcol][1], palette[kcol][2], 0xFF);
			}
			else{
				SDL_SetRenderDrawColor(renderer, emptyCol[0], emptyCol[1], emptyCol[2], 0xFF);
			}
			SDL_RenderFillRect(renderer, &drawRect);
			if(k != 0){
				numText.str("");
				numText<<gameBoard.getBlock(i,j);
				getBlockSize(gameBoard.getBlock(i,j),textSize);
				blockText.loadFromRenderedText(renderer,gFont,numText.str().c_str(),titleColor);
				blockText.render(renderer,textSize[0]+BOARD_X_OFFSET + (j*blockfill), textSize[1]+BOARD_Y_OFFSET + (i*blockfill), BLOCK_SIDE-textSize[0]*2, BLOCK_SIDE-textSize[1]*2);
			}
		}
	}
	
	SDL_RenderPresent(renderer);
}

void displayGameOver(){
	SDL_SetRenderDrawColor( renderer, bannerCol[0], bannerCol[1], bannerCol[2], 255);
	SDL_Rect drawRect = {0, SCREEN_HEIGHT/2 - 60, SCREEN_WIDTH, 120};
	SDL_RenderFillRect(renderer, &drawRect);
	GameOver.render(renderer, SCREEN_WIDTH/2 - 130, SCREEN_HEIGHT/2 - 30, 260, 60);
	SDL_RenderPresent(renderer);
}

void writeNewScore(int score){
	SDL_RWops* file = SDL_RWFromFile("bestscore.bin","w+b");
	SDL_RWwrite(file, &score, sizeof(int),1);
	SDL_RWclose(file);
}

int main( int argc, char* args[] ){
	if(!init()){
		printf("Failed to initialize\n");
		close();
		return 0;
	}
	else if(!loadMedia()){
		printf("Failed to load media\n");
		close();
		return 0;
	}
	
	Board gameBoard;
	gameBoard.init();
	
	bool change = true, quit = false, gameOver = false;
	SDL_Event e;
	int move = NO_MOVE;
	renderBoard(gameBoard);
	
	while(!quit){
		while(SDL_PollEvent(&e) != 0){
			if( e.type == SDL_QUIT ){
				quit = true;
			}
			else if( e.type == SDL_KEYDOWN && e.key.repeat == 0){
				
				switch(e.key.keysym.sym){
					case SDLK_w:
					case SDLK_UP:
						move = MOVE_UP;
						break;
					case SDLK_a:
					case SDLK_LEFT:
						move = MOVE_LEFT;
						break;
					case SDLK_s:
					case SDLK_DOWN:
						move = MOVE_DOWN;
						break;
					case SDLK_d:
					case SDLK_RIGHT:
						move = MOVE_RIGHT;
						break;
					case SDLK_r:
						change = true, gameOver = false;
						writeNewScore(PrevBestScore > gameBoard.getScore() ? PrevBestScore : gameBoard.getScore());
						updateLocalBest();
						move = NO_MOVE;
						gameBoard.init();
				}
			}
		}
		
		if( move != NO_MOVE && !gameOver ){
			change = gameBoard.move(move);
			gameOver = gameBoard.isGameOver();
			move = NO_MOVE;
		}
		
		if(change){
			renderBoard(gameBoard);
			if(gameOver){
				displayGameOver();
				long int temp = PrevBestScore > gameBoard.getScore() ? PrevBestScore : gameBoard.getScore();
				writeNewScore(temp);
			}
			change = false;
		}

	}
	writeNewScore(PrevBestScore > gameBoard.getScore() ? PrevBestScore : gameBoard.getScore());
	close();
	return 0;
}
