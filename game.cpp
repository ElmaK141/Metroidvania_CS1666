#include <iostream>
#include <cmath>
#include "game.h"
#include "sprite.h"
#include "entity.h"
#include "tile.h"
#include "background.h"

// for now - define the X length of the gameworld
constexpr int LEVEL_LEN = 3000;

//size of the Window/Screen and thus the size of the Camera
int SCREEN_WIDTH;
int SCREEN_HEIGHT;
bool in_air = true;
double gravity = 0.01;
int jump_strength = 3;

Game::Game(int width, int height)
{
	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_PNG);
	
	windowWidth = width;
	windowHeight = height;
	
	SCREEN_WIDTH = width;
	SCREEN_HEIGHT = height;

	gWindow = SDL_CreateWindow("METROIDVANIA", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
	width, height, 0);

	gRenderer = SDL_CreateRenderer(gWindow, -1, 0);
	running = true;
}

Game::Game(const Game &obj)
{
}

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
	maxHP = 40;
	playerHP = 40;
	SDL_Event e;
	while (true) {	//main menu

		SDL_PollEvent(&e);
		if (e.type == SDL_QUIT) {
			running = false;
		}
		else if (e.type == SDL_KEYDOWN)
			break;

		SDL_RenderClear(gRenderer);
		SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0x00);	//black background

		mmTitle.draw(gRenderer, 240, 120);
		if(SDL_GetTicks() % 1200 <= 700)	//blinking text
			mmPressAny.draw(gRenderer, 397, 400);

		SDL_RenderPresent(gRenderer);
	}

	// variables for background scrolling
	int scroll_offset = 0;
	int rem = 0;

	// Define where the Camera Thirds are for knowing when we should scroll
	int lthird = (SCREEN_WIDTH / 3);
	int rthird = (2 * SCREEN_WIDTH / 3);

	// determine players starting position (middle of background, on the left side of world)
	int x_pos = SCREEN_WIDTH / 2;
	int y_pos = SCREEN_HEIGHT / 2 - 145;
	
	//Define Graphical Objects
	Background bg(0, 0, 1280, 720, "assets/backgrounds/background1.png", gRenderer);
	Entity player("data/player.spr", x_pos, y_pos, gRenderer);

	double x_vel = 0;
	double y_vel = 0;
	
	int max_speed = 3;	//max velocity, prevents weird speed issues
	
	int index = 0;
	while (running == true) {
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) {
				running = false;
			}
			else if (e.type == SDL_KEYDOWN) {
				switch (e.key.keysym.sym) {

				case SDLK_w:
					if (!in_air)	//only jump from ground
					{
						in_air = true;
						y_vel -= jump_strength;
					}
					break;

				case SDLK_a:
					if (x_vel > -max_speed) //as long as we don't exceed max speed, change velocity
						x_vel -= 1;
					break;

				case SDLK_s:
					/*
					if (y_vel < max_speed) //as long as we don't exceed max speed, change velocity
						y_vel += 1;
					*/
					break;

				case SDLK_d:
					if (x_vel < max_speed) //as long as we don't exceed max speed, change velocity
						x_vel += 1;
					break;
				}
			}
			else if (e.type == SDL_KEYUP) {
				switch (e.key.keysym.sym) {
				case SDLK_w:
					/*
					while(y_vel < 0)	//drift to 0 speed
						y_vel += 1;
					*/
					break;

				case SDLK_a:
					while (x_vel < 0)	//drift to 0 speed
						x_vel += 1;
					break;

				case SDLK_s:
					/*
					while(y_vel > 0)	//drift to 0 speed
						y_vel -= 1;
					*/
					break;

				case SDLK_d:
					while (x_vel > 0)	//drift to 0 speed
						x_vel -= 1;
					break;

				case SDLK_e:
					break;

				case SDLK_r:
					break;

				}

			}
		}

		player.movePosition((int)x_vel, (int)y_vel);
		bool on_solid = detectCollision(player);
		if (!on_solid && max_speed > y_vel) // while in air
		{
			y_vel += gravity;
			player.setCurrFrame(1);
		}
		else if (on_solid)
		{
			in_air = false;
		}
		if (!in_air)
		{
			player.setCurrFrame(0);
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
		rem = scroll_offset % SCREEN_WIDTH;
		bg.getSprite()->draw(gRenderer, -rem, 0);
		bg.getSprite()->draw(gRenderer, (-rem + SCREEN_WIDTH), 0);

		//draw the player
		player.getCurrFrame().draw(gRenderer, player.getXPosition() - scroll_offset, player.getYPosition());

		drawHP();

		SDL_RenderPresent(gRenderer);
	}
}


// Detect collision of Entity with Gameworld (edge of screen)
bool Game::detectCollision(Entity& ent)
{
	if (ent.getXPosition() < 0) {
		ent.setPosition(0, ent.getYPosition());
	}
	if (ent.getYPosition() < 0) {
		ent.setPosition(ent.getXPosition(), 0);
	}
	if (ent.getXPosition() + ent.getCurrFrame().getWidth() > LEVEL_LEN) {
		ent.setPosition(LEVEL_LEN - ent.getCurrFrame().getWidth(), ent.getYPosition());
	}
	if (ent.getYPosition() + ent.getCurrFrame().getHeight() > SCREEN_HEIGHT) {
		ent.setPosition(ent.getXPosition(), SCREEN_HEIGHT - ent.getCurrFrame().getHeight());
		return true;
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

