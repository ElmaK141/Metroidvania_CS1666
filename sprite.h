#ifndef _SPRITE_H_
#define _SPRITE_H_

#ifdef __linux__
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#elif _WIN32
#include <SDL.h>
#include <SDL_image.h>
#endif

#include <string>
#include <vector>

class Sprite {
	public:
		Sprite(int x, int y, int w, int h, std::string file, SDL_Renderer* renderer);
		~Sprite();

		void draw(SDL_Renderer* render, int x, int y);
	private:
		SDL_Texture* texture;
		int xLoc;
		int yLoc;
		int spriteWidth;
		int spriteHeight;
};




#endif