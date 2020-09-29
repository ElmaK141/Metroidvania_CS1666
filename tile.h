#ifndef _TILE_H_
#define _TILE_H_

#include "sprite.h"

class Tile {

	public:
		Tile(Sprite* sp);
		Tile();
		~Tile();

		void setTilePos(int x, int y);
		int getTileXPos();
		int getTileYPos();

		void setTileFlag(char c);
		char getTileFlag();

		Sprite* getTileSprite();
		void setTileSprite(Sprite* sp);

	private:

		char flag;
		int xPos;
		int yPos;
		Sprite* tile;


};


#endif
