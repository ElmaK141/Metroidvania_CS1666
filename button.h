#ifndef _BUTTON_H_
#define _BUTTON_H_

#include "sprite.h"

class Button {

public:
	Button(int x, int y, int w, int h, int scale, std::string file, SDL_Renderer* gRender);
	Button(Sprite* sp);
	~Button();
	Sprite* getSprite();
private:
	Sprite* button;
};

#endif