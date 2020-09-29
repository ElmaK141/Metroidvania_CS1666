#ifndef _TILEMAP_H_
#define _TILEMAP_H_
#endif

#include <string>
#include <vector>
#include "tile.h"
#include "sprite.h"

// Suggested Modification:
//		- Include tile object and use for the tilemap
//		- Include function to read a tilemap from a function
class Tilemap
{
		public:
			Tilemap(std::string filename, SDL_Renderer* r, std::vector<Sprite> tiles);
			
			
			int getMaxWidth();
			int getMaxHeight();
			
			Tile** getTileMap();
			Tile* getTile(int x, int y);

			~Tilemap();
		private:

			void assignSprites();
			void loadTileMap(std::string filename, SDL_Renderer* r);
			Tile** tileMap;
			int yMax;
			int xMax;
			std::vector<Sprite> tiles;
};