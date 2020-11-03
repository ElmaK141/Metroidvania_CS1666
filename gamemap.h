#ifndef _GAMEMAP_H_
#define _GAMEMAP_H_
#endif

#include <string>
#include <vector>
#include "tilemap.h"
#include "sprite.h"
#include "background.h"

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
	Gamemap(int length, int height, std::vector<Tile*> tiles, std::vector<Background*> bgs);
	// Public Getters and Setters:	
	int getCurrentX();
	int getCurrentY();
	void setCurrentPosition(int x, int y); // update which room the player is in
	Tilemap* getCurrentRoom();
	// Public Destructor: 
	~Gamemap();
private:
	// Private Functions:
	void generateGamemap();		// creates the Gamemap
	std::string getSpawn(int x);
	int getSpawnRooms(int x);
	int randToDir(int r);
	bool isValid(int x, int y, int dir);
	int xFromDir(int dir);
	int yFromDir(int dir);
	int parentDir(int dir);
	// Private Variables:
	struct Node {			// Node is a wrapper for all of our Tilemaps
		bool valid;			// Has this node been created
		int x;
		int y;				// x and y location (in 2D array of this node)
		Tilemap* t;			// Pointer to this room's Tilemap
		int doors;			// 4-bit int encoding of where the doors are in this room
	};

	Node** map;				// 2D array of Nodes makes up the map
	
	std::vector<Tile*> tiles;		// tiles we have for the tilemaps
	std::vector<Background*> bgs;	// backgrounds we have for tilemaps

	int mapLength;				// number of rooms (x)
	int mapHeight;				// number of rooms (y)

	// Starting position will be determined by spawn room, and player will start there.
	int currYPos;				// current Y position of the player in the map
	int currXPos;				// current X position of the player in the map
};

