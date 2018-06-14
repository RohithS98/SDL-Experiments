#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <cmath>
#include <string>
#include <sstream>
#include "LTexture.h"
#include "LTimer.h"

using namespace std;

const int SCREEN_WIDTH = 600;
const int SCREEN_GAME_HEIGHT = 420;
const int SCREEN_HEIGHT = 520;
const float PADDLE_YMOVE = SCREEN_GAME_HEIGHT / 190.0;
const int PADDLE_START_POS = 20;
const int PADDLE_HEIGHT = 60;
const int PADDLE_THICKNESS = 10;

const int SCREEN_FPS = 60;
const int SCREEN_TICK_PER_FRAME = 1000 / SCREEN_FPS;

const int OPP_PADDLE_START_POS = SCREEN_WIDTH - 30;
const int ACCURACY_LIMIT = 2;
const int MY_SCORE_X = 20;
const int MY_SCORE_Y = SCREEN_GAME_HEIGHT + 30;
const int OPP_SCORE_X = SCREEN_WIDTH - 40;
const int OPP_SCORE_Y = MY_SCORE_Y;

const int XCHECKPOS = 400;

struct Circle{
	int x,y;
	int r;
};

bool init();
void close();

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
//Globally used font
TTF_Font *gFont = NULL;
Mix_Chunk *gBeep = NULL;
Mix_Chunk *gErr = NULL;
Mix_Chunk *gPo = NULL;

//Rendered texture
LTexture myScore;
LTexture oppScore;
LTexture ball;
stringstream streamText;

class MenuSq{
	public:
		
		MenuSq( SDL_Rect, SDL_Color, SDL_Color, bool);
		
		bool isOn();
		
		void setActive();
		
		void setPassive();
		
		void render();
		
		bool checkcol(int,int);
	
	private:
	
		SDL_Rect position;
		
		SDL_Color passiveCol;
		
		SDL_Color activeCol;
	
		bool On;	
};

MenuSq::MenuSq(SDL_Rect pos, SDL_Color pas, SDL_Color act, bool on){
	position = pos;
	passiveCol = pas;
	activeCol = act;
	On = on;
}

bool MenuSq::isOn(){
	return On;
}

void MenuSq::setActive(){
	On = true;
}

void MenuSq::setPassive(){
	On = false;
}

bool MenuSq::checkcol(int x, int y){
	if(((x >= position.x) && (x <= position.x + position.w)) && ((y >= position.y) && (y <= position.y + position.h))) {
		return true;
	}
	return false;
}

void MenuSq::render(){
	if(On){
		SDL_SetRenderDrawColor( renderer, activeCol.r, activeCol.g, activeCol.b, activeCol.a );
	}
	else{
		SDL_SetRenderDrawColor(renderer, passiveCol.r, passiveCol.g, passiveCol.b, passiveCol.a );
	}
	SDL_RenderFillRect( renderer, &position );
}

const int MENU_ICON_NUM = 2;
MenuSq** menuList;

double distanceSquared( int x1, int y1, int x2, int y2 )
{
	int deltaX = x2 - x1;
	int deltaY = y2 - y1;
	return deltaX*deltaX + deltaY*deltaY;
}

int checkCollision( Circle& a, float v , int myY, int opY)
{
    //Closest point on collision box
    int cX, cY;
    SDL_Rect b;
    
    if( v > 0 ){
    	b = { OPP_PADDLE_START_POS, opY, PADDLE_THICKNESS, PADDLE_HEIGHT };
    }
    else{
    	b = { PADDLE_START_POS, myY, PADDLE_THICKNESS, PADDLE_HEIGHT };
    }
    

    //Find closest x offset
    if( a.x < b.x )
    {
        cX = b.x;
    }
    else if( a.x > b.x + b.w )
    {
        cX = b.x + b.w;
    }
    else
    {
        cX = a.x;
    }

    //Find closest y offset
    if( a.y < b.y )
    {
        cY = b.y;
    }
    else if( a.y > b.y + b.h )
    {
        cY = b.y + b.h;
    }
    else
    {
        cY = a.y;
    }
    
    //printf("%d %d %d %d %d %d\n",a.x,a.y,b.x,b.y,cX,cY);

    //If the closest point is inside the circle
    if( distanceSquared( a.x, a.y, cX, cY ) < a.r * a.r )
    {
        //This box and the circle have collided
        return cY-b.y-(b.h/2);
    }

    //If the shapes have not collided
    return -1000;
}

