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
Gamemap::Gamemap(int length, int height, int type, std::vector<Tile*> tiles, std::vector<Background*> bgs)
{
	// set map size
	mapLength = length;
	mapHeight = height;

	// the TYPE will be 0-3
	// 0 - main room - loads from tilemap
	// 1 - first section - lxh map with tilemaps generated for no powerups
	// 2 - second section - lxh map with tilemaps generated for double jump
	// 3 - boss room - probably loads from tilemap
	this->type = type;

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
	if (type == 0) { //type 0, create the Main Room tilemap for this 1x1 map
		setCurrentPosition(0, 0);
		struct Node mainRoom = { true, 0, 0, new Tilemap("data/tilemaps/tilemap0.txt", tiles, bgs[0]), 0 };
		map[0][0] = mainRoom;
	}
	else if (type == 1 || type == 2) { // types 1 and 2 will generate a full gamemap with proc gen
		generateGamemap();
	}
	else { // type 4, create the Boss Room tilemap for this 1x1 map
		setCurrentPosition(0, 0);
		struct Node bossRoom = { true, 0, 0, new Tilemap("data/tilemaps/tilemap0.txt", tiles, bgs[1]), 0 }; // CHANGE THIS TO BOSS
		map[0][0] = bossRoom;
	}

	/* print debug */
	std::cout << "Spawn: "  << spawnY << " " << spawnX << std::endl;

	for (int i = 0; i < mapHeight; i++) {
		for (int j = 0; j < mapLength; j++) {
			if(map[i][j].valid)
				std::cout << map[i][j].doors << "\t";
			else
				std::cout << -1 << "\t";
		}
		std::cout << "\n" << std::endl;
	}
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
	this->spawnX = rand() % mapLength;
	this->spawnY = rand() % mapHeight;

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

	//std::cout << "Spawn " << spawn.t << " " << &spawn << std::endl;

	// when we add a node to the array, we count a room. spawn counts as a room, so we count for it
	numRooms -= 1;

	// while the stack is not empty - process next node
	while (!q.empty()) {
		// get the current node, and remove it from the queue
		Node* curr = q.front();
		q.pop();

		//std::cout << "Curr at pop " << curr->t << " " << curr << std::endl;

		// If this is the Spawn room, we are predefined for our children nodes
		if (curr->x == spawnX && curr->y == spawnY) {
			int spawnRooms = getSpawnRooms(spawnX);

			// create children nodes
			if (spawnRooms == 3) { // left and right
				// create both children nodes, one on the right, one on the left
				
				// Right node
				map[spawnY][spawnX + 1].valid = true;
				map[spawnY][spawnX + 1].x = spawnX + 1;
				map[spawnY][spawnX + 1].y = spawnY;
				map[spawnY][spawnX + 1].t = nullptr;
				map[spawnY][spawnX + 1].doors = 2;
				q.push(&map[spawnY][spawnX + 1]);

				// Left Node
				map[spawnY][spawnX - 1].valid = true;
				map[spawnY][spawnX - 1].x = spawnX - 1;
				map[spawnY][spawnX - 1].y = spawnY;
				map[spawnY][spawnX - 1].t = nullptr;
				map[spawnY][spawnX - 1].doors = 1;
				q.push(&map[spawnY][spawnX - 1]);

				// We have 2 new valid rooms
				numRooms -= 2;
			}
			else if (spawnRooms == 1) { // right
				// create the node for this child room, located one to the right of the spawn room
				// rooms value is default 2 because it knows it has a parent on the left
				map[spawnY][spawnX + 1].valid = true;
				map[spawnY][spawnX + 1].x = spawnX + 1;
				map[spawnY][spawnX + 1].y = spawnY;
				map[spawnY][spawnX + 1].t = nullptr;
				map[spawnY][spawnX + 1].doors = 2;
				q.push(&map[spawnY][spawnX + 1]);

				// 1 new room
				numRooms -= 1;
			}
			else if (spawnRooms == 2) { // left
				// create the node for this child room, located one to the left of the spawn room
				// rooms value is default 1 because it knows it has a parent on the right
				map[spawnY][spawnX - 1].valid = true;
				map[spawnY][spawnX - 1].x = spawnX - 1;
				map[spawnY][spawnX - 1].y = spawnY;
				map[spawnY][spawnX - 1].t = nullptr;
				map[spawnY][spawnX - 1].doors = 1;
				q.push(&map[spawnY][spawnX - 1]);

				// 1 new room
				numRooms -= 1;
			}

			// generate the spawn room based on template and record doors
			curr->doors = spawnRooms;
			curr->t = new Tilemap(getSpawn(spawnX), tiles, bgs[rand() % bgs.size()]);
			
			//std::cout << "Create spawn " << curr->t << " " << curr << std::endl;
			//std::cout << "Created spawn at " << spawnY << " " << spawnX << " " << spawnRooms << "\n" << std::endl;
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
					
						// set fields of this node we are setting 
						map[startY + yAdj][startX + xAdj].valid = true;
						map[startY + yAdj][startX + xAdj].x = startX + xAdj;
						map[startY + yAdj][startX + xAdj].y = startY + yAdj;
						map[startY + yAdj][startX + xAdj].t = nullptr;
						map[startY + yAdj][startX + xAdj].doors = from;

						//std::cout << "New Node1 " << map[startY + yAdj][startX + xAdj].t << " " << &map[startY + yAdj][startX + xAdj] << std::endl;
						
						// add this node to the queue
						q.push(&map[startY + yAdj][startX + xAdj]);
						numRooms -= 1;

						// count this door
						rooms += dir;
						//std::cout << "Added room at " << startY + yAdj << " " << startX + xAdj << "\n" << std::endl;
					}
				}
			}

			// add the doors
			curr->doors += rooms;

			//std::cout << "Generating tilemap for room at: " << curr->y << " " << curr->x << std::endl;

			//std::cout << "Before Gen: " << curr->t << " " << curr << std::endl;

			// generate a procgen room with these doors
			curr->t = new Tilemap(210, 45, curr->doors, tiles, bgs[rand() % bgs.size()]);

			//std::cout << "After Gen: " << curr->t << " " << curr << std::endl;
		}
		// update our node (since we have updated doors and t) - just in case, i was getting some weird issues that this fixed
		map[curr->y][curr->x] = *curr;
	}
}

// set the position of which room in the map the player is in
void Gamemap::setCurrentPosition(int x, int y) {
	currXPos = x;
	currYPos = y;
}

// Updates current position based on which door we go through
void Gamemap::updatePosition(int door) {
	if (door == 8) { //up
		currYPos -= 1;
	}
	else if (door == 4) { //down
		currYPos += 1;
	}
	else if (door == 2) { //left
		currXPos -= 1;
	}
	else { //right
		currXPos += 1;
	}
}

// returns a pointer to the current Tilemap that the player is in
Tilemap* Gamemap::getCurrentRoom() {
	return map[currYPos][currXPos].t;
}

// Returns "doors" 4-bit number of where there are doors in this room
int Gamemap::getRooms() {
	return map[currYPos][currXPos].doors;
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

// True if current room is spawn room
bool Gamemap::ifSpawn(){
	if (currXPos == spawnX && currYPos == spawnY) {
		return true;
	}
	else {
		return false;
	}
}

//
int Gamemap::getCurrX() {
	return this->currXPos;
}

//
int Gamemap::getCurrY() {
	return this->currYPos;
}

// returns this map type
int Gamemap::getType() {
	return this->type;
}