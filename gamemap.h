#ifndef _GAMEMAP_H_
#define _GAMEMAP_H_
#endif

#include <string>
#include <vector>
#include "tilemap.h"
#include "sprite.h"
#include "background.h"
#include "enemies.h"

/* The Gamemap class is a wrapper object that keeps track of the entire world map
* It does this by tracking and containing all of the tilemaps (rooms) that exist
* It is responsible for laying out the entire "world", and passing the appropriate metadata
* into each tilemap as they are created. It also keeps track of the current player position
* in relation to the different rooms. It is responsible for telling the game which room we are
* in and which room to generate.
*/
class Gamemap
{
public:
	// Public Constructors:
	Gamemap(int length, int height, int type, std::vector<Tile*> tiles, std::vector<Background*> bgs, std::vector<Enemy*> enemies);
	// Public Getters and Setters:	
	void setCurrentPosition(int x, int y);		// update which room the player is in with manual coordinates
	void updatePosition(int door);				// update which room the player is in relative to the door they went through
	Tilemap* getCurrentRoom();					// returns a ptr to the current room's Tilemap
	int getRooms();								// returns 4-bit doors number
	bool ifSpawn();								// are we in the spawn room
	int getCurrX();
	int getCurrY();
	int getType();
	// Public Destructor: 
	~Gamemap();									// destruction
private:
	// Private Functions:
	void generateGamemap();						// creates the Gamemap
	std::string getSpawn(int x);				// based on x location, returns string for the appropriate spawn room
	int getSpawnRooms(int x);					// based on spawn template, returns 4-bit doors number for the spawn
	int randToDir(int r);						// converts random 0-3 into 4-bit direction
	bool isValid(int x, int y, int dir);		// checks if room in direction is valid relative to x,y ropom
	int xFromDir(int dir);						// X adjustment from Dir
	int yFromDir(int dir);						// Y adjustment from Dir
	int parentDir(int dir);						// Where the room has a door based on the room that created it

	// Private Variables:
	struct Node {								// Node is a wrapper for all of our Tilemaps
		bool valid;								// Has this node been created
		int x;									// x and y location (in 2D array of this node)
		int y;
		Tilemap* t;								// Pointer to this Node/room's Tilemap
		bool power;
		int doors;								// 4-bit int encoding of where the doors are in this room
	};

	Node** map;									// 2D array of Nodes makes up the map (stores tilemaps and door info)

	std::vector<Tile*> tiles;					// tiles we have for the tilemaps
	std::vector<Background*> bgs;				// backgrounds we have for tilemaps
	std::vector<Enemy*> enemies;
	// Map dimensions
	int mapLength;								// number of rooms (x)
	int mapHeight;								// number of rooms (y)

	int type;									// Room type is for teleporters and tracking which gamemap we are currently using

												// Starting position will be determined by spawn room, and player will start there.
	int currYPos;								// current Y position of the player in the map
	int currXPos;								// current X position of the player in the map

	int spawnX;
	int spawnY;
	bool pAlready;
};
