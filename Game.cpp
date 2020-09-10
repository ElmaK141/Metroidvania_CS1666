#include <SDL_image.h>
#include <iostream>
#include <vector>

#include "Game.h"


Game::Game(int width, int height)
{
	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_PNG);
	
	windowWidth = width;
	windowHeight = height;

	gWindow = SDL_CreateWindow("METROIDVANIA", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		width, height, 0);

	gRenderer = SDL_CreateRenderer(gWindow, -1, 0);
	running = true;
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
	SDL_Event e;
	while (running == true) {
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) {
				running = false;
			}
		}

		update();
		render();

	}
}

void Game::update()
{
}

void Game::render()
{
	SDL_Surface* credits = IMG_Load("credit_pics/greenland.png");
	std::string error = SDL_GetError();
	SDL_Texture* image = SDL_CreateTextureFromSurface(gRenderer, credits);
	SDL_RenderClear(gRenderer);
	SDL_RenderCopy(gRenderer, image, NULL, NULL);
	SDL_RenderPresent(gRenderer);

}

void Game::rollCredits()
{
	std::vector<SDL_Texture*> credits;
	SDL_Surface* load = IMG_LoadTexture(")
}
