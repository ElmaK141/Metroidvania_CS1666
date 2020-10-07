#include "button.h"

Button::Button(int x, int y, int w, int h, int scale, std::string file, SDL_Renderer* gRender) {
	this->button = new Sprite(x, y, w, h, scale, file, gRender);
}

Button::Button(Sprite* sp) {
	this->button = new Sprite(*sp);
}

Button::~Button()
{
	delete this->button;
}

Sprite* Button::getSprite() {
	return button;
}