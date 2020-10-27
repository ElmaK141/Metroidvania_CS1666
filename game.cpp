#include <iostream>
#include <cmath>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include "game.h"
#include "sprite.h"
#include "entity.h"
#include "enemies.h"
#include "tile.h"
#include "background.h"
#include "button.h"

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

//Animation time
Uint32 lastAnim = 0;
Uint32 curAnim;
int lastAnimFrame = 3;

//Event pointer
SDL_Event e;

//scroll offset, needs to be used in a few places
int scroll_offset;

//Game State Tracker
//0 = main menu, 1 = game, 2 = pause menu, 3 = debug
int gameState;

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
	scroll_offset = 0;
	int rem_bg = 0;
	int rem_tile = 0;
	int lthird = (SCREEN_WIDTH / 3);
	int rthird = (2 * SCREEN_WIDTH / 3);

	//Define Backgrounds
	Background bg1(0, 0, 1280, 720, "assets/backgrounds/background1.png", gRenderer);
	Background bg2(0, 0, 1280, 720, "assets/backgrounds/background2.png", gRenderer);

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

	//Initialize tilemaps
	//Tilemap t0("data/tilemaps/tilemap0.txt", tiles);
	
	// USE THIS FOR PROC GEN TILEMAP
	//Tilemap t0(210, 45, 0, tiles, &bg1);
	//Tilemap t1(210, 45, 1, tiles, &bg2);

	//Generate First Room Tilemap
	srand(time(NULL)); // seeds rand with time
	Tilemap t0;
	switch (rand() % 3) {
		case 0:
			t0 = *(new Tilemap("data/tilemaps/procgen/rExit/tmr0.txt", tiles, &bg1));
			break;
		case 1:
			t0 = *(new Tilemap("data/tilemaps/procgen/rExit/tmr1.txt", tiles, &bg1));
			break;
		case 2:
			t0 = *(new Tilemap("data/tilemaps/procgen/rExit/tmr2.txt", tiles, &bg1));
			break;
	}

	int** tileArray0 = t0.getTileMap();

	//Generate Second Room Tilemap
	srand(time(NULL)); //seeds rand with time
	Tilemap t1;
	switch (rand() % 3) {
	case 0:
		t1 = *(new Tilemap("data/tilemaps/procgen/lExit/tml0.txt", tiles, &bg2));
		break;
	case 1:
		t1 = *(new Tilemap("data/tilemaps/procgen/lExit/tml1.txt", tiles, &bg2));
		break;
	case 2:
		t1 = *(new Tilemap("data/tilemaps/procgen/lExit/tml2.txt", tiles, &bg2));
		break;
	}

	int** tileArray1 = t1.getTileMap();

	//0 is first room, 1 is second room
	int roomNum = 0;
	Tilemap* currRoom = &t0;

	//Define player Position
	double x_pos = SCREEN_WIDTH / 2.0;
	double y_pos = SCREEN_HEIGHT / 2.0 - 145.0;

	//Find player spawn point
	for (int i = 0; i < t0.getMaxHeight(); i++)
	{
		for (int j = 0; j < t0.getMaxWidth(); j++)
		{
			if (tileArray0[i][j] == 3)
			{
				x_pos = j * 16.0;
				y_pos = i * 16.0;
			}
		}
	}

	//Define player entity
	SDL_RendererFlip flip = SDL_FLIP_NONE;
	Physics plp(1500, 500, 1000, 250);
	Entity player("data/player.spr", x_pos, y_pos, 3, 0, &plp, gRenderer);		//0 is flag for player entity

	std::vector<Enemy*> enemies;
	Enemy eye("data/eye.spr", 30, 30, 3, 1, &plp, gRenderer);
	Enemy eye2("data/eye.spr", 100, 40, 3, 1, &plp, gRenderer);
	Enemy eye3("data/eye.spr", 500, 600, 3, 1, &plp, gRenderer);
	Enemy eye4("data/eye.spr", 100, 400, 3, 1, &plp, gRenderer);
	Enemy eye5("data/eye.spr", 600, 10, 3, 1, &plp, gRenderer);
	enemies.push_back(&eye);
	enemies.push_back(&eye2);
	enemies.push_back(&eye3);
	enemies.push_back(&eye4);
	enemies.push_back(&eye5);
	int hitTick = 0;
	bool hit = false;

	//"Load" in the game by pausing to avoid buffering in the gappling hook input
	SDL_Delay(100);
	
	//Run the Game
	while (running && gameState == 1) {
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

		//Get Input
		getUserInput(&player);	

		//Handle in-air and on-ground collision for current room
		handleCollision(&player, currRoom);

		if (roomNum == 0) {
			//eye.update(t0.getTileMap(), delta_time, player.getXPosition(), player.getYPosition());
		}
		else if (roomNum == 1) {
			for (int i = 0; i < enemies.size(); i++) //handle enemies; update, check for hits, give player iframes if hit
			{
				enemies[i]->update(t1.getTileMap(), delta_time, player.getXPosition(), player.getYPosition());
				if (!hit)
				{
					hit = checkHitPlayer(&player, enemies[i]);
					if (hit)
						hitTick = SDL_GetTicks();
				}
				else if (SDL_GetTicks() - hitTick > 1000)
					hit = false;
			}	
		}

		// Update scroll if Player moves outside of middle third
		if (player.getXPosition() > (scroll_offset + rthird))
			scroll_offset = player.getXPosition() - rthird;
		else if (player.getXPosition() < (scroll_offset + lthird))
			scroll_offset = player.getXPosition() - lthird;

		//Prevent scroll_offset from placing Camera outside of gameworld
		//std::cout << currRoom->getMaxWidth()*16 << std::endl;
		if (scroll_offset < 0)
			scroll_offset = 0;
		if (scroll_offset + SCREEN_WIDTH > currRoom->getMaxWidth()*16)
			scroll_offset = currRoom->getMaxWidth()*16 - SCREEN_WIDTH;

		// Check to see if we are at door
		if (checkDoor(roomNum, player.getXVel(), player, currRoom)) {
			//Flip room
			if (roomNum == 0) {
				roomNum = 1;
				currRoom = &t1;
			}
			else {
				roomNum = 0;
				currRoom = &t0;
			}

			// change all details for the room we are rendering

			if (roomNum == 0) { //new room is 0
				//set offset and player position accordingly
				scroll_offset = currRoom->getMaxWidth()*16 - SCREEN_WIDTH;
				player.setPosition(currRoom->getMaxWidth()*16 - player.getCurrFrame().getWidth(), player.getYPosition());
			}
			else { //new room is 1
				//set offset and player position accordingly
				scroll_offset = 0;
				player.setPosition(0, player.getYPosition());
			}
		}

		//Draw to screen
		SDL_RenderClear(gRenderer);
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);


		// Draw the portion of the background currently inside the camera view
		rem_bg = scroll_offset % SCREEN_WIDTH;
		rem_tile = scroll_offset % (currRoom->getMaxWidth()*16);
		if (roomNum == 0) {
			t0.getBackground()->getSprite()->draw(gRenderer, -rem_bg, 0);
			t0.getBackground()->getSprite()->draw(gRenderer, (-rem_bg + SCREEN_WIDTH), 0);

			t0.drawTilemap(gRenderer, rem_tile);
		}
		else if (roomNum == 1) {
			t1.getBackground()->getSprite()->draw(gRenderer, -rem_bg, 0);
			t1.getBackground()->getSprite()->draw(gRenderer, (-rem_bg + SCREEN_WIDTH), 0);
			
			t1.drawTilemap(gRenderer, rem_tile);
		}


		//draw the player
		if (player.getXVel() > 0 && flip == SDL_FLIP_HORIZONTAL)
			flip = SDL_FLIP_NONE;
		else if (player.getXVel() < 0 && flip == SDL_FLIP_NONE)
			flip = SDL_FLIP_HORIZONTAL;

		//hit iframes indicator; blink a tenth of a second every tenth of a second
		if (player.getFrameIndex() == 0 && (SDL_GetTicks() % 100 > 50 || !hit))
			player.getCurrFrame().draw(gRenderer, player.getXPosition() - scroll_offset, player.getYPosition());
		else if (SDL_GetTicks() % 100 > 50 || !hit)
			player.getCurrFrame().draw(gRenderer, player.getXPosition() - scroll_offset, player.getYPosition(), flip);
		if (roomNum == 1)
		{
			for (int i = 0; i < enemies.size(); i++)
			{
				enemies[i]->getCurrFrame().draw(gRenderer, enemies[i]->getXPosition() - scroll_offset, enemies[i]->getYPosition());
			}
		}

		//Draw the player's hp
		drawHP();

		if (playerHP <= 0) {
			gameState = 4;
			break;
		}

		SDL_RenderPresent(gRenderer);
	}
}

