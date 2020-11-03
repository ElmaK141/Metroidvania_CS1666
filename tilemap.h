#ifndef _TILEMAP_H_
#define _TILEMAP_H_


#include <string>
#include <vector>
#include "tile.h"
#include "sprite.h"
#include "background.h"

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
			//Tilemap(std::string filename, std::vector<Tile*> tiles);
			Tilemap(int xDim, int yDim, int room, std::vector<Tile*> tiles, Background* bg);
			Tilemap(std::string filename, std::vector<Tile*> tiles, Background* bg);
		// Public Getters and Setters:			
			int getMaxWidth();
			int getMaxHeight();
			int** getTileMap();
			Tile* getTile(int index);
			Background* getBackground();
			void drawTilemap(SDL_Renderer* render, int offset);
		// Public Destructor: 
			~Tilemap();
		private:
		// Private Tilemap Functions:
			void generateTilemap(std::string mapPath);
			void generateTilemap();
		// Private Tilemap Variables: 
			int** tileMap;				// An int array representing the flags at every spot in our tilemap
			Background* bg;
			int yMax;					// The height dimension of the tilemap
			int xMax;					// The width dimension of the tilemap
			int room;					// Temp way to say which side the door is on for block gen
			std::vector<Tile*> tileArray;// Stores references to every tile type encountered in the tilemap
};

#endif