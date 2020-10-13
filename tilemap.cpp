#include "tilemap.h"
#include <iostream>
#include <fstream>

// See tilemap.h for higher level contex
/////////////////////////
// Public Constructors //
/////////////////////////

// Initializes a tilemap object by passing the path
// to a file representing the tilemap as well as a vector
// containing references to every tile type encountered
// within the  tilemap.
// TO-DO: Automatic detection and passing of the relevant tiles
//		  pertinent to our tileArray vector? (Proc Gen do you hear me???)
Tilemap::Tilemap(std::string tilemap, std::vector<Tile*> tiles)
{
	// Assign values
	this->xMax = 0;			// Default placeholder
	this->yMax = 0;			// Default placeholder
	this->tileArray = tiles;
	// Use the path to load in the tilemap's
	// text representation and populate the
	// tileMap array
	this->generateTilemap(tilemap);
	// this->assignSprites();
}

// Initializes a tile map based on given X and Y dimensions.
// Does not use a text file, instead tries to create a Tilemap using randomness
Tilemap::Tilemap(int xDim, int yDim, std::vector<Tile*> tiles) {
	//Assign attributes
	this->yMax = yDim;
	this->xMax = xDim;
	this->tileArray = tiles;

	//generate tilemap without text file
	this->generateTilemap();


}
///////////////////////
// Private Functions //
///////////////////////

// Our generateTilemap function simply
// reads in the data passed during construction
// and uses it to initialize and populate
// our tileMap array
void Tilemap::generateTilemap(std::string mapPath)
{
	// Read in the data pointed to by
	// our passed mapPath
	std::ifstream tileFile;
	tileFile.open(mapPath.c_str());

	// Now that our file is open,
	// we read in the dimensions
	// our our tilemap array and then 
	// iterate through, populating the
	// array with the values.
	// NOTE: It is imperative that the tilemap
	// .txt file is in the following format:
	// Line 0: xDimension (max)
	// Line 1: yDimension (max)
	// Line 2: tilemap contents
	// Line 3: tilemap contents
	// Line 4: tilemap contents
	if (tileFile.is_open())
	{
		// initialize a temp variable
		// to hold our read in input,
		// then  begin stepping through our
		// tilefile
		char readIn;
		// Read in the first two lines
		tileFile >> this->xMax;
		tileFile >> this->yMax;
		// Drop our trailing newline
		tileFile.get(readIn);

		// Use our read in dimensions to 
		// initialize our 2d tilemap array
		this->tileMap = new int* [this->yMax];
		for (int i = 0; i < this->yMax; i++)
			this->tileMap[i] = new int[this->xMax];

		// Now that our tileMap array is initialized,
		// step through the text file and read in the 
		// values
		for (int i = 0; i < this->yMax; i++)
		{
			for (int j = 0; j < this->xMax; j++)
			{
				// Get input and 
				// add to the array
				tileFile.get(readIn);
				this->tileMap[i][j] = (readIn - 48);
				//std::cout << tileMap[i][j] << " ";
			}
			// Drop our trailing newline
			tileFile.get(readIn);
			//std::cout << "\n";
		}

		// We're finished, close our file
		tileFile.close();
	}
	// Failed to open??
	else
		std::cout << "Error opening  tilemap file";
}

// Initialize and populate tileMap array without use of
// an external text file.
void Tilemap::generateTilemap() {
	// Use our dimensions to 
	// initialize our 2d tilemap array
	this->tileMap = new int* [this->yMax];
	for (int i = 0; i < this->yMax; i++)
		this->tileMap[i] = new int[this->xMax];

	// Variables for restrictions	
	int playerW = 3; //Side facing -> 3 tiles 
	int playerH = 9; //Height -> 9 tiles high

	//Jump -> 21 tiles high (head reaches tile 21
	int reach = 10; //Jump -> from standing on 0, top of 11 (feet)
	
	//0 = empty    1 = walls    2 = platform

	// RULES:
	// Must always have a wall around the very outside edge of all 4 sides
	// Platforms must be 2*reach tiles below the ceiling
	// Platforms should be height+1 off the ground
	// Platforms cannot be in the same column within height+1

	//idea: is this best to do recursively? - save time and computation to backtrack if needed?

	//init flag - by default it is an empty space
	int flag = 0;
	// Now that our tileMap array is initialized,
	// generate a flag value at each index
	for (int i = this->yMax-1; i > -1; i--)
	{
		//for this row i (starting from the bottom)

		// for this row, can we have platforms?
			// if no: 0
		// if yes: "random" generate and ensure:
			// # of platforms in this row (and size of each, including if this fits the xMax)
				//random number of platforms on this row
				//of a random size each (within a range)
				//at random but acceptable starting positions relative to each other
			// ensure that they are "traversable" - for now, that there is no platform/wall above them that would prevent the player from going on them
				//platform is not allowed if you cannot walk onto it
				//OR reach it (hard)

		// implementation:
			//generate a random spread of tiles that will be located within reach
			// try to generate several rows that will be reachable from the previous row
		//idea: can we keep track of which rows are allowed to have platforms? for quick back checking?
			// if i want to put platforms on row 20, where is the next row below this - can it be reached?
			// dont even get me started on reaching in x and y

		// in here, vars that determine the platforms for this row, which can be used with j below to place platforms

		for (int j = 0; j < this->xMax; j++)
		{
			//for this row i and this col j


			//check if this is an outer ring tile
			if (i == 0 || i == this->yMax - 1 || j == 0 || j == this->xMax - 1) {
				flag = 1; //always wall
			}
			else if ((i > 0 && i < reach) || (i > this->yMax - 1 - playerH && i < this->yMax - 1)) { //these rows violate the wall/ceiling rules
				flag = 0; //must be empty
			}
			else if (i == (this->yMax - 3 - playerH) && (j % 10 < 5)) { //test of some basic platforms
				flag = 2;
			}
			else {
				flag = 0; //for now
			}








			// add flag to the array
			this->tileMap[i][j] = flag;
			//std::cout << tileMap[i][j] << " ";
		}
		//std::cout << "\n";

	}
}
/////////////////////////
// Getters and Setters //
/////////////////////////

// Width and Height Getters
int Tilemap::getMaxWidth()
{
	return this->xMax;
}
int Tilemap::getMaxHeight()
{
	return this->yMax;
}

// Tilemap Getter
int** Tilemap::getTileMap()
{
	return this->tileMap;
}

// Tile Getter
// WARNING: DON'T GO OUT OF BOUNDS ON THE INDEX PLSS
Tile* Tilemap::getTile(int index)
{
	return this->tileArray.at(index);
}
/////////////////
// Destructor  //
/////////////////

// Make sure to clean up memory
Tilemap::~Tilemap()
{
	delete this->tileMap;
}