//Handle the user input
void Game::getUserInput(Entity* player) {
	//User input
	const Uint8* keystate = SDL_GetKeyboardState(nullptr);
	
	//Mouse Coordinate Variables
	int mouseXinWorld = 0, mouseYinWorld = 0;

	//Pause game and bring up Pause Menu (with esc)
	if (e.type == SDL_KEYDOWN) { //detect escape keydown
		switch (e.key.keysym.sym) {
		
		case SDLK_ESCAPE:
			if(!player->getPhysics()->inAir())
				pauseMenu(gameState);
			break;
		}
	}
	
	if (SDL_BUTTON(SDL_BUTTON_LEFT) & SDL_GetMouseState(&mouseXinWorld, &mouseYinWorld)) {
		mouseXinWorld += scroll_offset;
		
		
		
		if(!player->getPhysics()->isGrappling()){
			//std::cout << "pressed" << std::endl;
			grappleX = mouseXinWorld;
			grappleY = mouseYinWorld;
			
			player->getPhysics()->setGrappleState(true);
		}
	}
	else{
		
		if(player->getPhysics()->isGrappling()){
			//std::cout << "unpressed" << std::endl;
			player->getPhysics()->setGrappleState(false);
		}
	}

	//Holding E
	if (keystate[SDL_SCANCODE_E] && player->getShot())
	{/*Only allow one shot at a time*/
		/*
		//Emil
		player->setShot(false);
		// set the start position of projectile to be where the user is
		//only making horizontal projectile
		player->setPX(player->getXPosition());
		player->setPY(player->getYPosition());
		player->setPVel(-1);
		player->getCurrFrame().draw(gRenderer, player->getPX() - scroll_offset, player->getPY());
		*/
	}

	//Redone player physics
	if(player->getPhysics()->inAir()){
		//Air control
		
		//Holding Spacebar
		if (keystate[SDL_SCANCODE_SPACE]) {
			if (!player->getPhysics()->inAir() && player->getJump())    //only jump from ground
			{
				player->setJump(false);
				player->getPhysics()->setAirState(true);
				player->setYVel(player->getYVel() - player->getPhysics()->getJumpStrength());
			}
		}

		if (!keystate[SDL_SCANCODE_SPACE]){ //only jump if you've landed and pressed space again
			player->setJump(true);
		}
		
		if (keystate[SDL_SCANCODE_A]) {
			//Movement
			if (player->getXVel() > -player->getPhysics()->getMaxX()){ //as long as we don't exceed max speed, change velocity
				player->setXVel(fmin(player->getXVel() - player->getPhysics()->getAcceleration(), -player->getPhysics()->getMaxX()));
			}
		}
		
		if (keystate[SDL_SCANCODE_S]) {
			//Animation
			player->setCurrFrame(0);
			
			//Movement
		}
		
		if (keystate[SDL_SCANCODE_D]) {
			//Movement
			if (player->getXVel() < player->getPhysics()->getMaxX()){ //as long as we don't exceed max speed, change velocity
				player->setXVel(fmax(player->getXVel() + player->getPhysics()->getAcceleration(), player->getPhysics()->getMaxX()));
			}
		}
		
		//Not holding side buttons
		if (!(keystate[SDL_SCANCODE_A] || keystate[SDL_SCANCODE_D])) {
			//Animation
			if(player->getFrameIndex() != 0)
				player->setCurrFrame(1);
		}
		
		if(player->getPhysics()->isGrappling()){
			
			double playerCenterX = player->getXPosition() + (player->getCurrFrame().getWidth()/2);
			double playerCenterY = player->getYPosition() + (player->getCurrFrame().getHeight()/2);
			
			double xComp = (grappleX - playerCenterX) / sqrt((grappleX - playerCenterX) * (grappleX - playerCenterX) + (grappleY - playerCenterY) * (grappleY - playerCenterY));
			double yComp = (grappleY - playerCenterY) / sqrt((grappleX - playerCenterX) * (grappleX - playerCenterX) + (grappleY - playerCenterY) * (grappleY - playerCenterY));
		
			//Apply grapple force
			player->setXVel((player->getXVel() + xComp * player->getPhysics()->getGrappleStr()) * player->getPhysics()->getDampen());
			player->setYVel((player->getYVel() + yComp * player->getPhysics()->getGrappleStr()) * player->getPhysics()->getDampen());
			
		}
	}
	else{
		//Ground control
		
		//Holding Spacebar
		if (keystate[SDL_SCANCODE_SPACE]) {
			if (!player->getPhysics()->inAir() && player->getJump())    //only jump from ground
			{
				player->setJump(false);
				player->getPhysics()->setAirState(true);
				player->setYVel(player->getYVel() - player->getPhysics()->getJumpStrength());
			}
		}

		if (!keystate[SDL_SCANCODE_SPACE]){ //only jump if you've landed and pressed space again
			player->setJump(true);
		}
		
		if (keystate[SDL_SCANCODE_A]) {
			//Animation
			if (curAnim - lastAnim >= 200.0) {
				lastAnim = curAnim;
				if (player->getFrameIndex() == 1 || player->getFrameIndex() == 0) {
					if (lastAnimFrame == 2) {
						player->setCurrFrame(3);
						lastAnimFrame = 3;
					}
					else {
						player->setCurrFrame(2);
						lastAnimFrame = 2;
					}
				}
				else {
					player->setCurrFrame(1);
				}
			}
			
			//Movement
			if (player->getXVel() > -player->getPhysics()->getMaxX()){ //as long as we don't exceed max speed, change velocity
				player->setXVel(fmin(player->getXVel() - player->getPhysics()->getAcceleration(), -player->getPhysics()->getMaxX()));
			}
		}
		
		if (keystate[SDL_SCANCODE_S]) {
			//Animation
			player->setCurrFrame(0);
		}
		
		if (keystate[SDL_SCANCODE_D]) {
			//Animation
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
					}
				}
				else {
					player->setCurrFrame(1);
				}
			}
			
			//Movement
			
			if (player->getXVel() < player->getPhysics()->getMaxX()){ //as long as we don't exceed max speed, change velocity
				player->setXVel(fmax(player->getXVel() + player->getPhysics()->getAcceleration(), player->getPhysics()->getMaxX()));
			}
		}
		else
		
		//Not holding side buttons
		if (!(keystate[SDL_SCANCODE_A] || keystate[SDL_SCANCODE_D])) {
			//Animation
			if(player->getFrameIndex() != 0)
				player->setCurrFrame(1);
				
			//Movement
			if (player->getXVel() < 0) {
				player->setXVel(fmin(0, player->getXVel() + player->getPhysics()->getAcceleration()));
			}
			else if (player->getXVel() > 0) {
				player->setXVel(fmax(0, player->getXVel() - player->getPhysics()->getAcceleration()));
			}
		}
		
		if(player->getPhysics()->isGrappling()){
			
			double playerCenterX = player->getXPosition() + (player->getCurrFrame().getWidth()/2);
			double playerCenterY = player->getYPosition() + (player->getCurrFrame().getHeight()/2);
			
			double xComp = (grappleX - playerCenterX) / sqrt((grappleX - playerCenterX) * (grappleX - playerCenterX) + (grappleY - playerCenterY) * (grappleY - playerCenterY));
			double yComp = (grappleY - playerCenterY) / sqrt((grappleX - playerCenterX) * (grappleX - playerCenterX) + (grappleY - playerCenterY) * (grappleY - playerCenterY));
		
			//Apply grapple force
			player->setXVel((player->getXVel() + xComp * player->getPhysics()->getGrappleStr()) * player->getPhysics()->getDampen());
			player->setYVel((player->getYVel() + yComp * player->getPhysics()->getGrappleStr()) * player->getPhysics()->getDampen());
			
		}
	}
	
	//Old player Physics
	
	/*
	//THIS IS RESPONSABLE FOR THE SLOW SIDEWAYS GRAPPLE SPEED
	if (!player->getPhysics()->inAir() || (!(keystate[SDL_SCANCODE_A] || keystate[SDL_SCANCODE_D]) && player->getPhysics()->inAir())) {
		if (player->getXVel() < 0) {
			player->setXVel(fmin(0, player->getXVel() + player->getPhysics()->getAcceleration()));
		}
		else if (player->getXVel() > 0) {
			player->setXVel(fmax(0, player->getXVel() - player->getPhysics()->getAcceleration()));
		}
	}
	*/
	
}

