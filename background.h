#ifndef _BACKGROUND_H_
#define _BACKGROUND_H_

#include "sprite.h"
#include <fstream>

class Background {

public:
	Background(int x, int y, int w, int h, std::string file, SDL_Renderer* gRender);
	Background(Sprite* sp);
	~Background();
	Sprite* getSprite();
private:

	Sprite* background;
};


#endif