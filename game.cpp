#include <iostream>
#include <cmath>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include "game.h"
#include "gamemap.h"
#include "sprite.h"
#include "entity.h"
#include "enemies.h"
#include "tile.h"
#include "background.h"
#include "button.h"

// Double jump to fullfill requirement
bool doneSecond = false; // only allow 1 extra jump
double projectileVelocity = 25;
int projectileSize = 5;

//size of the Window/Screen and thus the size of the Camera
int SCREEN_WIDTH;
int SCREEN_HEIGHT;

// for now - define the X length of the gameworld
//constexpr int LEVEL_LEN = 3360;
double gravity = 0.03 * 3600;


//Question: This needs to be replaced with the players x and y pos.
int grappleX;
int grappleY;

//Init Stuff for delta_time
Uint32 lastTick = 0;
Uint32 curTick;
double delta_time;

//Player Animation time
Uint32 lastAnim = 0;
Uint32 curAnim = 0;
int lastAnimFrame = 3;

//Teleporter Anim
Uint32 telLast = 0;
Uint32 telAnim = 0;

//Event pointer
SDL_Event e;

//scroll offset, needs to be used in a few places
int scroll_offset_x;
int scroll_offset_y;

//Game State Tracker
//0 = main menu, 1 = game, 2 = pause menu, 3 = debug, 4 = quest menu, 5 = map
int gameState;


//Progress Indicators
int killedBoss = 0;
int gotDoubleJump = 0;
int gotGrapple = 0;
int gotHealth1 = 0;
int gotHealth2 = 0;

//Constructor
Game::Game(int width, int height)
{
	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_PNG);

	windowWidth = width;
	windowHeight = height;

	SCREEN_WIDTH = width;
	SCREEN_HEIGHT = height;

	maxHP = 40;
	playerHP = 40;

	gWindow = SDL_CreateWindow("METROIDVANIA", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		width, height, 0);


	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);

	running = true;
}

/*Game::Game(const Game &obj)
{
}*/

//Destructor
Game::~Game()
{
	running = false;
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	IMG_Quit();
	SDL_Quit();
}



//This function will serve as the main loop of the game - various game states and game logic will be called from here

void Game::gameLoop()
{
	//displayCredits();

	//Load into the start screen of the game
	loadStartScreen();

	while (running) {

		//Load main menu
		if (gameState == 0) {
			loadMainMenu();
		}
		else if (gameState == 1) {
			runGame();	//Run Game
		}
		else if (gameState == 4) {	//Player has died
			loadDeathScreen();
		}
		else if (gameState == 3) {	//Run Debug
			runDebug();
		}
	}
}

