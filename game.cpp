#include <iostream>
#include "game.h"
#include "sprite.h"
<<<<<<< HEAD
=======
#include "entity.h"
#include "tile.h"
#include "background.h"
>>>>>>> upstream/master
#include <cmath>

int SCREEN_WIDTH;
int SCREEN_HEIGHT;

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
	
	//Define Sprites
	Sprite sbg(0, 0, 1280, 720, 1, "assets/backgrounds/background1.png", gRenderer);
	Background bg(&sbg);

	int x_pos = SCREEN_WIDTH / 2;
	int y_pos = SCREEN_HEIGHT / 2 - 145;

	
	Entity player("data/player.spr", x_pos, y_pos, gRenderer);


	int x_vel = 0;
	int y_vel = 0;
	
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
					/*
					if(y_vel > -max_speed)	//as long as we don't exceed max speed, change velocity
						y_vel -= 1;
					*/
					//jump
					/*if(SCREEN_HEIGHT - player.getCurrFrame().getHeight() == y_pos){
						y_vel = -25;
					}
					player.setCurrFrame(1);
					*/
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
					while(x_vel < 0)	//drift to 0 speed
						x_vel += 1;
					break;

				case SDLK_s:
					/*
					while(y_vel > 0)	//drift to 0 speed
						y_vel -= 1;
					*/
					break;

				case SDLK_d:
					while(x_vel > 0)	//drift to 0 speed
						x_vel -= 1;
					break;

				case SDLK_e:
					break;

				case SDLK_r:
					break;
					
				}

			}
		}
		
		player.movePosition(x_vel, y_vel);

		detectCollision(player);

		//Draw to screen
		SDL_RenderClear(gRenderer);
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
		player.getCurrFrame().draw(gRenderer, player.getXPosition(), player.getYPosition());

		bg.getSprite()->draw(gRenderer, 0, 0);
		SDL_RenderPresent(gRenderer);
	}
}

void Game::detectCollision(Entity &ent)
{
	if (ent.getXPosition() < 0) {
		ent.setPosition(0,ent.getYPosition());
	}
	else if (ent.getYPosition() < 0) {
		ent.setPosition(ent.getXPosition(), 0);
	}
	else if (ent.getXPosition() + ent.getCurrFrame().getWidth() > SCREEN_WIDTH) {
		ent.setPosition(SCREEN_WIDTH - ent.getCurrFrame().getWidth(), ent.getYPosition());
	}
	else if (ent.getYPosition() + ent.getCurrFrame().getHeight() > SCREEN_HEIGHT ) {
		ent.setPosition(ent.getXPosition(),SCREEN_HEIGHT - ent.getCurrFrame().getHeight());
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

