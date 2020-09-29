#include "tilemap.h"
#include <iostream>
#include <fstream>


Tilemap::Tilemap(std::string tilemap, SDL_Renderer* r, std::vector<Sprite> tiles) {
	this->tiles = tiles;
	this->xMax = 0;
	this->yMax = 0;
	this->loadTileMap(tilemap, r);
	this->assignSprites();
}


void Tilemap::loadTileMap(std::string filename, SDL_Renderer* r)
{
	// First open our file and read in our array dimensions
	std::ifstream tileFile;
	tileFile.open(filename.c_str());

	//Tile** tileMap;

	// NOTE: We assume our text file representing our
	// tilemap object has AT LEAST the first two lines
	// representing the width and height of the tilemap, respectively
	if (tileFile.is_open())
	{
		// Declare some use variables
		char in;
		// First we read in the width and height of the tilemap
		tileFile >> this->yMax;
		tileFile >> this->xMax;

		// Now that we have our max values, we
		// can initialize our tilemap array

		this->tileMap = new Tile* [this->yMax];
		for (int i = 0; i < this->yMax; i++) {
			this->tileMap[i] = new Tile[this->xMax];
		}

		int i = 0;
		// Loop through our tilemap's dimensions
		// assigning values to each spot in the array

		for (int i = 0; i < this->yMax; i++)
		{
			for (int j = 0; j < this->xMax; j++)
			{
				tileFile.get(in);
				this->tileMap[i][j].setTilePos(i,j);
				this->tileMap[i][j].setTileFlag(in);
			}
			// read and discard '\n'
			tileFile.get(in);
		}
		
		// Now that our tilefile has been read,
		// we can return our generated array
		// to be used for rendering
		tileFile.close();
	}
	else
		std::cout << "Error opening  tilemap file";

}

Tile** Tilemap::getTileMap() {
	return this->tileMap;
}

Tile* Tilemap::getTile(int x, int y) {
	return &(tileMap[x][y]);
}

int Tilemap::getMaxWidth()
{
	return this->xMax;
}

int Tilemap::getMaxHeight()
{
	return this->yMax;
}

void Tilemap::assignSprites() {
	for (int i = 0; i < this->yMax; i++) {
		for (int j = 0; j < this->xMax; j++) {
			if (tileMap[i][j].getTileFlag() == 'w') {
				tileMap[i][j].setTileSprite(&this->tiles.at(1));
			}
			else {
				tileMap[i][j].setTileSprite(&this->tiles.at(0));
			}
		}
	}
}

Tilemap::~Tilemap() {
	delete this->tileMap;
}