//Handle the Collision
void Game::handleCollision(Entity* player, Tilemap* t) {
	if (!player->getShot())
	{/*If a shot has been fired*/
		/*
		 //EMIL
		std::cout << player->getPX() << std::endl;
		player->setPX(player->getPX() + player->getPVel());

		// if collision, just set shoot to true
		*/
	}
	bool on_solid = detectCollision(*player, t->getTileMap(), player->getXVel() * delta_time, player->getYVel() * delta_time);
	if (!on_solid) // while in air
	{
		if(player->getPhysics()->getMaxY() > player->getYVel())
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
	Sprite deadMsg(0,0, 1280, 720, 1, "assets/kennedys.png", gRenderer);

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

	SDL_Rect pauseBox = {SCREEN_WIDTH/2 - 163, 190, 325, 300};

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
		/*
		else if (e.type == SDL_KEYDOWN) { //or close menu with escape again
			switch (e.key.keysym.sym) {

			case SDLK_ESCAPE:
				gameState = 1;
				pauseM = false;
				break;
			}
		}
		*/

		//SDL_RenderClear(gRenderer);
		SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0x00);	//black background
		SDL_RenderFillRect(gRenderer, &pauseBox);

		//Draw to screen
		pauseLabel.draw(gRenderer, SCREEN_WIDTH/2 - 126, 200);
		resumeGame.getSprite()->draw(gRenderer, SCREEN_WIDTH/2 - 110, 300);
		mainMenu.getSprite()->draw(gRenderer, SCREEN_WIDTH/2 - 152, 400);

		SDL_RenderPresent(gRenderer);
	}

}