/////////////////////////////////////////////////////////

class Ball{
    public:
		//The dimensions of the dot
		static const int DOT_WIDTH = 20;
		static const int DOT_HEIGHT = 20;

		//Maximum axis velocity of the dot
		float DOT_VEL = 2.8;

		//Initializes the variables
		Ball();
		
		void start( float , float );

		//Moves the dot and checks collision
		int move( float, float );

		//Shows the dot on the screen
		void render(SDL_Renderer*);
		
		void reset( float, float);
		
		void setLimits(int,int);
		
		float getXPos();
		
		float getYPos();
		
		//LTexture ball;

		//Gets collision circle
		Circle& getCollider();

    private:
		//The X and Y offsets of the dot
		float mPosX, mPosY;
		
		int width_limit, height_limit;

		//The velocity of the dot
		float mVelX, mVelY;
		
		//Dot's collision circle
		Circle mCollider;

		//Moves the collision circle relative to the dot's offset
		void shiftColliders();
};

Ball::Ball()
{
    //Initialize the offsets
    mPosX = 0;
    mPosY = 0;

	//Set collision circle size
	mCollider.r = DOT_WIDTH / 2;

    //Initialize the velocity
    mVelX = 0;
    mVelY = 0;

	//Move collider relative to the circle
	shiftColliders();
}

void Ball::reset( float x, float y){
	//Initialize the offsets
    mPosX = x;
    mPosY = y;

	//Set collision circle size
	mCollider.r = DOT_WIDTH / 2;

    //Initialize the velocity
    mVelX = 0;
    mVelY = 0;

	//Move collider relative to the circle
	shiftColliders();
}

void Ball::start( float x, float y ){
	mVelX = DOT_VEL*x;
	mVelY = DOT_VEL*y;
}

float Ball::getXPos(){
	return mPosX;
}
		
float Ball::getYPos(){
	return mPosY;
}

void Ball::setLimits(int w, int h){
	width_limit = w;
	height_limit = h;
}

int Ball::move( float myY, float opY )
{
    //Move the dot left or right
    mPosX += mVelX;
	shiftColliders();
	float def;

    //If the dot collided or went too far to the left or right
	if(  mPosX - mCollider.r < 0 ){
		Mix_PlayChannel( -1, gErr, 0 );
		return -1;
	}
	else if( mPosX + mCollider.r > width_limit ) 
    {
    	Mix_PlayChannel( -1, gPo, 0 );
        return 1;
    }
    else{
    	if((def = checkCollision( mCollider , mVelX , myY, opY)) != -1000){
    		mVelX = (-1)*mVelX;
    		mVelY = DOT_VEL*(def/(PADDLE_HEIGHT/2));
    		Mix_PlayChannel( -1, gBeep, 0 );
    	}
    }

    //Move the dot up or down
    mPosY += mVelY;
	shiftColliders();

    //If the dot collided or went too far up or down
    if( ( mPosY - mCollider.r < 0 ) || ( mPosY + mCollider.r > height_limit ) )
    {
        mVelY = -1*mVelY;
        mPosY += mVelY;
    }
    else{
    	if(checkCollision( mCollider , mVelX , myY, opY) != -1000){
    		mVelY = (-1)*mVelY;
    	}
    }
    
    return 0;
}

void Ball::render(SDL_Renderer* renderer)
{
    //Show the dot
	ball.renderScaled( renderer, mPosX - mCollider.r, mPosY - mCollider.r, DOT_WIDTH, DOT_HEIGHT);
}

