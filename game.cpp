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


bool in_air = true;
double gravity = 0.03 * 3600;
double jump_strength = 1500;
double max_x_speed = 500;	//max velocity, prevents weird speed issues
double max_y_speed = 1000;	//Higher in y directions to give more weight to player
double acceleration = 250; //Player acceleration

//Init Stuff for delta_time
Uint32 lastTick = 0;
Uint32 curTick;
double delta_time;


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

Game::~Game() 
{
	running = false;
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	IMG_Quit();
	SDL_Quit();
}



void Game::runGame()
{
	Sprite mmTitle(0, 0, 796, 125, 1, "assets/main_menu/mainmenuTitle.png", gRenderer);
	Sprite mmPressAny(0, 0, 485, 56, 1, "assets/main_menu/pressAny.png", gRenderer);

	SDL_Event e;
	while (running == true) {	//Start screen

		SDL_PollEvent(&e);
		if (e.type == SDL_QUIT) {
			running = false;
		}
		else if (e.type == SDL_KEYDOWN)
			break;

		SDL_RenderClear(gRenderer);
		SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0x00);	//black background

		mmTitle.draw(gRenderer, 240, 120);
		if (SDL_GetTicks() % 1200 <= 700)	//blinking text
			mmPressAny.draw(gRenderer, 397, 400);

		SDL_RenderPresent(gRenderer);
	}

	//Main Menu State Variable and button variables
	int state = 0;
	int mouseX = 0, mouseY = 0;
	int unlockDebug = 0;
	Button startGame(0, 0, 452, 68, 1, "assets/main_menu/startGame.png", gRenderer);
	Button debug(0, 0, 294, 68, 1, "assets/main_menu/debug.png", gRenderer);

	while (running == true) {	//Main Menu

		SDL_PollEvent(&e);
		if (e.type == SDL_QUIT) {
			running = false;
		}
		else if (e.type == SDL_MOUSEBUTTONDOWN) {
			if (e.button.button == SDL_BUTTON_LEFT) {
				//Get mouse positon
				mouseX = e.button.x;
				mouseY = e.button.y;

				//Start Game
				if ((mouseX > startGame.getSprite()->getX()) && (mouseX < startGame.getSprite()->getX() + startGame.getSprite()->getWidth()) && (mouseY > startGame.getSprite()->getY()) && (mouseY < startGame.getSprite()->getY() + startGame.getSprite()->getHeight())) {
					state = 1;		//Start into main game
					break;
				}
				//Debug
				else if (unlockDebug && (mouseX > debug.getSprite()->getX()) && (mouseX < debug.getSprite()->getX() + debug.getSprite()->getWidth()) && (mouseY > debug.getSprite()->getY()) && (mouseY < debug.getSprite()->getY() + debug.getSprite()->getHeight())) {
					state = 2;		//Start into debug
					break;
				}
			}
			
		}
		else if (e.type == SDL_KEYDOWN) {
			switch (e.key.keysym.sym) {

			case SDLK_d:
				unlockDebug = 1;
				break;
			}
		}

		SDL_RenderClear(gRenderer);
		SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0x00);	//black background

		mmTitle.draw(gRenderer, 240, 120);
		startGame.getSprite()->draw(gRenderer, 397, 400);

		if (unlockDebug == 1)
			debug.getSprite()->draw(gRenderer, 397, 500);

		SDL_RenderPresent(gRenderer);

	}

	//Start normal game
	if (state == 1) {
		//Do nothing for now
	}
	//Debug
	else if (state == 2) {
		runDebug();
		running = false;
	}

	// Start Game

	// variables for background scrolling
	int scroll_offset = 0;
	int rem_bg = 0;
	int rem_tile = 0;

	//player hp values
	maxHP = 40;
	playerHP = 40;

	//Flip variable for flipping player sprite
	SDL_RendererFlip flip = SDL_FLIP_NONE;

	// Define where the Camera Thirds are for knowing when we should scroll
	int lthird = (SCREEN_WIDTH / 3);
	int rthird = (2 * SCREEN_WIDTH / 3);

	// determine players starting position (middle of background, on the left side of world)

	double x_pos = SCREEN_WIDTH / 2;
	double y_pos = SCREEN_HEIGHT / 2 - 145.0;
	
	//Define Graphical Objects
	Background bg1(0, 0, 1280, 720, "assets/backgrounds/debugBg.png", gRenderer);
	Background bg2(0, 0, 1280, 720, "assets/backgrounds/background2.png", gRenderer);
	Entity player("data/player.spr", (int)x_pos, (int)y_pos, 3, gRenderer);
	
	Sprite wall(98,96,16,16,4,"assets/sprites/tiles.png",gRenderer);
	Sprite floor(435, 94, 16, 16, 4, "assets/sprites/tiles.png", gRenderer);

	double x_vel = 0;
	double y_vel = 0;

	//Create temporary standalone tile
	Sprite platform(16, 0, 16, 16, 1, "assets/sprites/tiles.png", gRenderer);
	Tile platformTile(&platform);
		
	// Create Tile vector for tilemap contruction and push our temporary tile
	std::vector<Tile*> tiles;
	tiles.push_back(&(platformTile));

	Tilemap t("data/tilemap.txt", tiles);
	int** tileArray = t.getTileMap();

	int index = 0;

	// 1 -> Door is at right edge, -1 -> Door is at left edge
	int roomNum = 1;
	bool jumpLag = false;
	
	while (running == true) {
		
		//Delta time calculation
		curTick = SDL_GetTicks();
		delta_time = (curTick - lastTick) / 1000.0;
		lastTick = curTick;
		
		//User input
		const Uint8* keystate = SDL_GetKeyboardState(nullptr);
		
		//Holding W
		if (keystate[SDL_SCANCODE_W]){
			
		}
		
		//Holding A
		if (keystate[SDL_SCANCODE_A]){
			player.setCurrFrame(1);
			if (x_vel > -max_x_speed) //as long as we don't exceed max speed, change velocity
				x_vel = fmin(x_vel - acceleration, -max_x_speed);
		}
		
		//Holding S
		if (keystate[SDL_SCANCODE_S]){
			player.setCurrFrame(0);
		}
		
		//Holding D
		if (keystate[SDL_SCANCODE_D]){
			player.setCurrFrame(1);
			if (x_vel < max_x_speed) //as long as we don't exceed max speed, change velocity
				x_vel = fmax(x_vel + acceleration, max_x_speed);
		}
		
		//Holding Spacebar
		if (keystate[SDL_SCANCODE_SPACE]) {
			if (!in_air && !jumpLag)    //only jump from ground
			{
				jumpLag = true;
				in_air = true;
				y_vel -= jump_strength;
			}
		}

		if (!keystate[SDL_SCANCODE_SPACE])
			jumpLag = false;

		
		//Not holding side buttons
		if(!(keystate[SDL_SCANCODE_A] || keystate[SDL_SCANCODE_D])){
			if(x_vel > 0){
				x_vel = fmax(0, x_vel - acceleration);
			}
			else if(x_vel < 0){
				x_vel = fmin(0, x_vel + acceleration);
			}
		}
		
		//Quit game
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) {
				running = false;
			}
		}
		
		bool on_solid = detectCollision(player, t.getTileMap(), x_vel*delta_time, y_vel*delta_time);
		bool falling = false;
		if (!on_solid && max_y_speed > y_vel) // while in air
		{
			y_vel += gravity;
			in_air = true;
			//player.setCurrFrame(1);
		}
		else if (on_solid)
		{
			in_air = false;
		}
		if (!in_air)
		{
			//player.setCurrFrame(0);
			y_vel = 0.0;
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
		if (checkDoor(roomNum, x_vel, player)) {
			roomNum *= -1; //flip room

			// change all details for the room we are rendering
			
			if(roomNum == 1){ //new room is 1
				//set offset and player position accordingly
				scroll_offset = LEVEL_LEN - SCREEN_WIDTH;
				player.setPosition(LEVEL_LEN - player.getCurrFrame().getWidth(), player.getYPosition());
				//std::cout << roomNum << " " << scroll_offset << " " << player.getXPosition() << std::endl;
			}
			else { //new room is -1
				//set offset and player position accordingly
				scroll_offset = 0;
				player.setPosition(0, player.getYPosition());
				//std::cout << roomNum << " " << scroll_offset << " " << player.getXPosition() << std::endl;
			}
		}
		
		//Draw to screen
		SDL_RenderClear(gRenderer);
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

		
		// Draw the portion of the background currently inside the camera view
		rem_bg = scroll_offset % SCREEN_WIDTH;
		rem_tile = scroll_offset % LEVEL_LEN;
		if (roomNum == 1) {
			bg1.getSprite()->draw(gRenderer, -rem_bg, 0);
			bg1.getSprite()->draw(gRenderer, (-rem_bg + SCREEN_WIDTH), 0);

			//Temporarily manually draw platforms

			//Platform 1
			platformTile.getTileSprite()->draw(gRenderer, -rem_tile + 400, 480);
			platformTile.getTileSprite()->draw(gRenderer, -rem_tile + 416, 480);
			platformTile.getTileSprite()->draw(gRenderer, -rem_tile + 432, 480);
			platformTile.getTileSprite()->draw(gRenderer, -rem_tile + 448, 480);
			platformTile.getTileSprite()->draw(gRenderer, -rem_tile + 464, 480);
			platformTile.getTileSprite()->draw(gRenderer, -rem_tile + 480, 480);
			platformTile.getTileSprite()->draw(gRenderer, -rem_tile + 496, 480);
			platformTile.getTileSprite()->draw(gRenderer, -rem_tile + 512, 480);

			//Platform 2
			platformTile.getTileSprite()->draw(gRenderer, -rem_tile + 736, 480);
			platformTile.getTileSprite()->draw(gRenderer, -rem_tile + 752, 480);
			platformTile.getTileSprite()->draw(gRenderer, -rem_tile + 768, 480);
			platformTile.getTileSprite()->draw(gRenderer, -rem_tile + 784, 480);
			platformTile.getTileSprite()->draw(gRenderer, -rem_tile + 800, 480);
			platformTile.getTileSprite()->draw(gRenderer, -rem_tile + 816, 480);
			platformTile.getTileSprite()->draw(gRenderer, -rem_tile + 832, 480);
			platformTile.getTileSprite()->draw(gRenderer, -rem_tile + 848, 480);

			//Platform 3
			platformTile.getTileSprite()->draw(gRenderer, -rem_tile + 568, 390);
			platformTile.getTileSprite()->draw(gRenderer, -rem_tile + 584, 390);
			platformTile.getTileSprite()->draw(gRenderer, -rem_tile + 600, 390);
			platformTile.getTileSprite()->draw(gRenderer, -rem_tile + 616, 390);
			platformTile.getTileSprite()->draw(gRenderer, -rem_tile + 632, 390);
			platformTile.getTileSprite()->draw(gRenderer, -rem_tile + 648, 390);
			platformTile.getTileSprite()->draw(gRenderer, -rem_tile + 664, 390);
			platformTile.getTileSprite()->draw(gRenderer, -rem_tile + 680, 390);

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
						platformTile.getTileSprite()->draw(gRenderer, -rem_tile + (j * 16), i * 16);
					}
				}
			}
		}
		else if (roomNum == -1) {
			bg2.getSprite()->draw(gRenderer, -rem_bg, 0);
			bg2.getSprite()->draw(gRenderer, (-rem_bg + SCREEN_WIDTH), 0);
		}


		//draw the player
		if (x_vel > 0 && flip == SDL_FLIP_HORIZONTAL)
			flip = SDL_FLIP_NONE;
		else if (x_vel < 0 && flip == SDL_FLIP_NONE)
			flip = SDL_FLIP_HORIZONTAL;

		if (player.getFrameIndex() == 1)
			player.getCurrFrame().draw(gRenderer, player.getXPosition() - scroll_offset, player.getYPosition(), flip);
		else
			player.getCurrFrame().draw(gRenderer, player.getXPosition() - scroll_offset, player.getYPosition());

		drawHP();

		SDL_RenderPresent(gRenderer);
	}
}