//Run the main game code
void Game::runGame() {
	//Reset the Player's Health
	playerHP = 40;

	//Camera-related variables
	scroll_offset_x = 0;
	scroll_offset_y = SCREEN_HEIGHT;
	int rem_bg_x = 0;
	int rem_tile_x = 0;
	int rem_bg_y = SCREEN_HEIGHT;
	int rem_tile_y = SCREEN_HEIGHT;
	int lthird = (SCREEN_WIDTH / 3);
	int rthird = (2 * SCREEN_WIDTH / 3);
	int tthird = (SCREEN_HEIGHT / 3);
	int bthird = (2 * SCREEN_HEIGHT / 3);

	//Define Backgrounds
	Background bg1(0, 0, 1280, 720, "assets/backgrounds/background1.png", gRenderer);
	Background bg2(0, 0, 1280, 720, "assets/backgrounds/background2.png", gRenderer);

	std::vector<Background*> backgrounds;
	backgrounds.push_back(&bg1);
	backgrounds.push_back(&bg2);

	//Tiles to add to tilemap
	Tile groundTile(0, 0, 16, 16, 1, "assets/sprites/tiles.png", gRenderer);
	Tile platformTile(16, 0, 16, 16, 1, "assets/sprites/tiles.png", gRenderer);

	// Create Tile vector for tilemap contruction and push our temporary tile
	std::vector<Tile*> tiles;
	tiles.push_back(&groundTile);
	tiles.push_back(&platformTile);
	//Did this twice so for the time beindg we dont get a out of bounds vector error, this is not an error
	//with implementation it just needs some placeholder entries for right now.. so ya 
	tiles.push_back(&groundTile);
	tiles.push_back(&platformTile);



	Physics plp(1565, 500, 1000, 250);

	std::vector<Enemy*> enemies;
	Enemy eye("data/eye.spr", 30, 30, 3, 1, &plp, gRenderer);
	Enemy eye2("data/eye.spr", 100, 40, 3, 1, &plp, gRenderer);
	Enemy eye3("data/eye.spr", 500, 600, 3, 1, &plp, gRenderer);
	Enemy eye4("data/eye.spr", 100, 400, 3, 1, &plp, gRenderer);
	Enemy eye5("data/eye.spr", 600, 10, 3, 1, &plp, gRenderer);
	//enemies.push_back(&eye);
	//enemies.push_back(&eye);
	//enemies.push_back(&eye2);
	//enemies.push_back(&eye3);
	//enemies.push_back(&eye4);
	enemies.push_back(&eye5);

	Enemy boss("data/boss.spr", 2500, 380, 3, 0, &plp, gRenderer);
	std::vector<Enemy*> bossEnemy;
	bossEnemy.push_back(&boss);
	std::vector<Enemy*> blankEnemies;

	// allMaps is a Vector of all of our Gamemaps
		// Main Room - 1x1 premade room. Type 0
		// First Section - nxn proc gen map/rooms. Type 1 - generate for no powerups
		// Second Section - nxn proc gen map/rooms. Type 2 - generate for double jump
		// Boss Room - whatever ai wants - rn 1x1 premade using mainroom. Type 3
	// allMaps lets us change which map we are in
	std::vector<Gamemap*> allMaps;
	allMaps.push_back(new Gamemap(1, 1, 0, tiles, backgrounds, blankEnemies));
	allMaps.push_back(new Gamemap(3, 3, 1, tiles, backgrounds, enemies));
	allMaps.push_back(new Gamemap(3, 3, 2, tiles, backgrounds, enemies));
	allMaps.push_back(new Gamemap(1, 1, 3, tiles, backgrounds, bossEnemy));

	// Variables for tracking our current MAP and ROOM (and tileArray)
	// map will be the pointer to our current map
	Gamemap* map = allMaps[0];
	// CurrRoom is the pointer to our current room tilemap
	Tilemap* currRoom = map->getCurrentRoom();
	int** tileArray = currRoom->getTileMap();

	// Define Physics object and Flip flag
	SDL_RendererFlip flip = SDL_FLIP_NONE;

	//Define player Position
	double x_pos = SCREEN_WIDTH / 2.0;
	double y_pos = SCREEN_HEIGHT / 2.0 - 145.0;

	// Vector to track any powerups in the scene
	std::vector<Entity*> powerups;

	// Vector to track if there are any teleporters in the scene
	std::vector<Entity*> tps;

	// order used for different teleporters
	int order = 0;

	// temp
	int pu = 0;

	//Find player spawn point, and any teleporters in this room
	for (int i = 0; i < currRoom->getMaxHeight(); i++)
	{
		for (int j = 0; j < currRoom->getMaxWidth(); j++)
		{
			if (tileArray[i][j] == 3)
			{
				//std::cout << " Found spawn " << j << " " << i << std::endl;
				x_pos = j * 16.0;
				y_pos = i * 16.0;
			}
			else if (tileArray[i][j] == 9) { // 9 marks that there is a teleporter
				// Entity flag in the teleporters determine where they send you
					// 2 tp to main room, 3 tp to Sec1, 4 tp to Sec2, 5 tp to boss
				// teleporters decided by how high they are. with boss at highest, then 2nd Section, then 1st section at floor
					// this only matters / is the case for the main room

				// idea (when ready) is to no longer spawn TP to Sec1/2 after the player returns from that area with the powerup
				// as well as order, the player would need to have hasDouble and hasGrapple set to false for them to spawn
				if (order == 0) { // (to boss)
					//std::cout << "spawn tp 5" << std::endl;
					tps.push_back(new Entity("data/teleporter.spr", j * 16.0, i * 16.0, 3, 5, &plp, gRenderer));
				}
				else if (order == 1) { // (to Sec2)
					//std::cout << "spawn tp 4" << std::endl;
					tps.push_back(new Entity("data/teleporter.spr", j * 16.0, i * 16.0, 3, 4, &plp, gRenderer));
				}
				else { // (to Sec1)
					//std::cout << "spawn tp 3" << std::endl;
					tps.push_back(new Entity("data/teleporter.spr", j * 16.0, i * 16.0, 3, 3, &plp, gRenderer));
				}
				order++;
			}
		}
	}

	// Define player entity
	Entity player("data/player.spr", x_pos, y_pos, 3, 0, &plp, gRenderer);		//0 is flag for player entity
	player.setGrapple();
	gotGrapple = 1;
	int hitTick = 0;
	bool hit = false;
	int eyeSpawnCD = 0;

	//"Load" in the game by pausing to avoid buffering in the gappling hook input
	SDL_Delay(150);

	std::vector<Enemy*> ce;

	//Run the Game
	while (running && gameState == 1) {
		//Delta time calculation
		curTick = SDL_GetTicks();
		delta_time = (curTick - lastTick) / 1000.0;
		lastTick = curTick;


		//Player Anim frame tracker
		curAnim = SDL_GetTicks();

		//Teleporter animation frame tracker
		telAnim = SDL_GetTicks();

		//Quit game
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) {
				running = false;
			}
		}

		//Get Input
		// in order to change maps when interacting with an entity, getUserInput will return info on which map to change to (-1 if none)
		int changeMap = getUserInput(&player, tps);

		// need to update our map/room and spawning location after teleporting
		// if changeMap is not -1, then we interacted with a teleporter
		if (changeMap != -1) {

			//std::cout << "Changing map to " << changeMap - 2 << std::endl;

			// based on the tp flag (changeMap) we will switch to a different Map
				// type 2 tp returns us to main room - allMaps[0]
				// type 3 tp sends us to first section - allMaps[1]
				// type 4 tp sends us to second section - allMaps[2]
				// type 5 tp sends us to the boss - allMaps[3]
			// tp flag is 2 over index in allMaps
			int newMap = changeMap - 2;

			// update the map, current room, tilearray
			map = allMaps[newMap];
			currRoom = map->getCurrentRoom();
			ce = currRoom->getEnemies();
			tileArray = currRoom->getTileMap();

			// now with updated map, clear our teleporters/powerups
			tps.clear();
			powerups.clear();
			//std::cout << "Clear all tp" << std::endl;

			int order = 0;

			//Now we must find the player spawn point for this need room/map.
			for (int i = 0; i < currRoom->getMaxHeight(); i++)
			{
				for (int j = 0; j < currRoom->getMaxWidth(); j++) //spawn
				{
					if (tileArray[i][j] == 3)
					{
						x_pos = j * 16.0;
						y_pos = i * 16.0;
						player.setPosition(x_pos, y_pos);
					}
					if (tileArray[i][j] == 9) { //tps
						// When we teleport, we are either in main room or a spawn room
						if (map->getType() == 0) {
							// teleporters decided by how high they are. with boss at highest, then 2nd Section, then 1st section at floor

							// idea (when ready) is to no longer spawn TP to Sec1/2 after the player returns from that area with the powerup
							// as well as order, the player would need to have hasDouble and hasGrapple set to false for them to spawn

							if (order == 0) { // (to boss)
								//std::cout << "spawn tp 5" << std::endl;
								tps.push_back(new Entity("data/teleporter.spr", j * 16.0, i * 16.0, 3, 5, &plp, gRenderer));
							}
							else if (order == 1) { // (to Sec2)
								//std::cout << "spawn tp 4" << std::endl;
								tps.push_back(new Entity("data/teleporter.spr", j * 16.0, i * 16.0, 3, 4, &plp, gRenderer));
							}
							else { // (to Sec1)
								//std::cout << "spawn tp 3" << std::endl;
								tps.push_back(new Entity("data/teleporter.spr", j * 16.0, i * 16.0, 3, 3, &plp, gRenderer));
							}
							order++;
						}
						else if (map->getType() == 1 || map->getType() == 2) {
							//std::cout << "spawn tp 2" << std::endl;
							tps.push_back(new Entity("data/teleporter.spr", j * 16.0, i * 16.0, 3, 2, &plp, gRenderer));
						}
					}
					else if (tileArray[i][j] == 8) { //TESTING - POWERUPS SHOULD BE REMOVED FROM SPAWNS ONCE THEY ARE APPEARING IN THE WORLD

						// THIS CODE LOADS ENTITIES WHEN WE TAKE A TELEPORTER AND CHANGE MAPS
						// ALL OF THIS REGARDING POWERUPS WILL BE GONE, SINCE THEY WILL ONLY BE SPAWNED BY GOING THROUGH A DOOR

						if (map->getType() == 1) { // if Sec1, spawn DOUBLE JUMP

							// health or upgrade?

							powerups.push_back(new Entity("data/powerups.spr", j * 16.0, (i * 16.0) + 4, 3, 6, &plp, gRenderer));
							powerups.back()->setCurrFrame(0);
						}
						else { // if Sec2, spawn Grapple

							// health or upgrade?

							powerups.push_back(new Entity("data/powerups.spr", j * 16.0, (i * 16.0) + 4, 3, 7, &plp, gRenderer));
							powerups.back()->setCurrFrame(1);
						}
					}
				}
			}

			//end this call by skipping this loop?
			continue;
		}

		if (!player.getShot())
		{/*If a shot has been fired EMIL*/
			double xNorm = player.getPVelX() / projectileVelocity;
			double yNorm = player.getPVelY() / projectileVelocity;

			for (int j = 0; j < projectileVelocity; j++)
			{/*proj velocity is the hyp length so we go for each pixel on it*/
				double deltaX = scroll_offset_x + player.getPX() + xNorm * j;
				double deltaY = scroll_offset_y + player.getPY() + yNorm * j;


				if (deltaX > 0 && deltaX < SCREEN_WIDTH + scroll_offset_x && deltaY>0 && deltaY < SCREEN_HEIGHT + scroll_offset_y)
				{/*inside screen*/
					int tileMapX = (int)(deltaX / 16);
					int tileMapY = (int)(deltaY / 16);


					bool checkFlag0 = tileArray[tileMapY][tileMapX] == 0; // Air
					bool checkFlag3 = tileArray[tileMapY][tileMapX] == 3; // Spawn
					bool checkFlag8 = tileArray[tileMapY][tileMapX] == 8; // PowerSpawns
					bool checkFlag9 = tileArray[tileMapY][tileMapX] == 9; // Teleport

					if (!(checkFlag0 || checkFlag3 || checkFlag8 || checkFlag9))
					{ /*Collision with something not air*/
						player.setShot(true);
						break;
					}
					else
					{
						for (int i = 0; i < ce.size(); i++)
						{
							if (ce[i]->getHP() > 0 && checkHitEnemy(deltaX, deltaY, ce[i]))
							{
								int randomNumber = rand() % 100;
								int ignoreChance = 10;

								if (randomNumber > ignoreChance)
								{
									ce[i]->takeDamage(player.getPVelX() / projectileVelocity, player.getPVelY() / projectileVelocity);
									player.setShot(true);
									break;
								}
								else {
									//Add Guard animation
								}
							}
						}
					}
				}
				else
				{
					player.setShot(true);
					break;
				}
			}
			if (!player.getShot())
			{
				player.setPX(player.getPX() + player.getPVelX());
				player.setPY(player.getPY() + player.getPVelY());
			}
		}


		//Handle in-air and on-ground collision for current room
		handleCollision(&player, currRoom);

		if (map->getType() == 3)
		{
			if (eyeSpawnCD <= 0)
			{
				ce.push_back(new Enemy("data/eye.spr", 2520, 400, 3, 1, &plp, gRenderer));
				ce.push_back(new Enemy("data/boss.spr", 2500, 380, 3, 0, &plp, gRenderer));
				ce[ce.size() - 1]->setXVel(-8);
				ce[ce.size() - 1]->setYVel(-10);

				if (boss.getHP() > 100)
					eyeSpawnCD = 300;
				else
					eyeSpawnCD = 100;
			}
			else
				eyeSpawnCD -= delta_time;
		}
		// powerups - check if player collides
		for (auto&& p : powerups) {
			if (checkPlayerCollision(&player, p) && p->getFrameIndex() != 3) { // if the player collides with the power up, enable ability, disable powerup
				if (p->getFlag() == 6) { // Double Jump
					player.setDouble();
					gotDoubleJump = 1;
				}
				else if (p->getFlag() == 7) { // Grapple
					player.setGrapple();
					gotGrapple = 1;
				}
				else { // Health Up
					player.increaseHealth();
					maxHP += 10;
					playerHP = maxHP;

					if (gotHealth1 == 0)
						gotHealth1 = 1;
					else
						gotHealth2 = 1;
				}

				if (p->getFrameIndex() != 3) { //move down sprite so that when disabled it is in correct place
					p->setPosition(p->getXPosition(), p->getYPosition() + 52);
				}
				p->setCurrFrame(3); //set sprite to disabled
			}
		}

		// enemies
		if (!map->ifSpawn() || map->getType() == 3) {
			for (int i = 0; i < ce.size(); i++) //handle enemies; update, check for hits, give player iframes if hit
			{
				if (ce[i]->getFlag() == 0 && map->getType() != 3) continue;
				if (ce[i]->getHP() <= 0) continue;
				ce[i]->update(tileArray, delta_time, player.getXPosition(), player.getYPosition());

				if (!hit)
				{
					hit = checkHitPlayer(&player, ce[i]);
					if (hit)
						hitTick = SDL_GetTicks();
				}
				else if (SDL_GetTicks() - hitTick > 1000)
					hit = false;
			}
		}

		// Update scroll if Player moves outside of middle third in x direction
		if (player.getXPosition() > (scroll_offset_x + rthird))
			scroll_offset_x = player.getXPosition() - rthird;
		else if (player.getXPosition() < (scroll_offset_x + lthird))
			scroll_offset_x = player.getXPosition() - lthird;

		// Update scroll if Player moves outside middle third in y direction
		if (player.getYPosition() < (scroll_offset_y + tthird))
			scroll_offset_y = player.getYPosition() - tthird;
		else if (player.getYPosition() > (scroll_offset_y + bthird))
			scroll_offset_y = player.getYPosition() - bthird;

		//Prevent scroll_offset_x from placing Camera outside of gameworld in x direction
		//std::cout << currRoom->getMaxWidth()*16 << std::endl;
		if (scroll_offset_x < 0)
			scroll_offset_x = 0;
		if (scroll_offset_x + SCREEN_WIDTH > currRoom->getMaxWidth() * 16)
			scroll_offset_x = currRoom->getMaxWidth() * 16 - SCREEN_WIDTH;

		//Prevent scroll_offset_y from placing Camera outside of gameworld in y direction
		if (scroll_offset_y < 0)
			scroll_offset_y = 0;
		if (scroll_offset_y + SCREEN_HEIGHT > currRoom->getMaxHeight() * 16)
			scroll_offset_y = currRoom->getMaxHeight() * 16 - SCREEN_HEIGHT;

		// Check if we are going through a door
		int newRoom = checkDoor(map->getRooms(), player, currRoom);

		// if we are, update our room and position
		if (newRoom != -1) {
			// If this is true, then we must update our tilemap, and map position
			// as well as reset details for room rendering

			// updates our position in map to the new room, update our Tilemap pointer and tileArray
			map->updatePosition(newRoom);
			currRoom = map->getCurrentRoom();
			ce = currRoom->getEnemies();
			tileArray = currRoom->getTileMap();

			// clear our tps/powerups if we had any
			tps.clear();
			powerups.clear();

			// For updated room, scan for any teleporters that must be spawned
			for (int i = 0; i < currRoom->getMaxHeight(); i++)
			{
				for (int j = 0; j < currRoom->getMaxWidth(); j++)
				{
					if (tileArray[i][j] == 9) {
						// when we change rooms this way, we are in a procgen section. TP returns us to main room (2)
						tps.push_back(new Entity("data/teleporter.spr", j * 16.0, i * 16.0, 3, 2, &plp, gRenderer));
					}
					else if (tileArray[i][j] == 8) { // if there is a powerup in this room, spawn it
						// 6 (DJ), 7 (G), 8 (H)
						// flag determines which type they are what they will do

						// when we go through a door, we can come into powerup room (which is powerup based on map)
						// if there is no tp in the room, it is a health up, and that gets set after this double for
						if (map->getType() == 1) { // if Sec1, spawn DOUBLE JUMP
							powerups.push_back(new Entity("data/powerups.spr", j * 16.0, (i * 16.0) + 4, 3, 6, &plp, gRenderer));
							powerups.back()->setCurrFrame(0);
						}
						else { // if Sec2, spawn Grapple
							powerups.push_back(new Entity("data/powerups.spr", j * 16.0, (i * 16.0) + 4, 3, 7, &plp, gRenderer));
							powerups.back()->setCurrFrame(1);
						}
					}
				}
			}

			// if this room has a powerup but no teleporters, it is a health powerup
			if (tps.empty() && !powerups.empty()) {
				powerups[0]->setFlag(8);
				powerups[0]->setCurrFrame(2);
			}

			if (newRoom == 8) { //moving up
				scroll_offset_y = currRoom->getMaxHeight() * 16 - SCREEN_HEIGHT;
				player.setPosition(player.getXPosition(), currRoom->getMaxHeight() * 16 - player.getCurrFrame().getHeight());
			}
			else if (newRoom == 4) { //moving down
				scroll_offset_y = 0;
				player.setPosition(player.getXPosition(), 0);
			}
			else if (newRoom == 2) { //moving left
				scroll_offset_x = currRoom->getMaxWidth() * 16 - SCREEN_WIDTH;
				player.setPosition(currRoom->getMaxWidth() * 16 - player.getCurrFrame().getWidth(), player.getYPosition());
			}
			else { //moving right
				scroll_offset_x = 0;
				player.setPosition(0, player.getYPosition());
			}
		}

		//Draw to screen
		SDL_RenderClear(gRenderer);
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);


		// Draw the portion of the background currently inside the camera view
		rem_bg_x = scroll_offset_x % SCREEN_WIDTH;
		rem_tile_x = scroll_offset_x % (currRoom->getMaxWidth() * 16);
		rem_bg_y = scroll_offset_y % SCREEN_HEIGHT;
		rem_tile_y = scroll_offset_y % (currRoom->getMaxHeight() * 16);

		currRoom->getBackground()->getSprite()->draw(gRenderer, -rem_bg_x, -rem_bg_y);
		currRoom->getBackground()->getSprite()->draw(gRenderer, (-rem_bg_x + SCREEN_WIDTH), (-rem_bg_y));
		currRoom->getBackground()->getSprite()->draw(gRenderer, -rem_bg_x, -rem_bg_y + SCREEN_HEIGHT);
		currRoom->getBackground()->getSprite()->draw(gRenderer, -rem_bg_x + SCREEN_WIDTH, -rem_bg_y + SCREEN_HEIGHT);
		currRoom->drawTilemap(gRenderer, rem_tile_x, rem_tile_y);

		// if there are any teleporters in this scene, draw them
		for (auto&& s : tps) {
			if (telAnim - telLast >= 300.0) {
				if (s->getFrameIndex() == 0) {
					s->setCurrFrame(1);
				}
				else {
					s->setCurrFrame(0);
				}
			}
			s->getCurrFrame().draw(gRenderer, s->getXPosition() - scroll_offset_x, s->getYPosition() - scroll_offset_y + 4);
		}
		if (telAnim - telLast >= 300.0) {
			telLast = telAnim;
		}

		// if there are any powerups in this room, draw them
		for (auto&& p : powerups) {
			p->getCurrFrame().draw(gRenderer, p->getXPosition() - scroll_offset_x, p->getYPosition() - scroll_offset_y);
		}

		// draw the player
		if (player.getXVel() > 0 && flip == SDL_FLIP_HORIZONTAL)
			flip = SDL_FLIP_NONE;
		else if (player.getXVel() < 0 && flip == SDL_FLIP_NONE)
			flip = SDL_FLIP_HORIZONTAL;

		//hit iframes indicator; blink a tenth of a second every tenth of a second
		if (player.getFrameIndex() == 0 && (SDL_GetTicks() % 100 > 50 || !hit))
			player.getCurrFrame().draw(gRenderer, player.getXPosition() - scroll_offset_x, player.getYPosition() - scroll_offset_y);
		else if (SDL_GetTicks() % 100 > 50 || !hit)
			player.getCurrFrame().draw(gRenderer, player.getXPosition() - scroll_offset_x, player.getYPosition() - scroll_offset_y, flip);

		// ENEMY INFORMATION WILL HAVE TO BE MOVED INTO TILEMAP, SO THAT THE CURRENT ROOM CAN SPECIFY WHICH ENEMIES NEED TO BE SPAWNED, HOW MANY, AND WHERE. THEN THE GAME LOOP HERE
		// CAN CHECK FOR COLLISION AGAINST THE LIST OF CURRENT ENEMIES
		// I WONDER IF WE SHOULD JUST HAVE A SCENE ENTITY LIST AND WE CAN JUST PERFORM COLLISION AGAINST ALL ENTITIES IN THE SCENE

		// Draw Enemies
		if (!map->ifSpawn() || map->getType() == 3) {
			for (int i = 0; i < ce.size(); i++)
			{
				if (ce[i]->getHP() > 0) //only draw live enemies
					ce[i]->getCurrFrame().draw(gRenderer, ce[i]->getXPosition() - scroll_offset_x, ce[i]->getYPosition() - scroll_offset_y);
				if (ce[i]->getFlag() == 0 && map->getType() == 3)
				{
					if (ce[i]->getHP() > 0)
						drawBossHP(ce[i]->getHP());
					else
						displayCredits();
				}
			}
		}

		//Draw the player's hp
		drawHP();

		//Player is Dead
		if (playerHP <= 0) {
			gameState = 4;
			break;
		}

		// Draw box
		if (!player.getShot())
		{
			SDL_Rect fillRect = { player.getPX(), player.getPY(), projectileSize, projectileSize };
			SDL_RenderFillRect(gRenderer, &fillRect);
		}

		SDL_RenderPresent(gRenderer);
	}
}

