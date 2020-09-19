#include "sprite.h"

Sprite::Sprite(int x, int y, int w, int h, std::string file,
				SDL_Renderer* renderer) {
	
	this->xLoc = x;
	this->yLoc = y;
	this->spriteWidth = w;
	this->spriteHeight = h;

	SDL_Texture* sp = IMG_LoadTexture(renderer, file.c_str());
	this->texture = sp;
}

Sprite::~Sprite() {
	SDL_DestroyTexture(this->texture);
}

