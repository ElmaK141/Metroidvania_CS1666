#include "background.h"


Background::Background(int x, int y, int w, int h, std::string file, SDL_Renderer * gRender){
	this->background = new Sprite(x, y, w, h, 1, file, gRender);
}

Background::Background(Sprite* sp) {
	this->background = new Sprite(*sp);
}

Background::~Background()
{
	delete this->background;
}

Sprite* Background::getSprite() {
	return background;
}