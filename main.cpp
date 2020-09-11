#include <SDL.h>
#include <SDL_image.h>

#include "Game.h"


int main(int argc, char* argv[]) {
	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_PNG);

	Game g(1280, 720);

	g.runGame();

	SDL_Quit();
	return 0;
}
