#include "gamemap.h"
#include "block.h"
#include <stdlib.h>     /* srand, rand */
#include <time.h> 
#include <iostream>
#include <queue>

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

	//Create Map - 2D array of Tilemap Nodes
	map = new Node * [mapHeight];
	for (int i = 0; i < mapHeight; i++) {
		map[i] = new Node[mapLength];
		for (int j = 0; j < mapLength; j++) {
			map[i][j].valid = false;
		}
	}

	//Now we generate the Map - this is where we would define and create each Tilemap/Room
	// We know of Tiles, Bgs, Size, and Tilemap Array internally
	generateGamemap();

	/* print debug
	for (int i = 0; i < mapHeight; i++) {
		for (int j = 0; j < mapLength; j++) {
			if(map[i][j].valid)
				std::cout << map[i][j].doors << "\t";
			else
				std::cout << -1 << "\t";
		}
		std::cout << "\n" << std::endl;
	}
	*/
}

// Make sure to clean up memory
Gamemap::~Gamemap()
{
	delete this->map;
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

	// randomly choose a starting location for the player spawn room and map gen
	int spawnX = rand() % mapLength;
	int spawnY = rand() % mapHeight;

	// set current position to this tilemap - player starts in the spawn room
	setCurrentPosition(spawnX, spawnY);

	// min number of rooms we will allow
	int minRooms = 4;

	// then when we generate the gamemap, we will choose how many rooms in the map will be generated (one of these rooms is the spawn room)
	int numRooms = rand() % ((mapLength * mapHeight) - (minRooms - 1)) + minRooms;

	// create the vector that we will use as our queue
	std::queue<Node*> q;
	
	// create spawn node, add to array, add to queue
	struct Node spawn = {true, spawnX, spawnY, nullptr, 0};
	map[spawnY][spawnX] = spawn;
	q.push(&spawn);
	// spawn node created (and doors it has) based on where it is

	// when we add a node to the array, we count a room. spawn counts as a room, so we count for it
	numRooms -= 1;

	// while the stack is not empty - process next node
	while (!q.empty()) {
		// get the current node, and remove it from the queue
		Node* curr = q.front();
		q.pop();

		// If this is the Spawn room, we are predefined for our children nodes
		if (curr->x == spawnX && curr->y == spawnY) {
			int spawnRooms = getSpawnRooms(spawnX);

			// create children nodes
			if (spawnRooms == 3) { // left and right
				// create both children nodes, one on the right, one on the left
				Node r = { true, spawnX + 1, spawnY, nullptr, 2 };
				map[spawnY][spawnX + 1] = r;
				Node l = { true, spawnX - 1, spawnY, nullptr, 1 };
				map[spawnY][spawnX - 1] = l;
				
				// add them to queue
				q.push(&r);
				q.push(&l);
				numRooms -= 2;
			}
			else if (spawnRooms == 1) { // right
				// create the node for this child room, located one to the right of the spawn room
				// rooms value is default 2 because it knows it has a parent on the left
				Node c = { true, spawnX + 1, spawnY, nullptr, 2 };
				map[spawnY][spawnX + 1] = c;
				q.push(&c);
				numRooms -= 1;
			}
			else if (spawnRooms == 2) { // left
				// create the node for this child room, located one to the left of the spawn room
				// rooms value is default 1 because it knows it has a parent on the right
				Node c = { true, spawnX - 1, spawnY, nullptr, 1 };
				map[spawnY][spawnX - 1] = c;
				q.push(&c);
				numRooms -= 1;
			}

			// generate the spawn room based on template and record doors
			curr->doors = spawnRooms;
			curr->t = new Tilemap(getSpawn(spawnX), tiles, bgs[rand() % bgs.size()]);

			//std::cout << "Created spawn at " << spawnY << " " << spawnX << " " << spawnRooms << " " << curr->doors << "\n" << std::endl;
		}
		else { // This room is not the spawn room

			//std::cout << "Procesing Node at " << curr->y << " " << curr->x << "\n" << std::endl;

			// int for the doors off of this one
			int rooms = 0;

			// if we are allowed to make a new room, try to
			if (numRooms > 0) { // we can make a new room (at least one)
				
				//std::cout << numRooms << " remaining rooms\n";

				// how many should we try to make? anywhere from 1 to 3
				int r = rand() % std::min(3, numRooms) + 1;

				//std::cout << r << " rooms to create\n";

				// for each number, try to find a valid direction, if we can, make room and add it
				for (int i = 0; i < r; i++) {
					// direction is random - 0-3, converted to 4-bit
					int dir = randToDir(rand() % 4);
					int tries = 4;
					
					while (!isValid(curr->x, curr->y, dir) && tries > 0) {
						//std::cout << dir << "\n" << std::endl;
						dir = randToDir(rand() % 4);
						tries--;
					}

					//std::cout << "Create room in direction " << dir << "\n" << std::endl;

					if (isValid(curr->x, curr->y, dir)) { // if this is valid, then we can create a child
						int xAdj = xFromDir(dir);
						int yAdj = yFromDir(dir);

						int startX = curr->x;
						int startY = curr->y;

						//std::cout << "Current: " << curr->y << " " << curr->x << "\n";
						//std::cout << "Start: " << startY << " " << startX << "\n";

						int from = parentDir(dir);
						Node c = { true, startX + xAdj, startY + yAdj, nullptr, from};

						//std::cout << "Adjust: " << yAdj << " " << xAdj << "\n";

						map[startY + yAdj][startX + xAdj] = c;
						q.push(&c);
						numRooms -= 1;

						// count this door
						rooms += dir;
						//std::cout << "Added room at " << startY + yAdj << " " << startX + xAdj << "\n" << std::endl;
					}
				}
			}

			// add the doors
			curr->doors += rooms;

			// generate a procgen room with these doors
			curr->t = new Tilemap(210, 45, rooms, tiles, bgs[rand() % bgs.size()]);
		}
		// update our node (since we have updated doors and t)
		map[curr->y][curr->x] = *curr;
	}

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
}

