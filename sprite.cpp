#include "sprite.h"
#include <iostream>

Sprite::Sprite() {

}

Sprite::Sprite(int x, int y, int w, int h, int scalar, std::string file,
				SDL_Renderer* renderer) {
	
	this->xLoc = x;
	this->yLoc = y;
	this->spriteWidth = w;
	this->spriteHeight = h;
	this->entWidth = w*scalar;
	this->entHeight = h*scalar;

	SDL_Surface* load = IMG_Load(file.c_str());
	
	SDL_SetColorKey(load, SDL_TRUE, SDL_MapRGB(load->format, 255, 255, 255));
	this->texture = SDL_CreateTextureFromSurface(renderer, load);
	SDL_FreeSurface(load);
}

Sprite::~Sprite() {
	SDL_DestroyTexture(this->texture);
}

void Sprite::draw(SDL_Renderer * render, int x, int y){
	SDL_Rect loc = { x,y,this->entWidth,this->entHeight};
	SDL_Rect crop = { this->xLoc,this->yLoc,this->spriteWidth,this->spriteHeight };
	SDL_RenderCopy(render, this->texture, &crop, &loc);
}

int Sprite::getWidth() {
	return this->entWidth;
}

int Sprite::getHeight() {
	return this->entHeight;	
}

