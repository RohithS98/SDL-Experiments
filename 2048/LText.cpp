#include "LText.h"

LText::LText()
{
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LText::~LText()
{
	//Deallocate
	free();
}

bool LText::loadFromRenderedText( SDL_Renderer* gRenderer, TTF_Font* gFont, std::string textureText, SDL_Color textColor )
{
	//Get rid of preexisting texture
	free();

	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Solid( gFont, textureText.c_str(), textColor );
	if( textSurface != NULL )
	{
		//Create texture from surface pixels
        mTexture = SDL_CreateTextureFromSurface( gRenderer, textSurface );
		if( mTexture == NULL )
		{
			printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
		}
		else
		{
			//Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		//Get rid of old surface
		SDL_FreeSurface( textSurface );
	}
	else
	{
		printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
	}

	
	//Return success
	return mTexture != NULL;
}

void LText::free()
{
	//Free texture if it exists
	if( mTexture != NULL )
	{
		SDL_DestroyTexture( mTexture );
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void LText::render( SDL_Renderer* gRenderer, int x, int y, int mW, int mH )
{
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, mW, mH };

	//Render to screen
	SDL_RenderCopy( gRenderer, mTexture, NULL, &renderQuad);
}

int LText::getWidth()
{
	return mWidth;
}

int LText::getHeight()
{
	return mHeight;
}
