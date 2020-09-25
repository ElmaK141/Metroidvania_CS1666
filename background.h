#ifndef _BACKGROUND_H_
#define _BACKGROUND_H_

#include "sprite.h"

class Background {

public:
	Background(Sprite* sp);
	~Background();
	Sprite* getSprite();
private:
	Sprite* background;
};


#endif