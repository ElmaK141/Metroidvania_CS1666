#include "sprite.h"
#include <iostream>

Sprite::Sprite() {

}

Sprite::Sprite(int x, int y, int w, int h, std::string file,
				SDL_Renderer* renderer) {
	
	this->xLoc = x;
	this->yLoc = y;
	this->spriteWidth = w;
	this->spriteHeight = h;

	SDL_Surface* load = IMG_Load(file.c_str());
	
	SDL_SetColorKey(load, SDL_TRUE, SDL_MapRGB(load->format, 255, 255, 255));
	this->texture = SDL_CreateTextureFromSurface(renderer, load);
	SDL_FreeSurface(load);
}

Sprite::~Sprite() {
	SDL_DestroyTexture(this->texture);
}

void Sprite::draw(SDL_Renderer * render, int x, int y){
	SDL_Rect loc = { x,y,this->spriteWidth*4,this->spriteHeight*4 }; //stretch sprite to 4x size
	SDL_Rect crop = { this->xLoc,this->yLoc,this->spriteWidth,this->spriteHeight };
	SDL_RenderCopy(render, this->texture, &crop, &loc);
}

void Sprite::draw(SDL_Renderer * render, int x, int y, int len){
	SDL_Rect loc = { x,y,len,this->spriteWidth * 4 }; //stretch sprite to 4x size
	SDL_Rect crop = { this->xLoc, this->yLoc, this->spriteWidth, this->spriteHeight };
	SDL_RenderCopy(render, this->texture, &crop, &loc);
}

int Sprite::getWidth() {
	return this->spriteWidth * 4;	//compensate for sprite scaling
}

int Sprite::getHeight() {
	return this->spriteHeight * 4;	//compensate for sprite scaling
}

