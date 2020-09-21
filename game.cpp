#include <iostream>
#include "game.h"
#include "sprite.h"
#include "entity.h"
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
	Sprite base(1, 2, 14, 31, "assets/spritesheet.png", gRenderer);
	Sprite stick(0, 34, 16, 29, "assets/spritesheet.png", gRenderer);
	Sprite brick(16, 22, 16, 8, "assets/spritesheet.png", gRenderer);
		
	int x_pos = SCREEN_WIDTH / 2;
	int y_pos = SCREEN_HEIGHT / 2;

	int x_vel = 0;
	int y_vel = 0;
	
	int max_speed = 3;	//max velocity, prevents weird speed issues

	SDL_Event e;
	Sprite temp;
	temp = base;
	int index = 0;
	while (running == true) {
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) {
				running = false;
			}
			else if (e.type == SDL_KEYDOWN) {
				switch (e.key.keysym.sym) {
				case SDLK_w:
					if(y_vel > -max_speed)	//as long as we don't exceed max speed, change velocity
						y_vel -= 1;
					break;

				case SDLK_a:
					if (x_vel > -max_speed) //as long as we don't exceed max speed, change velocity
						x_vel -= 1;
					break;

				case SDLK_s:
					if (y_vel < max_speed) //as long as we don't exceed max speed, change velocity
						y_vel += 1;
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
					while(y_vel < 0)	//drift to 0 speed
						y_vel += 1;
					break;

				case SDLK_a:
					while(x_vel < 0)	//drift to 0 speed
						x_vel += 1;
					break;

				case SDLK_s:
					while(y_vel > 0)	//drift to 0 speed
						y_vel -= 1;
					break;

				case SDLK_d:
					while(x_vel > 0)	//drift to 0 speed
						x_vel -= 1;
					break;

				case SDLK_e:
					temp = stick;
					break;

				case SDLK_r:
					temp = base;
					break;
					
				}

			}
		}
		// Move box
		x_pos += x_vel;
		if (x_pos < 0)
			x_pos = 0;
		else if (x_pos + stick.getWidth() > SCREEN_WIDTH)	//if right edge of sprite hits screen edge
			x_pos = SCREEN_WIDTH - stick.getWidth();		//stop

		y_pos += y_vel;
		if (y_pos < 0)
			y_pos = 0;
		else if (y_pos + stick.getHeight() > SCREEN_HEIGHT)	//if bottom edge of sprite hits screen edge,
			y_pos = SCREEN_HEIGHT - stick.getHeight();		//stop

		SDL_RenderClear(gRenderer);
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
		temp.draw(gRenderer, x_pos, y_pos);
		for (int i = 0; i < 75; i++) {
			brick.draw(gRenderer,i*16,334);
		}
		SDL_RenderPresent(gRenderer);
		
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
