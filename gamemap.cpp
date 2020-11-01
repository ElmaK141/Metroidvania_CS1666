#include "gamemap.h"
#include "block.h"
#include <stdlib.h>     /* srand, rand */
#include <time.h> 
#include <iostream>

// See gamemap.h for higher level context
/////////////////////////
// Public Constructor  //
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

	//Map should be a Graph, where each node is a room, and edges represent a door between them
	// our graph will be laid out like a 2d array (vertex can have x,y pos var)
	// Maybe we have a 2d array of our nodes, and each node has a tilemap
	// then we can easily traverse the graph, and change which room we are in - returning current nodes tilemap

	//Create Map - 2D array of Tilemaps
	map = new Tilemap * [mapHeight];
	for (int i = 0; i < mapHeight; i++)
		map[i] = new Tilemap[mapLength];

	//Now we generate the Map - this is where we would define and create each Tilemap/Room
	// We know of Tiles, Bgs, Size, and Tilemap Array internally
	generateGamemap();

}

/* generates the full gamemap, using randomness (and some predefined values if we want) to define:
*	How many rooms we will have
*	Where these roms will be
*	What size they will be
*	Where they will connect to each other (doors)
*  It will then generate each room as a tilemap - with randomness used to layout platforms
*/
void Gamemap::generateGamemap() {
	//init rand seed based on current time
	srand(time(NULL));

	// randomly choose a starting location for the player spawn room
	int spawnX = rand() % mapLength;
	int spawnY = rand() % mapHeight;

	// create and place the spawn room in the map at the random location
	map[spawnY][spawnX] = *(new Tilemap("data/tilemaps/spawnroom.txt", tiles, bgs[rand() % bgs.size()]));
	// spawn room will have 2 doors max - left and right (unless we cannot have a left/right)
	// as of right now, the spawn room does not have doors - the doors must be set for it like for all other rooms?
	// doors cannot exist until we know where they must be - based on the rooms it will connect to

	// set current position to this tilemap - player starts in the spawn room
	setCurrentPosition(spawnX, spawnY);

	// min number of rooms we will allow
	int minRooms = 4;

	// then when we generate the gamemap, we will choose how many rooms in the map will be generated (one of these rooms is the spawn room)
	int numRooms = rand() % ((mapLength * mapHeight) - (minRooms - 1)) + minRooms;

	// the gamemap will create rooms that connect off of the spawn room, as long as every room is reachable from there we are good
	// could we BFS or DFS from spawn? Some sort of way to recursively branch rooms out from the spawn
	// ** THIS IS IMPORTANT - WHAT IS THE BEST WAY TO RANDOMLY CREATE THE MAP - SIMILAR TO WHAT DUNGEON CRAWLER DID??? **
	// doing it this way will allows us to ensure all of our rooms are reachable

	// Needs to be done recursively

	// generate the "path", then we can iterate over all rooms and create 
	// from spawn, create a room in 1 direction

	/*
	Create spawn room vertex as root node
		must keep track of graph size in terms of 2d array (map height and length)
	Starting from root node (which we will say only has doors on left/right when possible)
		Create up to 2 children (left and/or right)
		An edge connects these nodes to the spawn (tells us there should be a door
	Then for each node, based on the number of rooms we have vs number of rooms we are allowed:
		Randomly decide the number of children and which directions
			Cannot move outside of the map or into a node that already exists
		Create those children (as long as we have rooms avail)
		Add the children to the queue
	Breadth First approach: Say we have 3x3 map with 5 rooms
		Spawn is selected at left wall in the middle
			It gets 1 room to the right R1
		R1 rolls 2 rooms, one up, one right
			Creates R2 up, Creates R3 right
		R2 rolls 2, it cannot go up or down
			Creates R4 left, cannot create any more rooms (all 5 rooms exist)
		Process R3 and R4 (nothing happens because no more rooms
	Final layout:
	R4	R2	0
	S	R1	R3
	0	0	0
	*/




	// This is not even worth doing
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

// returns a pointer to the current Tilemap that the player is in
Tilemap* Gamemap::getCurrentRoom() {
	return &map[currYPos][currXPos];
}