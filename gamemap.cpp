#include "gamemap.h"
#include "block.h"
#include <stdlib.h>     /* srand, rand */
#include <time.h> 
#include <iostream>

// See gamemap.h for higher level context
/////////////////////////
// Public Constructors //
/////////////////////////

// Default Constructor
Gamemap::Gamemap(int length, int height, std::vector<Tile*> tiles, std::vector<Background*> bgs)
{
	// set map size
	mapLength = length;
	mapHeight = height;

	// set references to Tiles array and Backgrounds array
	this->tiles = tiles;
	this->bgs = bgs;

	//Create Map - 2D array of Tilemaps
	map = new Tilemap * [mapHeight];
	for (int i = 0; i < mapHeight; i++)
		map[i] = new Tilemap[mapLength];

	//Now we generate the Map - this is where we would define and create each Tilemap/Room
	// We know of Tiles, Bgs, Size, and Tilemap Array internally
	generateGamemap();

}

void Gamemap::generateGamemap() {
	//init rand seed based on current time
	srand(time(NULL));

	// randomly choose a starting location for the player spawn room
	int spawnX = rand() % mapLength;
	int spawnY = rand() % mapHeight;

	// first thing we do is place the spawn room - the spawn room will be a predefined room
		// then when we generate the gamemap, we will choose how many rooms in the map will be generated
		// the gamemap will create rooms that connect off of the spawn room, as long as every room is reachable from there we are good
	// spawn room will have 2 doors min?
	// create and place this room in map
	map[spawnY][spawnX] = *(new Tilemap("data/tilemaps/spawnroom.txt", tiles, bgs[rand() % bgs.size()]));

	// set current position to this tilemap
	setCurrentPosition(spawnX, spawnY);

	// could we BFS or DFS from spawn?


	//For each room/tilemap - generate information about the room, then create the tilemap
	for (int i = 0; i < mapHeight; i++) {
		for (int j = 0; j < mapLength; j++) {

			// 3 room sizes (right now)
			int roomSize = rand() % 3;		//For now, equal probability for small, medium, or large room

			//TODO: Make verticle rooms too, and also rooms with different proportions.
			//Right now every room has doors on the left or right and we will need ones with doors on the top/bottom

			// randomly select the background that we will use for this room
			int bg = rand() % bgs.size();

			switch (roomSize) {
			case 0:		//small room
				map[i][j] = *(new Tilemap(90, 45, 0, tiles, bgs[bg]));
				break;
			case 1:		//medium room
				map[i][j] = *(new Tilemap(210, 45, 0, tiles, bgs[bg]));
				break;
			case 2:		//large room
				map[i][j] = *(new Tilemap(420, 45, 0, tiles, bgs[bg]));
				break;
				;
			}
		}
	}
}

// set the position of which room in the map the player is in
void Gamemap::setCurrentPosition(int x, int y) {
	currXPos = x;
	currYPos = y;
}