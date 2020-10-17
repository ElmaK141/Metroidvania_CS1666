#ifndef _TILEMAP_H_
#define _TILEMAP_H_
#endif

#include <string>
#include <vector>
#include "tile.h"
#include "sprite.h"

// Tilemap object used to store relevant
// flags pertinent to sprites drawn the screen
// for collision and interaction purposes. 
// Stores and returns an array representation
// of the array as well as a vector of tiles
// for drawing and property retrieval purposes
class Tilemap
{
		public:
		// Public Constructors:
			Tilemap();
			Tilemap(std::string filename, std::vector<Tile*> tiles);
		// Public Getters and Setters:			
			int getMaxWidth();
			int getMaxHeight();
			int** getTileMap();
			Tile* getTile(int index);
		// Public Destructor: 
			~Tilemap();
		private:
		// Private Tilemap Functions:
			void generateTilemap(std::string mapPath);
		// Private Tilemap Variables: 
			int** tileMap;				// An int array representing the flags at every spot in our tilemap
			int yMax;					// The height dimension of the tilemap
			int xMax;					// The width dimension of the tilemap
			std::vector<Tile*> tileArray;// Stores references to every tile type encountered in the tilemap
};

