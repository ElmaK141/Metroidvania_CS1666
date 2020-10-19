#include "tilemap.h"
#include "platformInfo.h"
#include <iostream>
#include <fstream>

// See tilemap.h for higher level context
/////////////////////////
// Public Constructors //
/////////////////////////

// Default Constructor (does nothing for now)
Tilemap::Tilemap()
{

}

// Initializes a tilemap object by passing the path
// to a file representing the tilemap as well as a vector
// containing references to every tile type encountered
// within the  tilemap.
// TO-DO: Automatic detection and passing of the relevant tiles
//		  pertinent to our tileArray vector? (Proc Gen do you hear me???)
Tilemap::Tilemap(std::string tilemap, std::vector<Tile*> tiles, Background* bg)
{
	// Assign values
	this->xMax = 0;			// Default placeholder
	this->yMax = 0;			// Default placeholder
	this->tileArray = tiles;
	this->bg = bg;

	// Use the path to load in the tilemap's
	// text representation and populate the
	// tileMap array
	this->generateTilemap(tilemap);
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

void Tilemap::drawTilemap(SDL_Renderer* render, int offset) {
	
	for (int i = 0; i < this->yMax; i++) {
		for (int j = 0; j < this->xMax; j++) {




			if (this->tileMap[i][j] != 0 && this->tileMap[i][j] != 3) {
				this->tileArray[this->tileMap[i][j] - 1]->getTileSprite()->draw(render, -offset + (j * 16), i * 16);
			}

			/*if (this->tileMap[i][j] == 1) {
				this->tileArray[0]->getTileSprite()->draw(render, -offset + (j * 16), i * 16);
			}

			if(this)*/

		}
	}

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

	// Generates a random layout of platforms of different sizes
	PlatformInfo info(this->xMax, this->yMax);

	//what if we post process with platforms?????
	//We would iterate over +/- 1 on each rowLocation -> do sections of three in post

	//init flag - by default it is an empty space
	int flag = 0;
	// Now that our tileMap array is initialized,
	// generate a flag value at each index
	for (int i = this->yMax-1; i > -1; i--)
	{
		//for this row i (starting from the bottom)

		// see if we have platforms in this row (within +/- 1) -> rowLocations
		//    if true: there may be a platform in this row
		//    if false: there is not a platform in this row
		//bool platFound = info.checkRow(i);

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
			//else if (i == (this->yMax - reach) && (j % 10 < 5)) { //test of some basic platforms
			//	flag = 2;
			//}
			else {
				flag = 0; //for now
			}

			// add flag to the array
			this->tileMap[i][j] = flag;
			//std::cout << tileMap[i][j] << " ";
		}
		//std::cout << "\n";

	}

	//not the most efficient tho
	//for each row that we says has platforms
	for (int i = 0; i < info.getNumRows(); i++) {

		// the center row that we are placing around
		int centerRow = info.getRowLocation(i);
		// number of platforms in this row total
		int numPlat = info.getNumPlatforms(i);
		// X positions of each platform in this row
		int* positions = info.getPositions(i);
		// Y position of each platform in this row
		int* heights = info.getHeights(i);
		// Length of each platform in this row
		int* lengths = info.getLengths(i);

		// go across Array of platforms and implant them into tilemap
		for (int j = 0; j < numPlat; j++) {
			// j is current platform in our list
			int x = positions[j];
			int y = heights[j];
			int len = lengths[j];

			std::cout << x << " " << y << " " << len << " " << std::endl;
			
			for (int k = x; k < x+len; k++) {
				//row y, col x, for len col
				this->tileMap[y][k] = 2;
			}
		}
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

Background* Tilemap::getBackground() {
	return this->bg;
}


/////////////////
// Destructor  //
/////////////////

// Make sure to clean up memory
Tilemap::~Tilemap()
{
	delete this->tileMap;
}