// Detect collision of Entity with Gameworld (edge of screen)
bool Game::detectCollision(Entity& ent, int** tilemap, double x_vel, double y_vel)
{
	double pPosY = ent.getYPosition();
	double pPosX = ent.getXPosition();
	int pHeight = ent.getCurrFrame().getHeight();
	int pWidth = ent.getCurrFrame().getWidth();

	int yBlockD = (int)(floor((pPosY + pHeight) / 16)) + 1;
	int yBlockU = (int)(floor(pPosY / 16)) - 1;
	int xBlockL = (int)(floor(pPosX / 16)) - 1;
	int xBlockR = (int)(floor((pPosX + pWidth) / 16)) + 1;
	bool land = false;
	
	ent.setPosition(ent.getXPosition() + x_vel, ent.getYPosition() + y_vel);
	

	if (y_vel > 0) {
		for (int xAdjust = 1; xAdjust <= xBlockR - xBlockL; xAdjust++)
		{
			if (yBlockD <= 44)
			{
				if (pPosY + pHeight + y_vel >= yBlockD * 16 - 1 && tilemap[yBlockD][xBlockR - xAdjust] != 0)
				{
					ent.setPosition(ent.getXPosition(), yBlockD * 16 - pHeight - 1);
					land = true;
				}
			}
		}
	}

	if (y_vel < 0) {
		for (int xAdjust = 1; xAdjust <= xBlockR - xBlockL; xAdjust++)
		{
			if (pPosY + y_vel <= yBlockU * 16 + 16 && tilemap[yBlockU][xBlockR - xAdjust] != 0)
				ent.setPosition(ent.getXPosition(), yBlockU * 16 + 17);
		}
	}

	if (x_vel > 0) {
		for (int yAdjust = 1; yAdjust <= yBlockD - yBlockU; yAdjust++)
		{
			if (pPosX + pWidth + x_vel >= xBlockR * 16 - 1 && tilemap[yBlockD - yAdjust][xBlockR] != 0)
				ent.setPosition(xBlockR * 16 - pWidth - 1, ent.getYPosition());
		}
	}

	if (x_vel < 0) {
		for (int yAdjust = 1; yAdjust <= yBlockD - yBlockU; yAdjust++)
		{
			if (pPosX + x_vel <= xBlockL * 16 + 16 && tilemap[yBlockD - yAdjust][xBlockL] != 0)
				ent.setPosition(xBlockL * 16 + 16, ent.getYPosition());
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
	if (room == 1) { //room 1 has door at LEVEL_LEN
		if (ent.getXPosition() >= LEVEL_LEN - ent.getCurrFrame().getWidth() - 1) {
			//we are moving into right door
			return true;
		}
	}
	else { // room -1 has door at 0
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
	//For now this method is just a copy of the game code with changes to make it more useful for debug---should later change it so they use
	//same code but this one simply runs it in a debug environment. This requires splitting universal functions and logic up from game design
	//Code and putting them in separate methods

	// Define event variable
	SDL_Event e;

	// variables for background scrolling
	int scroll_offset = 0;
	int rem_bg = 0;
	int rem_tile = 0;

	//Flip variable for flipping player sprite
	SDL_RendererFlip flip = SDL_FLIP_NONE;

	// Define where the Camera Thirds are for knowing when we should scroll
	int lthird = (SCREEN_WIDTH / 3);
	int rthird = (2 * SCREEN_WIDTH / 3);

	// determine players starting position (middle of background, on the left side of world)

	double x_pos = SCREEN_WIDTH / 2;
	double y_pos = SCREEN_HEIGHT / 2 - 145.0;

	//Define Graphical Objects
	Background debugBg(0, 0, 1280, 720, "assets/backgrounds/debugBg.png", gRenderer);
	Entity player("data/player.spr", x_pos, y_pos, 3, gRenderer);
	
	Sprite groundTile(0, 0, 16, 16, 1, "assets/sprites/tiles.png", gRenderer);
	Sprite platform(16, 0, 16, 16, 1, "assets/sprites/tiles.png", gRenderer);
	std::vector<Tile*> tiles;

	Tilemap tilemap("data/tilemap.txt", tiles);

	//Create temporary standalone tile
	Tile platformTile(&platform);

	//Initialize velocity
	double x_vel = 0;
	double y_vel = 0;

	//Main loop
	int index = 0;
	while (running == true) {
		//Delta time calculation
		curTick = SDL_GetTicks();
		delta_time = (curTick - lastTick) / 1000.0;
		lastTick = curTick;

		//User input
		const Uint8* keystate = SDL_GetKeyboardState(nullptr);

		//Holding W
		if (keystate[SDL_SCANCODE_W]) {

		}

		//Holding A
		if (keystate[SDL_SCANCODE_A]) {
			player.setCurrFrame(1);
			if (x_vel > -max_x_speed) //as long as we don't exceed max speed, change velocity
				x_vel = fmin(x_vel - acceleration, -max_x_speed);
		}

		//Holding S
		if (keystate[SDL_SCANCODE_S]) {
			player.setCurrFrame(0);
		}

		//Holding D
		if (keystate[SDL_SCANCODE_D]) {
			player.setCurrFrame(1);
			if (x_vel < max_x_speed) //as long as we don't exceed max speed, change velocity
				x_vel = fmax(x_vel + acceleration, max_x_speed);
		}

		//Holding Spacebar
		if (keystate[SDL_SCANCODE_SPACE]) {
			if (!in_air)	//only jump from ground
			{
				in_air = true;
				y_vel -= jump_strength;
			}
		}

		//Not holding side buttons
		if (!(keystate[SDL_SCANCODE_A] || keystate[SDL_SCANCODE_D])) {
			if (x_vel > 0) {
				x_vel = fmax(0, x_vel - acceleration);
			}
			else if (x_vel < 0) {
				x_vel = fmin(0, x_vel + acceleration);
			}
		}

		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) {
				running = false;
			}
		}

		bool on_solid = detectCollision(player, tilemap.getTileMap(), x_vel, y_vel);
		player.movePosition((int)(x_vel * delta_time), (int)(y_vel * delta_time));
		if (!on_solid && max_y_speed > y_vel) // while in air
		{
			y_vel += gravity;
			//player.setCurrFrame(1);
		}
		else if (on_solid)
		{
			in_air = false;
		}
		if (!in_air)
		{
			//player.setCurrFrame(0);
			y_vel = 0.0;
		}


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

		//Draw tilemap
		//tilemap.drawTileMap();

		//Temporarily manually draw platforms

		//Platform 1
		platformTile.getTileSprite()->draw(gRenderer, -rem_tile + 400, 480);
		platformTile.getTileSprite()->draw(gRenderer, -rem_tile + 416, 480);
		platformTile.getTileSprite()->draw(gRenderer, -rem_tile + 432, 480);
		platformTile.getTileSprite()->draw(gRenderer, -rem_tile + 448, 480);
		platformTile.getTileSprite()->draw(gRenderer, -rem_tile + 464, 480);
		platformTile.getTileSprite()->draw(gRenderer, -rem_tile + 480, 480);
		platformTile.getTileSprite()->draw(gRenderer, -rem_tile + 496, 480);
		platformTile.getTileSprite()->draw(gRenderer, -rem_tile + 512, 480);

		//Platform 2
		platformTile.getTileSprite()->draw(gRenderer, -rem_tile + 736, 480);
		platformTile.getTileSprite()->draw(gRenderer, -rem_tile + 752, 480);
		platformTile.getTileSprite()->draw(gRenderer, -rem_tile + 768, 480);
		platformTile.getTileSprite()->draw(gRenderer, -rem_tile + 784, 480);
		platformTile.getTileSprite()->draw(gRenderer, -rem_tile + 800, 480);
		platformTile.getTileSprite()->draw(gRenderer, -rem_tile + 816, 480);
		platformTile.getTileSprite()->draw(gRenderer, -rem_tile + 832, 480);
		platformTile.getTileSprite()->draw(gRenderer, -rem_tile + 848, 480);

		//Platform 3
		platformTile.getTileSprite()->draw(gRenderer, -rem_tile + 568, 390);
		platformTile.getTileSprite()->draw(gRenderer, -rem_tile + 584, 390);
		platformTile.getTileSprite()->draw(gRenderer, -rem_tile + 600, 390);
		platformTile.getTileSprite()->draw(gRenderer, -rem_tile + 616, 390);
		platformTile.getTileSprite()->draw(gRenderer, -rem_tile + 632, 390);
		platformTile.getTileSprite()->draw(gRenderer, -rem_tile + 648, 390);
		platformTile.getTileSprite()->draw(gRenderer, -rem_tile + 664, 390);
		platformTile.getTileSprite()->draw(gRenderer, -rem_tile + 680, 390);
	


		//draw the player
		if (x_vel > 0 && flip == SDL_FLIP_HORIZONTAL)
			flip = SDL_FLIP_NONE;
		else if (x_vel < 0 && flip == SDL_FLIP_NONE)
			flip = SDL_FLIP_HORIZONTAL;

		if(player.getFrameIndex() == 1)
			player.getCurrFrame().draw(gRenderer, player.getXPosition() - scroll_offset, player.getYPosition(), flip);
		else
			player.getCurrFrame().draw(gRenderer, player.getXPosition() - scroll_offset, player.getYPosition());

		drawHP();

		SDL_RenderPresent(gRenderer);
	}

}
