#include "tilemap.h"
//#include "platformInfo.h"
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
Tilemap::Tilemap(int xDim, int yDim, std::vector<Tile*> tiles, Background* bg) {
	//Assign attributes
	this->yMax = yDim;
	this->xMax = xDim;
	this->tileArray = tiles;
	this->bg = bg;

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
	
	//using block system
	// so we have room size: 45x210
	// we define a block that is: 9x10
	// so any room/map is a 2d array of 5x21 blocks

	// Floor
	// Ceiling
	// WallL
	// WallR
	// Corners*
	// (Middle) Platforms
	// Empty
	
	// blockMap = new Block* [5];
	// for(int i = 0; i < 5; i++){
	//     blockMap[i] = new Block[21];
	// }
	
	// iterate over blockMap and create Blocks in each
	
	// Then we would generate one block at a time -> from bottom up, going across
	
	// create the block, check around it, generate it (passing in list of blocks around it that connect)
		//curr->checkBlock(b) -> check all surrounding blocks against current block
		// pass in an array of the blocks that DO connect to this block
	// This way, when generating the block, it accounts for the blocks around it -> knows where the block next to it would like to connect to it (if we decide to connect to it)
		// maybe we also include a priority for connection (must connect vs can connect)
	// After generating the block, we set it 0
	// If a block has 0s and 1s on all sides, it becomes 1 - perma set

	// ** Door set here somewhere? **


	// Any block needs:
	//     Metadata: what blocks CAN be around it
	//     What defines this block
	//     Where we get this block definition from?

	// For block index i,j: pick from appropriate pool
	// check the blocks around it to ensure compatibility (looking at set and other metadata)
	// if pass: move on (set = 1), if fail: try another from the pool OR (set = -1) and upon setting next block can backtrace


	// Use our dimensions to 
	// initialize our 2d tilemap array
	this->tileMap = new int* [this->yMax];
	for (int i = 0; i < this->yMax; i++)
		this->tileMap[i] = new int[this->xMax];

	// THIS WILL BE CHANGED WHEN BLOCK IS FINISHED OR CLOSER TO
	// Here we would iterate through blockMap and convert our completed blockMap into a final tilemap

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

