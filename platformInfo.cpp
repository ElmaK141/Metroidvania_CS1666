#include "platformInfo.h"
#include <iostream>
#include <stdlib.h>     /* srand, rand */
#include <time.h> 

// Player characteristics that determine platform position
constexpr int HEIGHT = 9; //player height
constexpr int REACH = 10; //player jump height (reach)
constexpr int WIDTH = 3; //player width

// Min/Max of platform lengths
constexpr int platMin = 4;
constexpr int platMax = 8;


// Creates and stores a platform info object
// This object will randomly generate information about the platforms that will be rendered in a scene
// This information is used to create a "random" tilemap at any time
PlatformInfo::PlatformInfo(int x, int y) {
	this->xMax = x;
	this->yMax = y;

	generateConditions();

	generatePlatforms();
}

// First we use RNG to determine the:
//    # of rows of platforms
//    y coordinate of each row
//    # of platforms in each row
void PlatformInfo::generateConditions() {
	int range = yMax - REACH - REACH; // range in middle where platforms are allowed

	//number of rows is roughly related to the number of rows we can reach
	this->nRows = range / REACH;
	if (range % REACH >= 5) { // 5 or more remainder we will try an extra platform
		this->nRows++;
	}
	
	// rowLocations and numPlatforms have an index for each row.
	this->rowLocations = new int[this->nRows];
	this->numPlatforms = new int[this->nRows];

	//first row is always with reach of the floor
	this->rowLocations[0] = yMax - REACH;

	std::cout << this->rowLocations[0] << " " << std::endl;

	// seed with the time
	srand(time(NULL));

	// Now generate row locations (y coordinate of this row where platforms will be)
	for(int i = 1; i < this->nRows; i++) {
		//random number between range
		this->rowLocations[i] = rand() % (this->yMax - 2*REACH) + REACH;
		std::cout << this->rowLocations[i] << " " << std::endl;
	}

	std::cout << "\n" << std::endl;

	// max and min numbers of platforms on any given row
	int maxPlat = xMax / (platMin + WIDTH);
	int minPlat = xMax / (platMax + WIDTH);

	// Now generate number of platforms in each row
	for (int i = 0; i < this->nRows; i++) {
		// random number between minPlat and maxPlat
		this->numPlatforms[i] = rand() % maxPlat + minPlat;
		std::cout << this->numPlatforms[i] << " " << std::endl;
	}

	std::cout << "\n" << std::endl;
}

// Use RNG to determine:
//    X starting position of each platform
//    Length of each platform
//    *Actual y placement of platform is around its row*
void PlatformInfo::generatePlatforms() {

	// create platformPositions and platformLengths
	this->platformPositions = new int* [this->nRows];
	this->platformHeights = new int* [this->nRows];
	this->platformLengths = new int* [this->nRows];
	for (int i = 0; i < this->nRows; i++) {
		this->platformPositions[i] = new int[this->numPlatforms[i]];
		this->platformHeights[i] = new int[this->numPlatforms[i]];
		this->platformLengths[i] = new int[this->numPlatforms[i]];
	}
	
	// new seed, new random ;)
	srand(time(NULL));

	// populate both arrays
	for (int i = 0; i < this->nRows; i++) {
		for (int j = 0; j < this->numPlatforms[i]; j++) {

			// place X anywhere between 1 and xMax-1 position
			int xPos = rand() % (this->xMax - 1) + 1;
			this->platformPositions[i][j] = xPos;

			//place Y +/- 1 around the row
			int yPos = rand() % 3 - 1;
			this->platformHeights[i][j] = this->rowLocations[i] + yPos;

			// length between minLen and maxLen
			int len = rand() % platMax + platMin;
			this->platformLengths[i][j] = len;

			//std::cout << xPos << " " << yPos << " " << len << "\n" << std::endl;
		}
	}
}

// checks if this row is allowed to have platforms - includes the +/- tolerance
bool PlatformInfo::checkRow(int row) {
	bool ret = false;

	if (row >= REACH && row <= this->yMax - REACH) { //if this is an acceptable platform row
		for (int i = 0; i < this->nRows; i++) { // check the chosen platform rows
			if (row < this->rowLocations[i] + 1 && row > this->rowLocations[i] - 1) { //tolerance
				for (int j = 0; j < this->numPlatforms[i]; j++) {
					//if()
				}
				ret = true;
				break;
			}
		}
	}
	return ret;
}


int PlatformInfo::getNumRows() {
	return this->nRows;
}

int PlatformInfo::getRowLocation(int i) {
	return this->rowLocations[i];
}

int PlatformInfo::getNumPlatforms(int i) {
	return this->numPlatforms[i];
}

int* PlatformInfo::getPositions(int i) {
	return this->platformPositions[i];
}

int* PlatformInfo::getHeights(int i) {
	return this->platformHeights[i];
}

int* PlatformInfo::getLengths(int i) {
	return this->platformLengths[i];
}