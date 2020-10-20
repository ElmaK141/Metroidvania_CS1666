#include "block.h"
#include <iostream>
#include <stdlib.h>     /* srand, rand */
#include <time.h> 

// Block Types:
// Empty - 0
// Floor - 1 
// Ceiling - 2
// WallL - 3
// WallR - 4
// Middle - 5
// * Walls contain corners

// Block Constants

// Height and Width of the block in number of tiles
constexpr int height = 9;
constexpr int width = 10;

// Constants for y index
constexpr int bottom = height - 1;
constexpr int top = 0;

// Constants for x index
constexpr int left = 0;
constexpr int right = width - 1;

// Blocks have internal metadata flags - see block.h

// Creates a Block that will be at position col,row in terms of the block Map 2d array
Block::Block(int row, int col, int numRow, int numCol) {
	this->row = row;
	this->col = col;

	this->numRow = numRow;
	this->numCol = numCol;

	// determine the type of this block
	setType();

	// Init the metadata flags and data **DO WE NEED THIS??**
	initMetadata();

	// the map is a subsection of tile map that represents this block
	this->map = new int* [height];
	for (int i = 0; i < height; i++)
		this->map[i] = new int[width];

}

// init the metadata for this block
void Block::initMetadata() {
	// no block is set initially
	this->set = -1;
	
	// by default we have no connections
	/*this->connectedRx = false;
	this->connectedRy = false;
	this->connectedLx = false;
	this->connectedLy = false;*/

	this->x = -1;
	this->y = -1;

	//connections represent anything that crosses the boundary of a block that is not the wall/floor/edge


	/* For now idk if we need this lol what a waste of my time
	//switch on type to set metadata (that we know of by type)
	switch (this->type) {
	case BlockType::Empty:
		// not connected
		this->connnectedRx = false;
		this->connnectedRy = false;
		this->connectedLx = false;
		this->connectedLy = false;

		// none
		this->x = -1;
		this->y = -1;
		break;
	case BlockType::Middle:
		this->connnectedRx = false;
		this->connnectedRy = false;
		this->connectedLx = false;
		this->connectedLy = false;

		this->x = -1;
		this->y = -1;
		break;
	case BlockType::Floor:
		this->connnectedRx = false;
		this->connnectedRy = false;
		this->connectedLx = false;
		this->connectedLy = false;

		this->x = -1;
		this->y = -1;
		break;
	case BlockType::Ceiling:
		this->connnectedRx = false;
		this->connnectedRy = false;
		this->connectedLx = false;
		this->connectedLy = false;

		this->x = -1;
		this->y = -1;
		break;
	case BlockType::WallL:
		this->connnectedRx = false;
		this->connnectedRy = false;
		this->connectedLx = false;
		this->connectedLy = false;

		this->x = -1;
		this->y = -1;
		break;
	case BlockType::WallR:
		this->connnectedRx = false;
		this->connnectedRy = false;
		this->connectedLx = false;
		this->connectedLy = false;

		this->x = -1;
		this->y = -1;
		break;
	}
	*/
}

// based on the x,y coordinate of this block, determine the type
void Block::setType() {

	if (this->col == 0) { // WallL
		this->type = BlockType::WallL;
		if (this->row == 0) {
			this->corner = true;
		}
		else if (this->row == this->numRow - 1) {
			this->corner = true;
		}
	}
	else if (this->col == this->numCol - 1) { // WallR
		this->type = BlockType::WallR;
		if (this->row == 0) {
			this->corner = true;
		}
		else if (this->row == this->numRow - 1) {
			this->corner = true;
		}
	}
	else if (this->row == 0) { // Ceiling
		this->type = BlockType::Ceiling;
	}
	else if (this->row == this->numRow - 1) { // Floor
		this->type = BlockType::Floor;
	}
	else { // if it is not on the edge, it is a Middle block
		this->type = BlockType::Middle;
	}

	//Blocks can be set to Empty later on - based on the blocks around them
}

// methods for generating block based on type

void Block::generateBlock() {
	//switch on type
	switch (this->type) {
		case BlockType::Empty:
			generateEmpty();
			break;
		case BlockType::Middle:
			generateMiddle();
			break;
		case BlockType::Floor:
			generateFloor();
			break;
		case BlockType::Ceiling:
			generateCeiling();
			break;
		case BlockType::WallL:
			generateWallL();
			break;
		case BlockType::WallR:
			generateWallR();
			break;
	}
}

