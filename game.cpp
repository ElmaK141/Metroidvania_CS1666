#include <iostream>
#include <cmath>
#include "game.h"
#include "sprite.h"
#include "entity.h"
#include "tile.h"
#include "background.h"

int SCREEN_WIDTH;
int SCREEN_HEIGHT;
bool in_air = true;
double gravity = 0.09;
int jump_strength = 5;

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


	int x_pos = SCREEN_WIDTH / 2;
	int y_pos = SCREEN_HEIGHT / 2 - 145;
	
	//Define Graphiical Objects
	Background bg(0, 0, 1280, 720, "assets/backgrounds/background1.png", gRenderer);
	Entity player("data/player.spr", x_pos, y_pos, gRenderer);
	
	Sprite wall(98,96,16,16,4,"assets/sprites/tiles.png",gRenderer);
	Sprite floor(435, 94, 16, 16, 4, "assets/sprites/tiles.png", gRenderer);

	std::vector<Sprite> tiles;
	tiles.push_back(wall);
	tiles.push_back(floor);


	double x_vel = 0;
	double y_vel = 0;
	
	int max_speed = 3;	//max velocity, prevents weird speed issues
	
	Tilemap t("data/tilemap.txt", gRenderer,tiles);

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

		//Draw to screen
		SDL_RenderClear(gRenderer);
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
		
		bg.getSprite()->draw(gRenderer, 0, 0);

		

		player.getCurrFrame().draw(gRenderer, player.getXPosition(), player.getYPosition());

		drawHP();

		SDL_RenderPresent(gRenderer);
	}
}


bool Game::detectCollision(Entity& ent)
{

	if (ent.getXPosition() < 0) {
		ent.setPosition(0, ent.getYPosition());
	}
	if (ent.getYPosition() < 0) {
		ent.setPosition(ent.getXPosition(), 0);
	}
	if (ent.getXPosition() + ent.getCurrFrame().getWidth() > SCREEN_WIDTH) {
		ent.setPosition(SCREEN_WIDTH - ent.getCurrFrame().getWidth(), ent.getYPosition());
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