//Handle the user input
int Game::getUserInput(Entity* player, std::vector<Entity*> tps) {
	//User input
	const Uint8* keystate = SDL_GetKeyboardState(nullptr);

	//Mouse Coordinate Variables
	int mouseXinWorld = 0, mouseYinWorld = 0;

	//Pause game and bring up Pause Menu (with esc)
	if (e.type == SDL_KEYDOWN) { //detect escape keydown
		switch (e.key.keysym.sym) {

		case SDLK_ESCAPE:
			if (!player->getPhysics()->inAir())
				pauseMenu(gameState);
			break;
		}
	}


	if (keystate[SDL_SCANCODE_J]) {
		//std::cout << "J PRESSED" << std::endl;
		if (!player->getPhysics()->inAir())
			questMenu();
	}


	if (keystate[SDL_SCANCODE_E]) {
		//std::cout << "E PRESSED" << std::endl;
		for (auto&& tp : tps) { // for each teleporter in the scene
			if (checkPlayerCollision(player, tp)) { // check if the player is touching it
				//std::cout << "Interacted with TP type " << tp->getFlag() << std::endl;

				// player has pressed E, and they are touching a teleporter. We will now return the flag from this teleporter, so we know which MAP to teleport to
				// idea is to return here because we will reset the game loop.
				return tp->getFlag();
			}
		}
	}

	if (SDL_BUTTON(SDL_BUTTON_RIGHT) & SDL_GetMouseState(&mouseXinWorld, &mouseYinWorld)) {
		mouseXinWorld += scroll_offset_x;
		mouseYinWorld += scroll_offset_y;

		if (!player->getPhysics()->isGrappling() && player->getGrapple()) {
			//std::cout << "pressed" << std::endl;
			grappleX = mouseXinWorld;
			grappleY = mouseYinWorld;

			player->getPhysics()->setGrappleState(true);
		}
	}
	else {

		if (player->getPhysics()->isGrappling()) {
			//std::cout << "unpressed" << std::endl;
			player->getPhysics()->setGrappleState(false);
		}
	}

	/*Shooting*/
	if (SDL_BUTTON(SDL_BUTTON_LEFT) & SDL_GetMouseState(&mouseXinWorld, &mouseYinWorld) && player->getShot()) {
		mouseXinWorld += scroll_offset_x;
		mouseYinWorld += scroll_offset_y;

		//Set projectile to start at player
		double playerCenterX = player->getXPosition() + (player->getCurrFrame().getWidth() / 2);
		double playerCenterY = player->getYPosition() + (player->getCurrFrame().getHeight() / 2);


		double x_vector = mouseXinWorld - playerCenterX;
		double y_vector = mouseYinWorld - playerCenterY;

		double playerNetVel = sqrt((x_vector * x_vector) + (y_vector * y_vector));
		double directionXVelNorm = x_vector / playerNetVel;
		double directionYVelNorm = y_vector / playerNetVel; //direction of shot, opposite of recoil

		/*Shoot*/
		player->setShot(false); //can only shoot one at a time. for multple maybe change to an array of n size for n shots?


		player->setPX(playerCenterX - scroll_offset_x);
		player->setPY(playerCenterY - scroll_offset_y);

		//Set the velocities
		player->setPVelX(projectileVelocity * directionXVelNorm);
		player->setPVelY(projectileVelocity * directionYVelNorm);

		/*Apply recoil*/
		double recoilForce = 200;
		player->setXVel(player->getXVel() - (recoilForce * directionXVelNorm));
		player->setYVel(player->getYVel() - (recoilForce * directionYVelNorm));
	}

	//Redone player physics
	if (player->getPhysics()->inAir()) {
		//Air control
		if (keystate[SDL_SCANCODE_SPACE]) {
			if (player->getJump() && player->getDouble() && !doneSecond)
			{ /* Double jump */
				doneSecond = true;

				player->setJump(false);
				player->setYVel(-player->getPhysics()->getJumpStrength());
			}
		}

		//Apply Drag
		if (!player->getPhysics()->isGrappling()) {
			//Own rules while grappling, do this otherwise
			double playerNetVel = sqrt((player->getXVel() * player->getXVel()) + (player->getYVel() * player->getYVel()));
			double playerXVelNorm = player->getXVel() / playerNetVel;
			double playerYVelNorm = player->getYVel() / playerNetVel;
			double dragForce = fmin((1 * playerNetVel) * player->getPhysics()->getAirDrag(), playerNetVel);

			player->setXVel(player->getXVel() - (playerXVelNorm * dragForce));
			player->setYVel(player->getYVel() - (playerYVelNorm * dragForce));
		}
	}
	else {
		//Ground control

		if (keystate[SDL_SCANCODE_SPACE]) {
			if (player->getJump()) //jump from ground
			{
				player->getPhysics()->setAirState(true);
				doneSecond = false; //jumping from ground resets the double

				player->setJump(false);
				player->setYVel(player->getYVel() - player->getPhysics()->getJumpStrength());
			}
		}

		//Animation
		if (keystate[SDL_SCANCODE_A]) {
			if (curAnim - lastAnim >= 200.0) {
				lastAnim = curAnim;
				if (player->getFrameIndex() == 1 || player->getFrameIndex() == 0) {
					if (lastAnimFrame == 2) {
						player->setCurrFrame(3);
						lastAnimFrame = 3;
					}
					else {
						player->setCurrFrame(2);
						//player->setPosition(player->getXPosition() - 3, player->getYPosition());
						lastAnimFrame = 2;
					}
				}
				else {
					player->setCurrFrame(1);
				}
			}
		}

		if (keystate[SDL_SCANCODE_S]) {
			//Animation
			if (player->getXVel() == 0) {
				if (player->getFrameIndex() != 0) {
					player->setPosition(player->getXPosition() - 39, player->getYPosition());
				}

				player->setCurrFrame(0);
			}

		}

		//Animation
		if (keystate[SDL_SCANCODE_D]) {
			if (curAnim - lastAnim >= 200.0 && !player->getPhysics()->inAir()) {
				lastAnim = curAnim;
				if (player->getFrameIndex() == 1 || player->getFrameIndex() == 0) {
					if (lastAnimFrame == 2) {
						player->setCurrFrame(3);
						lastAnimFrame = 3;
					}
					else {
						player->setCurrFrame(2);
						lastAnimFrame = 2;

						//Adjust for shift in width
						player->setPosition(player->getXPosition() - 9, player->getYPosition());
					}
				}
				else {
					player->setCurrFrame(1);
				}
			}
		}

		//Not holding side buttons (Move outside else if we want 'immediate' stopping for not pressing. using drag)
		if (!(keystate[SDL_SCANCODE_A] || keystate[SDL_SCANCODE_D])) {
			//Animation
			if (player->getFrameIndex() != 0)
				player->setCurrFrame(1);

			//Movement apply stopping force
			if (player->getXVel() < 0) {
				player->setXVel(fmin(0, player->getXVel() + player->getPhysics()->getAcceleration()));
			}
			else if (player->getXVel() > 0) {
				player->setXVel(fmax(0, player->getXVel() - player->getPhysics()->getAcceleration()));
			}
		}
	}


	// simplifying code. both were identical for air or ground so moved out
	if (!keystate[SDL_SCANCODE_SPACE]) { //only jump if you've landed and pressed space again
		player->setJump(true);
	}

	/*making a and d button else if instead of if takes away the ability to move while grappling. fixes zooming but no additional control while doing it*/
	if (player->getPhysics()->isGrappling()) {
		double playerCenterX = player->getXPosition() + (player->getCurrFrame().getWidth() / 2);
		double playerCenterY = player->getYPosition() + (player->getCurrFrame().getHeight() / 2);


		double xComp = (grappleX - playerCenterX) / sqrt((grappleX - playerCenterX) * (grappleX - playerCenterX) + (grappleY - playerCenterY) * (grappleY - playerCenterY));
		double yComp = (grappleY - playerCenterY) / sqrt((grappleX - playerCenterX) * (grappleX - playerCenterX) + (grappleY - playerCenterY) * (grappleY - playerCenterY));

		//Apply grapple force
		int errorSpace = 50;
		int closeSpace = 5;
		if (std::abs(grappleX - playerCenterX) > errorSpace || std::abs(grappleY - playerCenterY) > errorSpace)
		{
			player->setXVel((player->getXVel() + xComp * player->getPhysics()->getGrappleStr()) * player->getPhysics()->getDampen());
			player->setYVel((player->getYVel() + yComp * player->getPhysics()->getGrappleStr()) * player->getPhysics()->getDampen());
		}
		else if (std::abs(grappleX - playerCenterX) < closeSpace || std::abs(grappleY - playerCenterY) < closeSpace)
		{
			player->setXVel(0);
			player->setYVel(0);
		}
		else
		{
			player->setXVel((player->getXVel() + xComp * player->getPhysics()->getGrappleStr())* player->getPhysics()->getDampen()*0.50);
			player->setYVel((player->getYVel() + yComp * player->getPhysics()->getGrappleStr())* player->getPhysics()->getDampen()*0.50);
		}

	}
	//movement
	else if (keystate[SDL_SCANCODE_A]) {
		//Movement
		if (player->getXVel() > -player->getPhysics()->getMaxX()) { //as long as we don't exceed max speed, change velocity
			player->setXVel(fmin(player->getXVel() - player->getPhysics()->getAcceleration(), -player->getPhysics()->getMaxX()));
		}
	}

	//movement
	else if (keystate[SDL_SCANCODE_D]) {
		//Movement
		if (player->getXVel() < player->getPhysics()->getMaxX()) { //as long as we don't exceed max speed, change velocity
			player->setXVel(fmax(player->getXVel() + player->getPhysics()->getAcceleration(), player->getPhysics()->getMaxX()));
		}
	}

	// return -1 if we just handle input normally
	return -1;
}
//Handle the Collision
void Game::handleCollision(Entity* player, Tilemap* t) {
	bool on_solid = detectCollision(*player, t->getTileMap(), player->getXVel() * delta_time, player->getYVel() * delta_time, t->getMaxHeight());
	if (!on_solid) // while in air
	{
		if (player->getPhysics()->getMaxY() > player->getYVel())
			player->setYVel(player->getYVel() + gravity);
		player->getPhysics()->setAirState(true);
		//player.setCurrFrame(1);
	}
	else if (on_solid)
	{
		player->getPhysics()->setAirState(false);
	}
	if (!player->getPhysics()->inAir())
	{
		//player.setCurrFrame(0);
		player->setYVel(0.0);
	}
}