// Detect collision of Entity with Gameworld (edge of screen)
bool Game::detectCollision(Entity& ent, int** tilemap, double x_vel, double y_vel)
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
			if (yBlockD <= 44)
			{	//if you'd pass a solid block
				for (int range = pPosY; range <= ent.getYPosition(); range += 16) //for EVERY block that is passed during movement
				{
					yBlockD = (int)(((range + pHeight) / 16)) + 1;
					if (range + pHeight + y_vel > yBlockD * 16 - 1 && (tilemap[yBlockD][xBlockR - xAdjust] != 0 && tilemap[yBlockD][xBlockR - xAdjust] != 3))
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
				ent.setPosition(ent.getXPosition(), SCREEN_HEIGHT - pHeight - 17);
				pPosY = ent.getYPosition();
				land = true;
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
					if (range + y_vel <= yBlockU * 16 + 16 && (tilemap[yBlockU][xBlockR - xAdjust] != 0 && tilemap[yBlockU][xBlockR - xAdjust] != 3))
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
				ent.setPosition(ent.getXPosition(), 17);
				pPosY = ent.getYPosition();
				land = true;
				break;
			}
		}
	}

	if (x_vel > 0) {
		for (int yAdjust = 1; yAdjust < yBlockD - yBlockU; yAdjust++)
		{	//hit blocks to your right accounting for player height
			if (pPosX + pWidth + x_vel >= xBlockR * 16 - 1 && (tilemap[yBlockD - yAdjust][xBlockR] != 0 && tilemap[yBlockD - yAdjust][xBlockR] != 3))
			{
				ent.setPosition(xBlockR * 16 - pWidth - 1, ent.getYPosition());
				pPosX = ent.getXPosition();
				break;
			}
		}
	}

	if (x_vel < 0) {
		for (int yAdjust = 1; yAdjust < yBlockD - yBlockU; yAdjust++)
		{	//hit blocks to your left accounting for player height
			if (pPosX + x_vel <= xBlockL * 16 + 16 && (tilemap[yBlockD - yAdjust][xBlockL] != 0 && tilemap[yBlockD - yAdjust][xBlockL] != 3))
			{
				ent.setPosition(xBlockL * 16 + 16, ent.getYPosition());
				pPosX = ent.getXPosition();
				break;
			}
		}
	}
	return land;
}

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

