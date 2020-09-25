#include "background.h"


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