void Game::loadDeathScreen() {
	Sprite deadMsg(0, 0, 1280, 720, 1, "assets/kennedys.png", gRenderer);

	SDL_RenderClear(gRenderer);
	SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0);
	deadMsg.draw(gRenderer, 0, 0);
	SDL_RenderPresent(gRenderer);
	SDL_Delay(1000);

	while (running) {
		SDL_PollEvent(&e);

		if (e.type == SDL_QUIT) {
			running = false;
			return;
		}
		else if (e.type == SDL_KEYDOWN) {
			gameState = 0;
			return;
		}

		SDL_RenderClear(gRenderer);
		SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0);
		deadMsg.draw(gRenderer, 0, 0);
		SDL_RenderPresent(gRenderer);
	}
}

//Load the start screen/main menu of the game
void Game::loadStartScreen() {
	//Declare Start Screen Sprites
	Sprite mmTitle(0, 0, 796, 125, 1, "assets/main_menu/mainmenuTitle.png", gRenderer);
	Sprite mmPressAny(0, 0, 485, 56, 1, "assets/main_menu/pressAny.png", gRenderer);

	//Start Screen loop
	while (running) {
		//Check for button input to go to main menu
		SDL_PollEvent(&e);
		if (e.type == SDL_QUIT) {
			running = false;
			return;
		}
		else if (e.type == SDL_KEYDOWN)
			break;

		SDL_RenderClear(gRenderer);
		SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0x00);	//black background

		//Draw sprites
		mmTitle.draw(gRenderer, 240, 120);
		if (SDL_GetTicks() % 1200 <= 700)	//blinking text
			mmPressAny.draw(gRenderer, 397, 400);

		SDL_RenderPresent(gRenderer);
	}

	//Set game state to 0 (Main Menu)
	gameState = 0;
}

