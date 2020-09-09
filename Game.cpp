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

	SDL_Texture* image = NULL;
	SDL_Surface* loadSurface = IMG_Load("");
	if (loadSurface == NULL) {
		std::string error = SDL_GetError();
		std::cout << "Image Load Failed" << std::endl;
	}

	image = SDL_CreateTextureFromSurface(gRenderer, loadSurface);
	SDL_RenderClear(gRenderer);
	SDL_RenderCopy(gRenderer, image, NULL, NULL);
	SDL_RenderPresent(gRenderer);

}
