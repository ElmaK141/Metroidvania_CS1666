#include <iostream>
#include <cmath>
#include "game.h"
#include "sprite.h"
#include "entity.h"
#include "tile.h"
#include "background.h"
#include "button.h"

//size of the Window/Screen and thus the size of the Camera
int SCREEN_WIDTH;
int SCREEN_HEIGHT;

// for now - define the X length of the gameworld
constexpr int LEVEL_LEN = 3360;
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

	while (running && gameState == 0) {
		//Load main menu
		loadMainMenu();

		//Start normal game
		if (gameState == 1) {
			runGame();
		}
		//Debug
		else if (gameState == 3) {
			runDebug();
		}
	}
}

//Run the main game code
void Game::runGame() {
	//Camera-related variables
	scroll_offset = 0;
	int rem_bg = 0;
	int rem_tile = 0;
	int lthird = (SCREEN_WIDTH / 3);
	int rthird = (2 * SCREEN_WIDTH / 3);

	//Flip variable for flipping player sprite
	SDL_RendererFlip flip = SDL_FLIP_NONE;

	//Determine players starting position (middle of background, on the left side of world)
	double x_pos = SCREEN_WIDTH / 2.0;
	double y_pos = SCREEN_HEIGHT / 2.0 - 145.0;

	//Define Graphical Objects
	Background bg1(0, 0, 1280, 720, "assets/backgrounds/background1.png", gRenderer);
	Background bg2(0, 0, 1280, 720, "assets/backgrounds/background2.png", gRenderer);
	Physics plp(1500, 500, 1000, 250);
	Entity player("data/player.spr", x_pos, y_pos, 3, 0,&plp, gRenderer);		//0 is flag for player entity

	//Tiles to add to tilemap
	Tile groundTile(0, 0, 16, 16, 1, "assets/sprites/tiles.png", gRenderer);
	Tile platformTile(16, 0, 16, 16, 1, "assets/sprites/tiles.png", gRenderer);
		
	// Create Tile vector for tilemap contruction and push our temporary tile
	std::vector<Tile*> tiles;
	tiles.push_back(&groundTile);
	tiles.push_back(&platformTile);

	//Initialize tilemaps
	Tilemap t0("data/tilemaps/tilemap0.txt", tiles);
	int** tileArray0 = t0.getTileMap();
	
	//Temporarily the exact same tilemap
	Tilemap t1("data/tilemaps/tilemap1.txt", tiles);
	int** tileArray1 = t1.getTileMap();

	//0 is first room, 1 is second room
	int roomNum = 0;

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

		//Handle in-air and on-ground collision -- Different tilemap per room
		if (roomNum == 0) {
			handleCollision(&player, &t0);
		}
		else if (roomNum == 1) {
			handleCollision(&player, &t1);
		}

		// Update scroll if Player moves outside of middle third
		if (player.getXPosition() > (scroll_offset + rthird))
			scroll_offset = player.getXPosition() - rthird;
		else if (player.getXPosition() < (scroll_offset + lthird))
			scroll_offset = player.getXPosition() - lthird;

		//Prevent scroll_offset from placing Camera outside of gameworld
		if (scroll_offset < 0)
			scroll_offset = 0;
		if (scroll_offset + SCREEN_WIDTH > LEVEL_LEN)
			scroll_offset = LEVEL_LEN - SCREEN_WIDTH;

		// Check to see if we are at door
		if (checkDoor(roomNum, player.getXVel(), player)) {
			//Flip room
			if (roomNum == 0) {
				roomNum = 1;
			}
			else {
				roomNum = 0;
			}

			// change all details for the room we are rendering

			if (roomNum == 0) { //new room is 0
				//set offset and player position accordingly
				scroll_offset = LEVEL_LEN - SCREEN_WIDTH;
				player.setPosition(LEVEL_LEN - player.getCurrFrame().getWidth(), player.getYPosition());
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
		rem_tile = scroll_offset % LEVEL_LEN;
		if (roomNum == 0) {
			bg1.getSprite()->draw(gRenderer, -rem_bg, 0);
			bg1.getSprite()->draw(gRenderer, (-rem_bg + SCREEN_WIDTH), 0);

			// Temporary: Render the tilemap using the stored tile vector
			// NOTE: Do not even try to read 0's right now to avoid having
			// 		 the tilemap.txt at this point in time - JTP
			// NOTE: Temporary build: Need to fix implementation of tile vector
			//		 in tilemap implementation.
			for (int i = 0; i < t0.getMaxHeight(); i++)
			{
				for (int j = 0; j < t0.getMaxWidth(); j++)
				{
					if (tileArray0[i][j] == 1)
					{
						t0.getTile(0)->getTileSprite()->draw(gRenderer, -rem_tile + (j * 16), i * 16);
					}
					else if (tileArray0[i][j] == 2) {
						t0.getTile(1)->getTileSprite()->draw(gRenderer, -rem_tile + (j * 16), i * 16);
					}
				}
			}
		}
		else if (roomNum == 1) {
			bg2.getSprite()->draw(gRenderer, -rem_bg, 0);
			bg2.getSprite()->draw(gRenderer, (-rem_bg + SCREEN_WIDTH), 0);

			for (int i = 0; i < t1.getMaxHeight(); i++)
			{
				for (int j = 0; j < t1.getMaxWidth(); j++)
				{
					if (tileArray1[i][j] == 1)
					{
						t1.getTile(0)->getTileSprite()->draw(gRenderer, -rem_tile + (j * 16), i * 16);
					}
					else if (tileArray1[i][j] == 2) {
						t1.getTile(1)->getTileSprite()->draw(gRenderer, -rem_tile + (j * 16), i * 16);
					}
				}
			}
		}


		//draw the player
		if (player.getXVel() > 0 && flip == SDL_FLIP_HORIZONTAL)
			flip = SDL_FLIP_NONE;
		else if (player.getXVel() < 0 && flip == SDL_FLIP_NONE)
			flip = SDL_FLIP_HORIZONTAL;

		if (player.getFrameIndex() == 0)
			player.getCurrFrame().draw(gRenderer, player.getXPosition() - scroll_offset, player.getYPosition());
		else
			player.getCurrFrame().draw(gRenderer, player.getXPosition() - scroll_offset, player.getYPosition(), flip);

		//Draw the player's hp
		drawHP();

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

	//Holding W
	if (keystate[SDL_SCANCODE_W]) {

	}
	
	//Holding S
	if (keystate[SDL_SCANCODE_S]) {
		player->setCurrFrame(0);
	}

	//Holding A
	if (keystate[SDL_SCANCODE_A]) {

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

		if (player->getXVel() > -player->getPhysics()->getMaxX()) //as long as we don't exceed max speed, change velocity
			player->setXVel(fmin(player->getXVel() - player->getPhysics()->getAcceleration(), -player->getPhysics()->getMaxX()));
	}

	
	//Holding D
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

		if (player->getXVel() < player->getPhysics()->getMaxX()) //as long as we don't exceed max speed, change velocity
			player->setXVel(fmax(player->getXVel() + player->getPhysics()->getAcceleration(), player->getPhysics()->getMaxX()));
	}
	//TODO
	if (!player->getPhysics()->inAir() || (!(keystate[SDL_SCANCODE_A] || keystate[SDL_SCANCODE_D]) && player->getPhysics()->inAir())) {
		if (player->getXVel() < 0) {
			player->setXVel(fmin(0, player->getXVel() + player->getPhysics()->getAcceleration()));
		}
		else if (player->getXVel() > 0) {
			player->setXVel(fmax(0, player->getXVel() - player->getPhysics()->getAcceleration()));
		}
	}
	

	//Holding Spacebar
	if (keystate[SDL_SCANCODE_SPACE]) {
		if (!player->getPhysics()->inAir() && player->getJump())    //only jump from ground
		{
			player->setJump(false);
			player->getPhysics()->setAirState(true);
			player->setYVel(player->getYVel() - player->getPhysics()->getJumpStrength());
		}
	}

	if (!keystate[SDL_SCANCODE_SPACE]) //only jump if you've landed and pressed space again
		player->setJump(true);


	//Not holding side buttons
	if (!(keystate[SDL_SCANCODE_A] || keystate[SDL_SCANCODE_D])) {
		if(player->getFrameIndex() != 0)
			player->setCurrFrame(1);
	}
	
	if(player->getPhysics()->isGrappling()){
		
		double xComp = (grappleX - player->getXPosition()) / sqrt((grappleX - player->getXPosition()) * (grappleX - player->getXPosition()) + (grappleY - player->getYPosition()) * (grappleY - player->getYPosition()));
		double yComp = (grappleY - player->getYPosition()) / sqrt((grappleX - player->getXPosition()) * (grappleX - player->getXPosition()) + (grappleY - player->getYPosition()) * (grappleY - player->getYPosition()));
	
		//Apply grapple force
		player->setXVel((player->getXVel() + xComp * player->getPhysics()->getGrappleStr()) * player->getPhysics()->getDampen());
		player->setYVel((player->getYVel() + yComp * player->getPhysics()->getGrappleStr()) * player->getPhysics()->getDampen());
		
	}
}

//Handle the Collision
void Game::handleCollision(Entity* player, Tilemap* t) {
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

	int yBlockD = (int)(((pPosY + pHeight) / 16)) + 1;
	int yBlockU = (int)((pPosY / 16)) - 1;
	int xBlockL = (int)((pPosX / 16)) - 1;					//gather adjacent block locations
	int xBlockR = (int)(((pPosX + pWidth) / 16)) + 1;		//including left, right, up, down
	bool land = false;			//determines if you've landed on something			

	ent.setPosition(ent.getXPosition() + x_vel, ent.getYPosition() + y_vel);	//set position

	if (y_vel >= 0) {
		for (int xAdjust = 1; xAdjust < xBlockR - xBlockL; xAdjust++)
		{	//for every block under the player's width
			if (yBlockD <= 44)
			{	//if you'd pass a solid block
				if (pPosY + pHeight + y_vel > yBlockD * 16 - 1 && tilemap[yBlockD][xBlockR - xAdjust] != 0) 
				{	//set position to above the block
					ent.setPosition(ent.getXPosition(), yBlockD * 16 - pHeight - 1);
					pPosY = ent.getYPosition();
					land = true;
					break;
				}
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

	if (y_vel < 0) {
		for (int xAdjust = 1; xAdjust < xBlockR - xBlockL; xAdjust++)
		{	//bonk head on blocks above
			if (pPosY + y_vel < yBlockU * 16 + 16 && tilemap[yBlockU][xBlockR - xAdjust] != 0)
			{
				ent.setPosition(ent.getXPosition(), yBlockU * 16 + 16);
				pPosY = ent.getYPosition();
				ent.setYVel(-y_vel);
				break;
			}
		}
	}

	if (x_vel > 0) {
		for (int yAdjust = 1; yAdjust < yBlockD - yBlockU; yAdjust++)
		{	//hit blocks to your right accounting for player height
			if (pPosX + pWidth + x_vel >= xBlockR * 16 - 1 && tilemap[yBlockD - yAdjust][xBlockR] != 0)
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
			if (pPosX + x_vel <= xBlockL * 16 + 16 && tilemap[yBlockD - yAdjust][xBlockL] != 0)
			{
				ent.setPosition(xBlockL * 16 + 16, ent.getYPosition());
				pPosX = ent.getXPosition();
				break;
			}
		}
	}



	/*
	if (ent.getXPosition() + x_vel < 0) {
		ent.setPosition(0, ent.getYPosition());
	}
	if (ent.getYPosition() + y_vel < 0) {
		ent.setPosition(ent.getXPosition(), 0);
	}
	if (ent.getXPosition() + ent.getCurrFrame().getWidth() + x_vel > LEVEL_LEN) {
		ent.setPosition(LEVEL_LEN - ent.getCurrFrame().getWidth(), ent.getYPosition());
	}
	if (ent.getYPosition() + ent.getCurrFrame().getHeight() + y_vel > SCREEN_HEIGHT) {
		ent.setPosition(ent.getXPosition(), SCREEN_HEIGHT - ent.getCurrFrame().getHeight());
		return true;
	}*/
	return land;
}

bool Game::checkDoor(int room, double vel, Entity& ent) {
	if (room == 0) { //room 0 has door at LEVEL_LEN
		if (ent.getXPosition() >= LEVEL_LEN - ent.getCurrFrame().getWidth() - 1) {
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

	//Initialize tilemaps
	Tilemap t("data/tilemaps/tilemap0.txt", tiles);
	int** tileArray = t.getTileMap();

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

		getUserInput(&player);
		
		handleCollision(&player, &t);

		// Update scroll if Player moves outside of middle third
		if (player.getXPosition() > (scroll_offset + rthird))
			scroll_offset = player.getXPosition() - rthird;
		else if (player.getXPosition() < (scroll_offset + lthird))
			scroll_offset = player.getXPosition() - lthird;

		//Prevent scroll_offset from placing left side of the Cam outside of gameworld
		if (scroll_offset < 0)
			scroll_offset = 0;
		if (scroll_offset + SCREEN_WIDTH > LEVEL_LEN)
			scroll_offset = LEVEL_LEN - SCREEN_WIDTH;

		//Draw to screen
		SDL_RenderClear(gRenderer);
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

		// Draw the portion of the background currently inside the camera view
		rem_bg = scroll_offset % SCREEN_WIDTH;
		rem_tile = scroll_offset % LEVEL_LEN;
		debugBg.getSprite()->draw(gRenderer, -rem_bg, 0);
		debugBg.getSprite()->draw(gRenderer, (-rem_bg + SCREEN_WIDTH), 0);

		// Temporary: Render the tilemap using the stored tile vector
			// NOTE: Do not even try to read 0's right now to avoid having
			// 		 the tilemap.txt at this point in time - JTP
			// NOTE: Temporary build: Need to fix implementation of tile vector
			//		 in tilemap implementation.
		for (int i = 0; i < t.getMaxHeight(); i++)
		{
			for (int j = 0; j < t.getMaxWidth(); j++)
			{
				if (tileArray[i][j] == 1)
				{
					t.getTile(0)->getTileSprite()->draw(gRenderer, -rem_tile + (j * 16), i * 16);
				}
				else if (tileArray[i][j] == 2) {
					t.getTile(1)->getTileSprite()->draw(gRenderer, -rem_tile + (j * 16), i * 16);
				}
			}
		}

		//draw the player
		if (player.getXVel() > 0 && flip == SDL_FLIP_HORIZONTAL)
			flip = SDL_FLIP_NONE;
		else if (player.getXVel() < 0 && flip == SDL_FLIP_NONE)
			flip = SDL_FLIP_HORIZONTAL;

		if (player.getFrameIndex() == 0)
			player.getCurrFrame().draw(gRenderer, player.getXPosition() - scroll_offset, player.getYPosition());
		else
			player.getCurrFrame().draw(gRenderer, player.getXPosition() - scroll_offset, player.getYPosition(), flip);

		drawHP();

		SDL_RenderPresent(gRenderer);
	}
}