void Game::loadMainMenu() {
	//Mouse Coordinate Variables
	int mouseX = 0, mouseY = 0;

	//Unlock debug Option
	bool unlockDebug = false;

	//Main Menu Buttons/Sprites
	Sprite mmTitle(0, 0, 796, 125, 1, "assets/main_menu/mainmenuTitle.png", gRenderer);
	Button startGame(0, 0, 452, 68, 1, "assets/main_menu/startGame.png", gRenderer);
	Button debug(0, 0, 294, 68, 1, "assets/main_menu/debug.png", gRenderer);

	killedBoss = 0;
	gotDoubleJump = 0;
	gotGrapple = 0;
	gotHealth1 = 0;
	gotHealth2 = 0;

	//Main Menu Loop
	while (running) {

		SDL_PollEvent(&e);
		if (e.type == SDL_QUIT) {
			running = false;
			return;
		}
		else if (e.type == SDL_MOUSEBUTTONDOWN) {
			if (e.button.button == SDL_BUTTON_LEFT) {
				//Get mouse positon
				mouseX = e.button.x;
				mouseY = e.button.y;

				//Start Game
				if ((mouseX > startGame.getSprite()->getX()) && (mouseX < startGame.getSprite()->getX() + startGame.getSprite()->getWidth()) && (mouseY > startGame.getSprite()->getY()) && (mouseY < startGame.getSprite()->getY() + startGame.getSprite()->getHeight())) {
					//Set gameState to start into normal game
					gameState = 1;
					break;
				}
				//Debug
				else if (unlockDebug && (mouseX > debug.getSprite()->getX()) && (mouseX < debug.getSprite()->getX() + debug.getSprite()->getWidth()) && (mouseY > debug.getSprite()->getY()) && (mouseY < debug.getSprite()->getY() + debug.getSprite()->getHeight())) {
					//Set gameState to start into debug
					gameState = 3;
					break;
				}
			}

		}
		else if (e.type == SDL_KEYDOWN) {
			switch (e.key.keysym.sym) {

			case SDLK_d:
				//Unlock the debug menu button
				unlockDebug = true;
				break;
			}
		}

		SDL_RenderClear(gRenderer);
		SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0x00);	//black background

		//Draw to screen
		mmTitle.draw(gRenderer, 240, 120);
		startGame.getSprite()->draw(gRenderer, 397, 400);

		if (unlockDebug)
			debug.getSprite()->draw(gRenderer, 397, 500);

		SDL_RenderPresent(gRenderer);
	}
}