Circle& Ball::getCollider()
{
	return mCollider;
}

void Ball::shiftColliders()
{
	//Align collider to center of dot
	mCollider.x = mPosX;
	mCollider.y = mPosY;
}
//////////////////////////////////////////////////////////////////

bool init(){
	bool success = true;
	
	if(SDL_Init(SDL_INIT_VIDEO) < 0){
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else{
		//Set texture filtering to linear
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
		{
			printf( "Warning: Linear texture filtering not enabled!" );
		}

		//Create window
		window = SDL_CreateWindow( "PONG", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		
		if(window == NULL){
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else{
			renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED);
			if( renderer == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor( renderer, 0x0, 0x0, 0x0, 0xFF );
				
				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) )
				{
					printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
					success = false;
				}

				 //Initialize SDL_ttf
				if( TTF_Init() == -1 )
				{
					printf( "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError() );
					success = false;
				}
								 //Initialize SDL_mixer
				if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
				{
					printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
					success = false;
				}
			}
		}
	}
	
	SDL_Rect rect = {SCREEN_WIDTH/2 - 40,SCREEN_GAME_HEIGHT+40,30,30};
	SDL_Color c1 = {0xAA,0xFF,0xAA,0xFF};
	SDL_Color c2 = {0x0,0xFF,0x0,0xFF};
	
	menuList = new MenuSq*[2];
	menuList[0] = new MenuSq(rect,c1,c2,true);
	
	rect = {SCREEN_WIDTH/2 + 10, SCREEN_GAME_HEIGHT+40, 30, 30};
	c1 = {0xFF,0xAA,0xAA,0xFF};
	c2 = {0xFF,0x0,0x0,0xFF};
		
	menuList[1] = new MenuSq(rect,c1,c2,false);

	return success;
}

int handleMenus(){
	int x,y;
	SDL_GetMouseState(&x,&y);
	int k = 0;
	for(int i = 0; i<MENU_ICON_NUM ; i++){
		if(menuList[i]->checkcol(x,y)){
			k = i+1;
		}
	}
	if(k){
		for(int i = 0; i<MENU_ICON_NUM ; i++){
			menuList[i]->setPassive();
		}
		menuList[k-1]->setActive();
	}
	return k;
}

bool loadMedia()
{
	//Loading success flag
	bool success = true;

	//Open the font
	gFont = TTF_OpenFont( "digital-7.ttf", 28 );
	if( gFont == NULL )
	{
		printf( "Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError() );
		success = false;
	}
	if( !ball.loadFromFile(renderer, "ball.png") ){
		printf( "Failed to load ball texture!\n" );
		success = false;
	}
	
	//Load music
	gBeep = Mix_LoadWAV( "Beep2.wav" );
	if( gBeep == NULL )
	{
		printf( "Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError() );
		success = false;
	}
	
	gErr = Mix_LoadWAV( "Bad.wav" );
	if( gErr == NULL )
	{
		printf( "Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError() );
		success = false;
	}
	
	gPo = Mix_LoadWAV( "Good.wav" );
	if( gErr == NULL )
	{
		printf( "Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError() );
		success = false;
	}
	

	return success;
}


void close()
{
	//Free loaded images
	myScore.free();
	oppScore.free();
	ball.free();
	for(int i = 0; i<MENU_ICON_NUM ; i++){
		free(menuList[i]);
	}
	
	free(menuList);
	Mix_FreeChunk( gBeep );
	Mix_FreeChunk( gErr );
	Mix_FreeChunk( gPo );

	//Free global font
	TTF_CloseFont( gFont );
	gFont = NULL;

	//Destroy window	
	SDL_DestroyRenderer( renderer );
	SDL_DestroyWindow( window );
	window = NULL;
	renderer = NULL;

	//Quit SDL subsystems
	Mix_Quit();
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

float movePaddle( float paddleYPos, bool moveUp, bool moveDown){
	if( (moveUp && moveDown) || (!moveUp && !moveDown) ){
		;
	}
	else if(moveDown){
		paddleYPos += PADDLE_YMOVE;
		if(paddleYPos + PADDLE_HEIGHT > SCREEN_GAME_HEIGHT ){
			paddleYPos = SCREEN_GAME_HEIGHT - PADDLE_HEIGHT;
		}
	}
	else{
		paddleYPos -= PADDLE_YMOVE;
		if(paddleYPos < 0 ){
			paddleYPos = 0;
		}
	}
	
	return paddleYPos;
}

float moveOpp( float oppPos, Ball myBall, bool dotDead , int difficulty = 1){

	if(difficulty >= 1){
	
		if(dotDead) {
			return oppPos;
		}
		
		if (myBall.getXPos() < (SCREEN_WIDTH - XCHECKPOS )){
			if( oppPos + PADDLE_HEIGHT/2 - SCREEN_GAME_HEIGHT/2 > ACCURACY_LIMIT ){
				oppPos -= PADDLE_YMOVE;
			}
			else if( oppPos + PADDLE_HEIGHT/2 - SCREEN_GAME_HEIGHT/2 < (-1)*ACCURACY_LIMIT ){
				oppPos += PADDLE_YMOVE;
			}
		}
		else if( oppPos + PADDLE_HEIGHT/2 - myBall.getYPos() > ACCURACY_LIMIT ){
			oppPos -= PADDLE_YMOVE;
		}
		else if( oppPos + PADDLE_HEIGHT/2 - myBall.getYPos() < (-1)*ACCURACY_LIMIT ){
			oppPos += PADDLE_YMOVE;
		}
	}
	
	if(oppPos + PADDLE_HEIGHT > SCREEN_GAME_HEIGHT ){
		oppPos = SCREEN_GAME_HEIGHT - PADDLE_HEIGHT;
	}
	
	if(oppPos < 0 ){
		oppPos = 0;
	}
	
	return oppPos;
}

bool renderScores( int s1, int s2 ){
	SDL_Color textColor = { 0xFF, 0xFF, 0xFF };
	streamText.str("");
	streamText<<s1;
	if( !myScore.loadFromRenderedText( renderer, gFont, streamText.str().c_str(), textColor ) ){
		printf( "Failed to render text texture!\n" );
		return true;
	}
	streamText.str("");
	streamText<<s2;
	if( !oppScore.loadFromRenderedText( renderer, gFont, streamText.str().c_str(), textColor ) ){
		printf( "Failed to render text texture!\n" );
		return true;
	}
	
	return false;
}

float speed(int la){
	if(la == 1){
		return 1;
	}
	else{
		return 1.5;
	}
}

int main( int argc, char* args[] )
{
	//Start up SDL and create window
	if( !init() ){
		printf( "Failed to initialize!\n" );
	}
	else if(!loadMedia()){
		printf( "Failed to load media!\n" );
	}
	else{
		bool quit = false;
		SDL_Event e;
		bool upPressed = false, downPressed = false, dotDead = true;
		float paddleY = (SCREEN_GAME_HEIGHT - PADDLE_HEIGHT) / 2;
		float opponentY = paddleY;
		int score1 = 0, score2 = 0, toggle = 0;
		Ball myBall;
		myBall.setLimits(SCREEN_WIDTH, SCREEN_GAME_HEIGHT);
		//The frames per second cap timer
		LTimer capTimer, spawnTimer;
		
		int lastActive = 1, newActive = 1;

		while( !quit ){
		
			capTimer.start();
			
			while( SDL_PollEvent( &e ) != 0 ){
				if( e.type == SDL_QUIT ){
					quit = true;
				}
				else if( e.type == SDL_KEYDOWN  && e.key.repeat == 0 ){
					
					switch(e.key.keysym.sym){
						case SDLK_w:
							upPressed = true;
							break;
						case SDLK_s:
							downPressed = true;
							break;
					}
				}
				else if( e.type == SDL_KEYUP  && e.key.repeat == 0 ){
				
					switch(e.key.keysym.sym){
						case SDLK_w:
							upPressed = false;
							break;
						case SDLK_s:
							downPressed = false;
							break;
					}
				}
				else if( e.type == SDL_MOUSEBUTTONUP ){
					int t = handleMenus();
					if(t){
						newActive = t;
					}
				}
			}
			
			if(lastActive != newActive){
				lastActive = newActive;
				upPressed = false, downPressed = false, dotDead = true;
				paddleY = (SCREEN_GAME_HEIGHT - PADDLE_HEIGHT) / 2;
				opponentY = paddleY;
				dotDead = true;
				score1 = 0, score2 = 0;
				
			}
			
			if(dotDead){
				myBall.reset(SCREEN_WIDTH/2, SCREEN_GAME_HEIGHT/2);
				if(spawnTimer.isStarted() && spawnTimer.getTicks() > 1000){
					dotDead = false;
					if(toggle)
						myBall.start(speed(lastActive),1);
					else
						myBall.start(-speed(lastActive),-1);
					toggle = 1 - toggle;
					spawnTimer.stop();
				}	
				else if(!spawnTimer.isStarted()){
					spawnTimer.start();
				}
			}
			
			paddleY = movePaddle( paddleY, upPressed, downPressed);
			opponentY = moveOpp( opponentY , myBall, dotDead, lastActive);
			int rc = myBall.move( paddleY, opponentY );
			if(!dotDead && rc > 0){
				dotDead = true;
				score1 += 1;
			}
			else if(!dotDead && rc < 0){
				dotDead = true;
				score2 += 1;
			}
			
			quit = quit || renderScores(score1, score2);
			
			SDL_SetRenderDrawColor( renderer, 0x0, 0x0, 0x0, 0xFF );
			SDL_RenderClear( renderer );
			
			SDL_Rect fillRect = { PADDLE_START_POS, paddleY, PADDLE_THICKNESS, PADDLE_HEIGHT };
			SDL_SetRenderDrawColor( renderer, 0xFF, 0xFF, 0xFF, 0xFF );		
			SDL_RenderFillRect( renderer, &fillRect );
			
			if(!dotDead){
				myBall.render(renderer);
			}
			
			for(int i = 0; i<MENU_ICON_NUM ; i++){
				menuList[i]->render();
			}
			
			
			fillRect = { OPP_PADDLE_START_POS, opponentY, PADDLE_THICKNESS, PADDLE_HEIGHT };
			SDL_SetRenderDrawColor( renderer, 0xFF, 0xFF, 0xFF, 0xFF );		
			SDL_RenderFillRect( renderer, &fillRect );
			
			fillRect = { 0, SCREEN_GAME_HEIGHT, SCREEN_WIDTH, 5 };
			SDL_SetRenderDrawColor( renderer, 0xFF, 0xFF, 0xFF, 0xFF );		
			SDL_RenderFillRect( renderer, &fillRect );
			
			if(dotDead){
				SDL_SetRenderDrawColor( renderer, 0xFF, 0x0, 0x0, 0xFF);
			}
			else{
				SDL_SetRenderDrawColor( renderer, 0x0, 0x0, 0xFF, 0xFF);
			}
			SDL_RenderDrawLine( renderer, 0, SCREEN_GAME_HEIGHT/2, SCREEN_WIDTH, SCREEN_GAME_HEIGHT/2);
			
			//SDL_SetRenderDrawColor( renderer, 0xFF, 0xFF, 0xFF, 0xFF );
			
			myScore.render( renderer, MY_SCORE_X, MY_SCORE_Y );
			oppScore.render( renderer, OPP_SCORE_X, OPP_SCORE_Y );
			
			SDL_RenderPresent( renderer );
			
			int frameTicks = capTimer.getTicks();
			if( frameTicks < SCREEN_TICK_PER_FRAME )
			{
				//Wait remaining time
				SDL_Delay( SCREEN_TICK_PER_FRAME - frameTicks );
			}
		}
	}
	
	close();
	return 0;
}

