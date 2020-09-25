#include "tilemap.h"
#include <iostream>
#include <fstream>

Tilemap::Tilemap()
{
	this->yMax = 0;
	this->xMax = 0;
}

int** Tilemap::getTileArray(std::string filename)
{
	// First open our file and read in our array dimensions
	std::ifstream tileFile;
	tileFile.open(filename.c_str());

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
		int** tileMap = new int* [this->xMax];
		for (int i = 0; i < this->xMax; i++)
			tileMap[i] = new int[this->yMax];

		// Loop through our tilemap's dimensions
		// assigning values to each spot in the array
		for (int i = 0; i < this->xMax; i++)
		{
			for (int j = 0; j < this->yMax; j++)
			{
				tileFile.get(in);
				tileMap[i][j] = in;
			}
			// read and discard '\n'
			tileFile.get(in);
		}
		
		// Now that our tilefile has been read,
		// we can return our generated array
		// to be used for rendering
		tileFile.close();
		return tileMap;
	}
	else
		std::cout << "Error opening  tilemap file";

}

int Tilemap::getMaxWidth()
{
	return this->xMax;
}

int Tilemap::getMaxHeight()
{
	return this->yMax;
}

Tilemap::~Tilemap()
{
	// N/A?
}