//Pauses Main game loop and brings up Pause menu
void Game::pauseMenu(int prevGameState)
{
	//Mouse Coordinate Variables
	int mouseX = 0, mouseY = 0;

	// gamestate is paused (2)
	gameState = 2;

	//Pause menu sprites
	//Sprite border(0, 0, );
	Sprite pauseLabel(0, 0, 253, 75, 1, "assets/pause_menu/paused.png", gRenderer);
	Button resumeGame(0, 0, 220, 70, 1, "assets/pause_menu/resume.png", gRenderer);
	Button mainMenu(0, 0, 303, 70, 1, "assets/pause_menu/mainMenu.png", gRenderer);

	SDL_Rect pauseBox = { SCREEN_WIDTH / 2 - 163, 190, 325, 300 };

	//Pause menu loop
	while (running) {

		//Poll to see if we close the game at any time
		SDL_PollEvent(&e);
		if (e.type == SDL_QUIT) {
			running = false;
			return;
		}
		else if (e.type == SDL_MOUSEBUTTONDOWN) {
			if (e.button.button == SDL_BUTTON_LEFT) { //if they click on a button
				//Get mouse positon
				mouseX = e.button.x;
				mouseY = e.button.y;

				//Resume Game
				if ((mouseX > resumeGame.getSprite()->getX()) && (mouseX < resumeGame.getSprite()->getX() + resumeGame.getSprite()->getWidth()) && (mouseY > resumeGame.getSprite()->getY()) && (mouseY < resumeGame.getSprite()->getY() + resumeGame.getSprite()->getHeight())) {
					if (prevGameState == 1)
						gameState = 1;
					else
						gameState = 3;
					break;
				}
				else if ((mouseX > mainMenu.getSprite()->getX()) && (mouseX < mainMenu.getSprite()->getX() + mainMenu.getSprite()->getWidth()) && (mouseY > mainMenu.getSprite()->getY()) && (mouseY < mainMenu.getSprite()->getY() + mainMenu.getSprite()->getHeight())) {
					SDL_Delay(250);
					gameState = 0;
					break;
				}
			}

		}

		//SDL_RenderClear(gRenderer);
		SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0x00);	//black background
		SDL_RenderFillRect(gRenderer, &pauseBox);

		//Draw to screen
		pauseLabel.draw(gRenderer, SCREEN_WIDTH / 2 - 126, 200);
		resumeGame.getSprite()->draw(gRenderer, SCREEN_WIDTH / 2 - 110, 300);
		mainMenu.getSprite()->draw(gRenderer, SCREEN_WIDTH / 2 - 152, 400);

		SDL_RenderPresent(gRenderer);
	}

}

//Pauses Main game loop and brings up Pause menu
void Game::questMenu()
{
	//Mouse Coordinate Variables
	int mouseX = 0, mouseY = 0;

	// gamestate is in quest menu (4)
	gameState = 4;

	//Quest menu sprites
	Sprite questLabel(0, 0, 256, 75, 1, "assets/quest_menu/quests.png", gRenderer);
	Sprite questList(0, 0, 352, 256, 1, "assets/quest_menu/questList.png", gRenderer);
	Button resumeGame(0, 0, 220, 70, 1, "assets/quest_menu/resume.png", gRenderer);

	SDL_Rect questBox = { SCREEN_WIDTH / 2 - 200, 190, 400, 500 };

	//Check off objectives
	SDL_Rect doneKaren = { SCREEN_WIDTH / 2 - 131, 355, 160, 2 };
	SDL_Rect doneDoubleJump = { SCREEN_WIDTH / 2 - 120, 398, 235, 2 };
	SDL_Rect doneGrapple = { SCREEN_WIDTH / 2 - 120, 429, 266, 2 };
	SDL_Rect doneHealth1 = { SCREEN_WIDTH / 2 - 133, 510, 304, 2 };
	SDL_Rect doneHealth2 = { SCREEN_WIDTH / 2 - 133, 542, 304, 2 };

	//std::cout << gotDoubleJump << std::endl;

	//Quest menu loop
	while (running) {

		//Poll to see if we close the game at any time
		SDL_PollEvent(&e);
		if (e.type == SDL_QUIT) {
			running = false;
			return;
		}
		else if (e.type == SDL_MOUSEBUTTONDOWN) {
			if (e.button.button == SDL_BUTTON_LEFT) { //if they click on a button
				//Get mouse positon
				mouseX = e.button.x;
				mouseY = e.button.y;

				//Resume Game
				if ((mouseX > resumeGame.getSprite()->getX()) && (mouseX < resumeGame.getSprite()->getX() + resumeGame.getSprite()->getWidth()) && (mouseY > resumeGame.getSprite()->getY()) && (mouseY < resumeGame.getSprite()->getY() + resumeGame.getSprite()->getHeight())) {
					gameState = 1;
					break;
				}
			}
		}

		//SDL_RenderClear(gRenderer);
		SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0x00);	//black background
		SDL_RenderFillRect(gRenderer, &questBox);

		//Draw to screen
		questLabel.draw(gRenderer, SCREEN_WIDTH / 2 - 126, 200);
		resumeGame.getSprite()->draw(gRenderer, SCREEN_WIDTH / 2 - 110, 600);

		questList.draw(gRenderer, SCREEN_WIDTH / 2 - 175, 300);

		//Check off objectives
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0x00);
		if (killedBoss != 0) {
			SDL_RenderFillRect(gRenderer, &doneKaren);
		}

		if (gotDoubleJump != 0) {
			SDL_RenderFillRect(gRenderer, &doneDoubleJump);
		}

		if (gotGrapple != 0) {
			//std::cout << "in method" << std::endl;
			SDL_RenderFillRect(gRenderer, &doneGrapple);
		}

		if (gotHealth1 != 0) {
			SDL_RenderFillRect(gRenderer, &doneHealth1);
		}

		if (gotHealth2 != 0) {
			SDL_RenderFillRect(gRenderer, &doneHealth2);
		}

		SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0x00);

		SDL_RenderPresent(gRenderer);
	}

}

