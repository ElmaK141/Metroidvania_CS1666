#include <SDL_image.h>
#include <iostream>

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
	SDL_Event e;
	while (running == true) {
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) {
				running = false;
			}
		}

		update();
		render();
		SDL_Delay(2500);

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
	temp = SDL_CreateTextureFromSurface(gRenderer, image);
	if (i + 1 < creditFiles.size()) {
		i++;
	}
	else {
		i = 0;
	}
	return temp;
}