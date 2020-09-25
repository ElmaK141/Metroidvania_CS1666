#ifndef _TILE_H_
#define _TILE_H_

#include "sprite.h"

class Tile {

	public:
		Tile(Sprite* sp);
		~Tile();
		Sprite* getSprite();
	private:
		Sprite* tile;
};


#endif