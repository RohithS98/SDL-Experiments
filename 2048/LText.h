#ifndef __LTEXTURE_H__
#define __LTEXTURE_H__

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

//Text wrapper class
class LText
{
	public:
		//Initializes variables
		LText();

		//Deallocates memory
		~LText();
		
		//Creates image from font string
		bool loadFromRenderedText( SDL_Renderer* gRenderer, TTF_Font* gFont, std::string textureText, SDL_Color textColor );

		//Deallocates texture
		void free();

		//Set color modulation
		void setColor( Uint8 red, Uint8 green, Uint8 blue );
		
		void render( SDL_Renderer* gRenderer, int x, int y, int mW, int mH );

		//Gets image dimensions
		int getWidth();
		int getHeight();

	private:
		//The actual hardware texture
		SDL_Texture* mTexture;

		//Image dimensions
		int mWidth;
		int mHeight;
};

#endif
