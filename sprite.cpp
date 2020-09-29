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
	this->scale = scalar;
	this->screenX = x;
	this->screenY = y;

	SDL_Surface* load = IMG_Load(file.c_str());
	SDL_GetError();

	SDL_SetColorKey(load, SDL_TRUE, SDL_MapRGB(load->format, 255, 255, 255));
	this->texture = SDL_CreateTextureFromSurface(renderer, load);
	SDL_FreeSurface(load);
}

Sprite::~Sprite() {
}


/**
 *  \brief Draw this sprite to the screen at given location.
 *
 *  \param render:	The renderer which should copy parts of a texture.
 *  \param x:	X location to draw to.
 *  \param y:	Y location to draw to.
 */
void Sprite::draw(SDL_Renderer * render, int x, int y){
	SDL_Rect loc = { x,y,this->entWidth,this->entHeight};
	SDL_Rect crop = { this->xLoc,this->yLoc,this->spriteWidth,this->spriteHeight };
	SDL_RenderCopy(render, this->texture, &crop, &loc);
	this->screenX = x;
	this->screenY = y;
}

//draw with flip
void Sprite::draw(SDL_Renderer* render, int x, int y, SDL_RendererFlip flip) {
	SDL_Rect loc = { x,y,this->entWidth,this->entHeight };
	SDL_Rect crop = { this->xLoc,this->yLoc,this->spriteWidth,this->spriteHeight };
	SDL_RenderCopyEx(render, this->texture, &crop, &loc, 0.0, nullptr, flip);
	this->screenX = x;
	this->screenY = y;
}

int Sprite::getWidth() {
	return this->entWidth;
}

int Sprite::getHeight() {
	return this->entHeight;	
}

int Sprite::getScale() {
	return this->scale;
}

int Sprite::getX() {
	return this->screenX;
}

int Sprite::getY() {
	return this->screenY;
}