bool Game::checkDoor(int room, double vel, Entity& ent, Tilemap* currRoom) {
	if (room == 0) { //room 0 has door at the end of the room
		if (ent.getXPosition() >= currRoom->getMaxWidth()*16 - ent.getCurrFrame().getWidth() - 1) {
			//we are moving into right door
			return true;
		}
	}
	else { // room 1 has door at 0
		if (ent.getXPosition() <= 0 && vel < 0) {
			//we are moving into left door
			return true;
		}
	}
	return false;
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

void Game::generateMap(Tilemap** map, int mapX, int mapY, std::vector<Tile*> tiles, Background* bg) {
	srand(time(NULL));
	//Decide what size rooms to generate/initialize the tilemaps
	for (int i = 0; i < mapY; i++) {
		for (int j = 0; j < mapX; j++) {
			int roomSize = rand() % 3;		//For now, equal probability for small, medium, or large room
			
			//std::cout << roomSize << std::endl;

			//TODO: Make verticle rooms too, and also rooms with different proportions.
			//Right now every room has doors on the left or right and we will need ones with doors on the top/bottom

			switch (roomSize) {
				case 0:		//small room
					map[i][j] = *(new Tilemap(90, 45, 0, tiles, bg));
					break;
				case 1:		//medium room
					map[i][j] = *(new Tilemap(210, 45, 0, tiles, bg));
					break;
				case 2:		//large room
					map[i][j] = *(new Tilemap(420, 45, 0, tiles, bg));
					break;
;			}
		}
	}
}

void Game::update()
{
}

void Game::render()
{
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
	int scroll_offset = 0;
	int rem_bg = 0;
	int rem_tile = 0;
	int lthird = (SCREEN_WIDTH / 3);
	int rthird = (2 * SCREEN_WIDTH / 3);

	//Flip variable for flipping player sprite
	SDL_RendererFlip flip = SDL_FLIP_NONE;

	// determine players starting position (middle of background, on the left side of world)
	double x_pos = SCREEN_WIDTH / 2;
	double y_pos = SCREEN_HEIGHT / 2 - 145.0;

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

	//Generate Map
	Tilemap** map;
	int mapX = 3;
	int mapY = 3;
	map = new Tilemap * [mapY];
	for (int i = 0; i < mapY; i++)
		map[i] = new Tilemap[mapX];

	generateMap(map, mapX, mapY, tiles, &debugBg);

	//Generate tileArrays

	int** tArr0 = map[0][0].getTileMap();
	int** tArr1 = map[0][1].getTileMap();

	//Current room
	int roomNum = 0;
	Tilemap* currRoom = &map[0][0];

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
		getUserInput(&player);
		
		//Check for Collision
		handleCollision(&player, currRoom);

		// Update scroll if Player moves outside of middle third
		if (player.getXPosition() > (scroll_offset + rthird))
			scroll_offset = player.getXPosition() - rthird;
		else if (player.getXPosition() < (scroll_offset + lthird))
			scroll_offset = player.getXPosition() - lthird;

		//Prevent scroll_offset from placing left side of the Cam outside of gameworld
		if (scroll_offset < 0)
			scroll_offset = 0;
		if (scroll_offset + SCREEN_WIDTH > currRoom->getMaxWidth()*16)
			scroll_offset = currRoom->getMaxWidth()*16 - SCREEN_WIDTH;

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

		//Draw to screen
		SDL_RenderClear(gRenderer);
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

		// Draw the portion of the background currently inside the camera view
		rem_bg = scroll_offset % SCREEN_WIDTH;
		rem_tile = scroll_offset % (currRoom->getMaxWidth()*16);

		//Draw Room
		currRoom->getBackground()->getSprite()->draw(gRenderer, -rem_bg, 0);
		currRoom->getBackground()->getSprite()->draw(gRenderer, (-rem_bg + SCREEN_WIDTH), 0);

		//Draw Tilemap
		currRoom->drawTilemap(gRenderer, rem_tile);

		//draw the player
		if (player.getXVel() > 0 && flip == SDL_FLIP_HORIZONTAL)
			flip = SDL_FLIP_NONE;
		else if (player.getXVel() < 0 && flip == SDL_FLIP_NONE)
			flip = SDL_FLIP_HORIZONTAL;

		if (player.getFrameIndex() == 0)
			player.getCurrFrame().draw(gRenderer, player.getXPosition() - scroll_offset, player.getYPosition());
		else
			player.getCurrFrame().draw(gRenderer, player.getXPosition() - scroll_offset, player.getYPosition(), flip);

		//Draw HP
		drawHP();

		//Update Screen
		SDL_RenderPresent(gRenderer);
	}
}