// Detect collision of Entity with Gameworld (edge of screen)
bool Game::detectCollision(Entity& ent, int** tilemap, double x_vel, double y_vel, int roomHeight)
{
	double pPosY = ent.getYPosition();
	double pPosX = ent.getXPosition();
	int pHeight = ent.getCurrFrame().getHeight();
	int pWidth = ent.getCurrFrame().getWidth();		//get player height, width, & positions

	int yBlockD = (int)((pPosY + pHeight) / 16) + 1;
	int yBlockU = (int)(pPosY / 16) - 1;
	int xBlockL = (int)(pPosX / 16) - 1;					//gather adjacent block locations
	int xBlockR = (int)((pPosX + pWidth) / 16) + 1;		//including left, right, up, down
	bool land = false;			//determines if you've landed on something			

	ent.setPosition(ent.getXPosition() + x_vel, ent.getYPosition() + y_vel);	//set position

	if (y_vel >= 0) {
		for (int xAdjust = 1; xAdjust < xBlockR - xBlockL; xAdjust++)
		{	//for every block under the player's width
			if (yBlockD <= roomHeight - 1)
			{	//if you'd pass a solid block
				for (int range = pPosY; range <= ent.getYPosition(); range += 16) //for EVERY block that is passed during movement
				{
					yBlockD = (int)(((range + pHeight) / 16)) + 1;
					if (yBlockD <= roomHeight - 1 && range + pHeight + y_vel > yBlockD * 16 - 1 && (tilemap[yBlockD][xBlockR - xAdjust] != 0 && tilemap[yBlockD][xBlockR - xAdjust] != 3 && tilemap[yBlockD][xBlockR - xAdjust] != 8 && tilemap[yBlockD][xBlockR - xAdjust] != 9))
					{	//set position to above the block
						ent.setPosition(ent.getXPosition(), yBlockD * 16 - pHeight - 1);
						pPosY = ent.getYPosition();
						land = true;
						break;
					}
				}
				if (land)
					break;
			}
			else //if you're trying to fall through the bottom of the screen
			{
				/*ent.setPosition(ent.getXPosition(), SCREEN_HEIGHT - pHeight - 17);
				pPosY = ent.getYPosition();
				land = true;*/
				break;
			}
		}
	}

	bool bonk = false;
	if (y_vel < 0) {
		for (int xAdjust = 1; xAdjust < xBlockR - xBlockL; xAdjust++)
		{	//bonk head on blocks above
			if (yBlockU >= 0)
			{
				for (int range = pPosY; range >= ent.getYPosition(); range -= 16) //for EVERY block that is passed during movement
				{
					yBlockU = (int)(range / 16) - 1;
					if (yBlockU >= 0 && range + y_vel <= yBlockU * 16 + 16 && (tilemap[yBlockU][xBlockR - xAdjust] != 0 && tilemap[yBlockU][xBlockR - xAdjust] != 3 && tilemap[yBlockU][xBlockR - xAdjust] != 8 && tilemap[yBlockU][xBlockR - xAdjust] != 9))
					{
						ent.setPosition(ent.getXPosition(), yBlockU * 16 + 16);
						pPosY = ent.getYPosition();
						ent.setYVel(0);
						bonk = true;
						break;
					}
				}
				if (bonk)
					break;
			}
			else
			{
				/*ent.setPosition(ent.getXPosition(), 17);
				pPosY = ent.getYPosition();
				land = true;*/
				break;
			}
		}
	}

	if (x_vel > 0) {
		for (int yAdjust = 1; yAdjust < yBlockD - yBlockU; yAdjust++)
		{
			//hit blocks to your right accounting for player height
			if (yBlockD <= roomHeight && yBlockU >= 0 && pPosX + pWidth + x_vel >= xBlockR * 16 - 1 && (tilemap[yBlockD - yAdjust][xBlockR] != 0 && tilemap[yBlockD - yAdjust][xBlockR] != 3 && tilemap[yBlockD - yAdjust][xBlockR] != 8 && tilemap[yBlockD - yAdjust][xBlockR] != 9))
			{
				ent.setPosition(xBlockR * 16 - pWidth - 1, ent.getYPosition());
				pPosX = ent.getXPosition();
				break;
			}
		}
	}

	if (x_vel < 0) {
		for (int yAdjust = 1; yAdjust < yBlockD - yBlockU; yAdjust++)
		{
			//hit blocks to your left accounting for player height
			if (yBlockD <= roomHeight && yBlockU >= 0 && pPosX + x_vel <= xBlockL * 16 + 16 && (tilemap[yBlockD - yAdjust][xBlockL] != 0 && tilemap[yBlockD - yAdjust][xBlockL] != 3 && tilemap[yBlockD - yAdjust][xBlockL] != 8 && tilemap[yBlockD - yAdjust][xBlockL] != 9))
			{
				ent.setPosition(xBlockL * 16 + 16, ent.getYPosition());
				pPosX = ent.getXPosition();
				break;
			}
		}
	}
	return land;
}

// check player collision with an Enemy
bool Game::checkHitPlayer(Entity* player, Enemy* enemy)
{
	if (enemy->getXPosition() < player->getXPosition() + player->getCurrFrame().getWidth() &&
		enemy->getXPosition() + enemy->getCurrFrame().getWidth() > player->getXPosition() &&
		enemy->getYPosition() < player->getYPosition() + player->getCurrFrame().getHeight() &&
		enemy->getYPosition() + enemy->getCurrFrame().getHeight() > player->getYPosition())
	{
		playerHP -= enemy->getDamage();
		return true;
	}

	return false;
}

bool Game::checkHitEnemy(double x, double y, Enemy* enemy)
{
	return (
		x + projectileSize > enemy->getXPosition() &&
		x < enemy->getXPosition() + enemy->getCurrFrame().getWidth() &&
		y + projectileSize < enemy->getYPosition() &&
		y < enemy->getYPosition() + enemy->getCurrFrame().getHeight());
}

// check player collision with a generic entity
bool Game::checkPlayerCollision(Entity* player, Entity* ent) {
	if (ent->getXPosition() < player->getXPosition() + player->getCurrFrame().getWidth() &&
		ent->getXPosition() + ent->getCurrFrame().getWidth() > player->getXPosition() &&
		ent->getYPosition() < player->getYPosition() + player->getCurrFrame().getHeight() &&
		ent->getYPosition() + ent->getCurrFrame().getHeight() > player->getYPosition())
	{
		return true;
	}

	return false;
}

// Look at player position, the room, and if we are leaving (and can).
// Return the door that we are going through (4-bit) or -1
int Game::checkDoor(int doors, Entity& ent, Tilemap* currRoom) {
	// bools break up doors
	bool up = false;
	bool down = false;
	bool left = false;
	bool right = false;

	// based on doors, determine where we have doors
	if (doors >= 8) {
		up = true;
		doors -= 8;
	}
	if (doors >= 4) {
		down = true;
		doors -= 4;
	}
	if (doors >= 2) {
		left = true;
		doors -= 2;
	}
	if (doors >= 1) {
		right = true;
		doors -= 1;
	}

	// if there is a door in this direction, and we are at the edge of the room on this side and moving in that direction
	if (up && ent.getYPosition() <= 1 && ent.getYVel() < 0) {
		return 8;
	}
	else if (down && ent.getYPosition() >= currRoom->getMaxHeight() * 16 - 1 && ent.getYVel() > 0) { // needs testing when we get up/down doors
		return 4;
	}
	else if (left && ent.getXPosition() <= 0 && ent.getXVel() < 0) {
		return 2;
	}
	else if (right && ent.getXPosition() >= currRoom->getMaxWidth() * 16 - ent.getCurrFrame().getWidth() - 1 && ent.getXVel() > 0) {
		return 1;
	}
	else {
		return -1;
	}
}

void Game::drawHP()
{
	Sprite healthbarBase(0, 0, 59, 48, 4, "assets/health_bar/base.png", gRenderer);
	healthbarBase.draw(gRenderer, 50, 50);
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x00, 0x00, 0xFF);
	SDL_Rect* healthLine = new SDL_Rect;
	healthLine->y = 110;
	healthLine->w = 4;
	healthLine->h = 12;
	for (int h = 0; h < playerHP - 1; h++)
	{
		healthLine->x = 94 + 4 * h;
		SDL_RenderFillRect(gRenderer, healthLine);
	}
	if (playerHP == maxHP)
	{
		healthLine->x = 90 + 4 * playerHP;
		healthLine->h = 8;
		SDL_RenderFillRect(gRenderer, healthLine);

		healthLine->x = 94 + 4 * playerHP;
		healthLine->h = 4;
		SDL_RenderFillRect(gRenderer, healthLine);
	}

	//Reset Render color to white
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
}

void Game::drawBossHP(int health)
{
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x00, 0x00, 0xFF);
	SDL_Rect* healthLine = new SDL_Rect;
	healthLine->y = 650;
	healthLine->w = 13;
	healthLine->h = 18;
	for (int h = 0; h < health - 1; h++)
	{
		healthLine->x = 144 + 4 * h;
		SDL_RenderFillRect(gRenderer, healthLine);
	}

	//Reset Render color to white
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
}