// generate an Empty block - tilemap of all 0s
void Block::generateEmpty() {
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			this->map[i][j] = 0;
		}
	}

	// Empty blocks are set
	this->set = 1;
}

// generate a Middle block - this block has no walls, and is only platforms
void Block::generateMiddle() {
	generateEmpty();
}

// generate a Floor block - this block always has 1s on the floor at least *except if we do doors in the floor*
void Block::generateFloor() {
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			
			// if we are at the bottom 
			if (i == bottom) {
				// floor is 1
				this->map[i][j] = 1;
			}
			else {
				// else empty (for now)
				this->map[i][j] = 0;
			}
			
			// we can add nuance here to create different types of floors with randomness


		}
	}
}

// generate a Ceiling block - this block always has 1s on the ceiling at least *except if we do doors in the ceiling*
void Block::generateCeiling() {
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {

			// if we are at the bottom 
			if (i == top) {
				// floor is 1
				this->map[i][j] = 1;
			}
			else {
				// else empty (for now)
				this->map[i][j] = 0;
			}

			// we can add nuance here to create different types of floors with randomness


		}
	}
}

// generate a WallL block - this block always has 1s on the left *except when it has a door*
void Block::generateWallL() {
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			
			//Leftmost column - if not a door
			if (j == left && !this->door) {
				this->map[i][j] = 1;
			}
			else if (this->corner) { //if we are a corner
				if (this->row == 0 && i == 0) { // ceiling of corner
					this->map[i][j] = 1;
				}
				else if (this->row == this->numRow - 1 && i == bottom) {// floor of corner
					this->map[i][j] = 1;
				}
				else { // not a floor/ceiling
					this->map[i][j] = 0;
				}
			}
			else { // otherwise

				// this space would allow us to create partial platforms in this block

				this->map[i][j] = 0;
			}
		}
	}

	// Initial set
	this->set = 0;
}

// generate a WallR block - this block always has 1s on the right *except when it has a door*
void Block::generateWallR() {
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {

			//Leftmost column - if not a door
			if (j == right && !this->door) {
				this->map[i][j] = 1;
			}
			else if (this->corner) { //if we are a corner
				if (this->row == 0 && i == 0) { // ceiling of corner
					this->map[i][j] = 1;
				}
				else if (this->row == this->numRow - 1 && i == bottom) {// floor of corner
					this->map[i][j] = 1;
				}
				else { // not a floor/ceiling
					this->map[i][j] = 0;
				}
			}
			else { // otherwise

				// this space would allow us to create partial platforms in this block

				this->map[i][j] = 0;
			}
		}
	}

	// Initial set
	this->set = 0;
}

// Check if block B is connected to this block
bool Block::checkBlock(Block* b) {
	
	//if B is left
	if (b->getCol() < this->col && b->isRight()) {
		return true;
	}
	//if B is right
	if (b->getCol() > this->col && b->isLeft()) {
		return true;
	}
	//if B is up
	if (b->getRow() < this->row && b->isBelow()) {
		return true;
	}
	//if B is down
	if (b->getRow() > this->row && b->isAbove()) {
		return true;
	}
	else {
		return false;
	}
}

// returns this blocks internal tilemap
int** Block::getBlockMap() {
	return this->map;
}


// Getters and Setters

// Set the block to the desired position
// Ideally only set:
//     -1 -> 0,
//     -1 -> 1,
//     0 -> 1
void Block::setBlock(int s) {
	this->set = s;
}

int Block::getSet() {
	return this->set;
}

int Block::getRow() {
	return this->row;
}

int Block::getCol() {
	return this->col;
}

/*
BlockType Block::getType() {
	return this->type;
}
*/

bool Block::isRight() {
	return this->connectedRight;
}

bool Block::isLeft() {
	return this->connectedLeft;
}

bool Block::isAbove() {
	return this->connectedAbove;
}

bool Block::isBelow() {
	return this->connectedBelow;
}

int Block::getXcon() {
	return this->x;
}

int Block::getYcon() {
	return this->y;
}