// set the position of which room in the map the player is in
void Gamemap::setCurrentPosition(int x, int y) {
	currXPos = x;
	currYPos = y;
}

// returns a pointer to the current Tilemap that the player is in
Tilemap* Gamemap::getCurrentRoom() {
	return map[currYPos][currXPos].t;
}

// gets the template for the spawn room
std::string Gamemap::getSpawn(int x) {
	std::string s;
	if (x > 0 && x < mapLength - 1) { // if not on either edge, door on both sides
		s = "data/tilemaps/spawn/spawnroomLR.txt";
	}
	else if (x == 0) { // if on the left, door on right
		s = "data/tilemaps/spawn/spawnroomR.txt";
	}
	else { // if on the right, door on left
		s = "data/tilemaps/spawn/spawnroomL.txt";
	}
	return s;
}

// gets the "rooms" value for the spawn room
int Gamemap::getSpawnRooms(int x){
	int r;
	if (x > 0 && x < mapLength - 1) { // if not on either edge, door on both sides
		r = 3;
	}
	else if (x == 0) { // if on the left, door on right
		r = 1;
	}
	else { // if on the right, door on left
		r = 2;
	}
	return r;
}

//  converts the rand 0-3 for direction into our "4-bit"
int Gamemap::randToDir(int r) {
	if (r == 0) { // right
		return 1;
	}
	else if (r == 1) { //left
		return 2;
	}
	else if (r == 2) { //down
		return 4;
	}
	else { //up
		return 8;
	}
}

// checks if the direction chosen is valid for a new room based on where we are now and the direction we want to go in.
bool Gamemap::isValid(int x, int y, int dir) {
	bool ret = false;

	//std::cout << x << " " << y << " " << dir << " " << std::endl;

	// if a node is already there OR if it goes out of bounds - invalid
	
	if (dir == 1 && x != mapLength - 1) { // if right is valid
		if (map[y][x + 1].valid == false) //check node
			ret = true;
	}
	else if (dir == 2 && x != 0) { // if left is valid
		if (map[y][x - 1].valid == false) //check node
			ret = true;
	}
	else if (dir == 4 && y != mapHeight - 1) { // if down is valid
		if (map[y + 1][x].valid == false) //check node
			ret = true;
	}
	else if(dir == 8 && y != 0) { // if up is valid
		if (map[y - 1][x].valid == false) //check node
			ret = true;
	}

	//std::cout << ret << "\n" << std::endl;
	return ret;
}

// converts the dir value into x adjustment
int Gamemap::xFromDir(int dir){
	if (dir == 1)
		return 1;
	else if (dir == 2)
		return -1;
	else
		return 0;
}

// converts the dir value into y adjustment
int Gamemap::yFromDir(int dir) {
	if (dir == 4)
		return 1;
	else if (dir == 8)
		return -1;
	else
		return 0;
}

// convert dir into direction for child node
int Gamemap::parentDir(int dir) {
	if (dir == 1) { // if we go right, come from left
		return 2;
	}
	else if (dir == 2) { // if we go left, come from right
		return 1;
	}
	else if (dir == 4) { // if we go down, come from up
		return 8;
	}
	else { // if we go up, come from down
		return 4;
	}
}