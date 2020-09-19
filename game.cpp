#include <iostream>
#include "game.h"


int SCREEN_WIDTH;
int SCREEN_HEIGHT;
constexpr int BOX_WIDTH = 20;
constexpr int BOX_HEIGHT = 20;

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

Game::Game(const Game & obj)
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
	//Roll credits
	for (int credit_image = 0; credit_image < creditFiles.size(); credit_image++) {
		update();
		render();
//		SDL_Delay(2500);
	}

	// Current position to render the box
	// Start off with it in the middle
	int x_pos = SCREEN_WIDTH / 2 - BOX_WIDTH / 2;
	int y_pos = SCREEN_HEIGHT / 2 - BOX_HEIGHT / 2;

	// Current velocity of the box
	// Start off at reset
	int x_vel = 0;
	int y_vel = 0;

	//Actiual Game
	SDL_Event e;
	while (running == true) {
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) {
				running = false;
			}
			else if (e.type == SDL_KEYDOWN) {
				switch (e.key.keysym.sym) {
				case SDLK_w:
					y_vel -= 1;
					break;

				case SDLK_a:
					x_vel -= 1;
					break;

				case SDLK_s:
					y_vel += 1;
					break;

				case SDLK_d:
					x_vel += 1;
					break;
				}
			}
		}
		// Move box
		x_pos += x_vel;
		y_pos += y_vel;

		// Draw box
		// Clear black
		SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
		SDL_RenderClear(gRenderer);
		// Cyan box
		SDL_SetRenderDrawColor(gRenderer, 0x00, 0xFF, 0xFF, 0xFF);
		SDL_Rect fillRect = { x_pos, y_pos, BOX_WIDTH, BOX_HEIGHT };
		SDL_RenderFillRect(gRenderer, &fillRect);
		SDL_RenderPresent(gRenderer);
	}
}

void Game::update()
{
}

void Game::render()
{
	SDL_RenderClear(gRenderer);
	SDL_Texture* temp = rollCredits();
	SDL_RenderCopy(gRenderer, temp, NULL, NULL);
	SDL_RenderPresent(gRenderer);
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
