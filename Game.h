#ifndef _GAME_H_
#define _GAME_H_
#include <SDL.h>
#include <string>

class Game {
	public:
		Game(int width, int height);
		~Game();
		void runGame();

	private:
		SDL_Window* gWindow;
		SDL_Renderer* gRenderer;
		int windowWidth;
		int windowHeight;
		bool running;

		void update();
		void render();
		void rollCredits();
};
#endif
