#ifndef _TILE_H_
#define _TILE_H_

#include "sprite.h"

class Tile {

	public:
		Tile(Sprite* sp);
		~Tile();
	private:
		Sprite* tile;
};

#endif