void Game::update()
{
}

void Game::render()
{
}

void Game::displayCredits()
{
	//Roll credits
	for (int credit_image = 0; credit_image < creditFiles.size(); credit_image++) {
		SDL_RenderClear(gRenderer);
		SDL_Texture* temp = rollCredits();
		SDL_RenderCopy(gRenderer, temp, NULL, NULL);
		SDL_RenderPresent(gRenderer);
		SDL_Delay(1000);
	}

}

SDL_Texture* Game::rollCredits()
{
	SDL_Texture* temp;
	SDL_Surface* image;
	static int i = 0;
	std::string itr = creditFiles.at(i);

	SDL_RenderClear(gRenderer);
	image = IMG_Load(itr.c_str());
	std::string err = SDL_GetError();
	temp = SDL_CreateTextureFromSurface(gRenderer, image);

	i++;
	return temp;
}

void Game::runDebug() {
	//For now this method is still very similar to game code - this will change as well continue developing the game, but we should still
	//work on putting drawing functions and scrolling functions into separate functions

	playerHP = 40;

	// variables for background scrolling
	int scroll_offset_x = 0;
	int rem_bg_x = 0;
	int rem_tile_x = 0;
	int scroll_offset_y = SCREEN_HEIGHT;
	int rem_bg_y = SCREEN_HEIGHT;
	int rem_tile_y = SCREEN_HEIGHT;
	int lthird = (SCREEN_WIDTH / 3);
	int rthird = (2 * SCREEN_WIDTH / 3);
	int tthird = (SCREEN_HEIGHT / 3);
	int bthird = (2 * SCREEN_HEIGHT / 3);

	//Flip variable for flipping player sprite
	SDL_RendererFlip flip = SDL_FLIP_NONE;

	// determine players starting position (middle of background, on the left side of world)
	double x_pos = SCREEN_WIDTH / 2;
	double y_pos = SCREEN_HEIGHT - 145.0;

	//Define Graphical Objects
	Background debugBg(0, 0, 1280, 720, "assets/backgrounds/debugBg.png", gRenderer);
	Physics plp(1500, 500, 1000, 250);
	Entity player("data/player.spr", x_pos, y_pos, 3, 0, &plp, gRenderer);

	//Tiles to add to tilemap
	Tile groundTile(0, 0, 16, 16, 1, "assets/sprites/tiles.png", gRenderer);
	Tile platformTile(16, 0, 16, 16, 1, "assets/sprites/tiles.png", gRenderer);

	// Create Tile vector for tilemap contruction and push our temporary tile
	std::vector<Tile*> tiles;
	tiles.push_back(&groundTile);
	tiles.push_back(&platformTile);
	tiles.push_back(&groundTile);
	tiles.push_back(&platformTile);

	/*Generate Map
	Tilemap** map;
	int mapX = 3;
	int mapY = 3;
	map = new Tilemap * [mapY];
	for (int i = 0; i < mapY; i++)
		map[i] = new Tilemap[mapX];

	generateMap(map, mapX, mapY, tiles, &debugBg);

	//Generate tileArrays

	int** tArr0 = map[0][0].getTileMap();
	int** tArr1 = map[0][1].getTileMap();*/
	Tilemap spawn("data/tilemaps/hub/mainSpawn.txt", tiles, &debugBg);

	//Current room
	int roomNum = 0;
	Tilemap* currRoom = &spawn;

	std::vector<Entity*> tps;

	//"Load" in the game by pausing to avoid buffering in the gappling hook input
	SDL_Delay(100);

	//Main loop
	while (running && gameState == 3) {
		//Delta time calculation
		curTick = SDL_GetTicks();
		delta_time = (curTick - lastTick) / 1000.0;
		lastTick = curTick;

		//Anim frame tracker
		curAnim = SDL_GetTicks();

		//Quit game
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) {
				running = false;
			}
		}

		//Get User Input
		getUserInput(&player, tps);

		//Check for Collision
		handleCollision(&player, currRoom);

		// Update scroll if Player moves outside of middle third
		if (player.getXPosition() > (scroll_offset_x + rthird))
			scroll_offset_x = player.getXPosition() - rthird;
		else if (player.getXPosition() < (scroll_offset_x + lthird))
			scroll_offset_x = player.getXPosition() - lthird;

		if (player.getYPosition() < (scroll_offset_y + tthird))
			scroll_offset_y = player.getYPosition() - tthird;
		else if (player.getYPosition() > (scroll_offset_y + bthird))
			scroll_offset_y = player.getYPosition() - bthird;

		//Prevent scroll_offset from placing left side of the Cam outside of gameworld
		if (scroll_offset_x < 0)
			scroll_offset_x = 0;
		if (scroll_offset_x + SCREEN_WIDTH > currRoom->getMaxWidth() * 16)
			scroll_offset_x = currRoom->getMaxWidth() * 16 - SCREEN_WIDTH;

		if (scroll_offset_y < 0)
			scroll_offset_y = 0;
		if (scroll_offset_y + SCREEN_HEIGHT > currRoom->getMaxHeight() * 16)
			scroll_offset_y = currRoom->getMaxHeight() * 16 - SCREEN_HEIGHT;

		/* REMOVED FOR TESTING ON MAIN
		//Check for switching room
		if (checkDoor(roomNum, player.getXVel(), player, currRoom)) {
			//Flip room
			if (roomNum == 0) {
				roomNum = 1;
				currRoom = &map[0][1];
			}
			else {
				roomNum = 0;
				currRoom = &map[0][0];
			}

			// change all details for the room we are rendering

			if (roomNum == 0) { //new room is 0
				//set offset and player position accordingly
				scroll_offset = currRoom->getMaxWidth() * 16 - SCREEN_WIDTH;
				player.setPosition(currRoom->getMaxWidth() * 16 - player.getCurrFrame().getWidth(), player.getYPosition());
			}
			else { //new room is 1
				//set offset and player position accordingly
				scroll_offset = 0;
				player.setPosition(0, player.getYPosition());
			}
		}
		*/

		//Draw to screen
		SDL_RenderClear(gRenderer);
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

		// Draw the portion of the background currently inside the camera view
		rem_bg_x = scroll_offset_x % SCREEN_WIDTH;
		rem_tile_x = scroll_offset_x % (currRoom->getMaxWidth() * 16);
		rem_bg_y = scroll_offset_y % SCREEN_HEIGHT;
		rem_tile_y = scroll_offset_y % (currRoom->getMaxHeight() * 16);

		//Draw Room
		currRoom->getBackground()->getSprite()->draw(gRenderer, -rem_bg_x, -rem_bg_y);
		currRoom->getBackground()->getSprite()->draw(gRenderer, (-rem_bg_x + SCREEN_WIDTH), (-rem_bg_y));
		currRoom->getBackground()->getSprite()->draw(gRenderer, -rem_bg_x, -rem_bg_y + SCREEN_HEIGHT);
		currRoom->getBackground()->getSprite()->draw(gRenderer, -rem_bg_x + SCREEN_WIDTH, -rem_bg_y + SCREEN_HEIGHT);

		//Draw Tilemap
		currRoom->drawTilemap(gRenderer, rem_tile_x, rem_tile_y);

		//draw the player
		if (player.getXVel() > 0 && flip == SDL_FLIP_HORIZONTAL)
			flip = SDL_FLIP_NONE;
		else if (player.getXVel() < 0 && flip == SDL_FLIP_NONE)
			flip = SDL_FLIP_HORIZONTAL;

		if (player.getFrameIndex() == 0)
			player.getCurrFrame().draw(gRenderer, player.getXPosition() - scroll_offset_x, player.getYPosition() - scroll_offset_y);
		else
			player.getCurrFrame().draw(gRenderer, player.getXPosition() - scroll_offset_x, player.getYPosition() - scroll_offset_y, flip);

		//Draw HP
		drawHP();

		//Update Screen
		SDL_RenderPresent(gRenderer);
	}
}
