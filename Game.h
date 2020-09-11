#ifndef _GAME_H_
#define _GAME_H_
#include <SDL.h>
#include <string>
#include <vector>

class Game {
	public:
		Game(int width, int height);
		Game(const Game &obj);
		~Game();
		void runGame();

	private:
		SDL_Window* gWindow;
		SDL_Renderer* gRenderer;
		int windowWidth;
		int windowHeight;
		bool running;
		std::vector<std::string> creditFiles = { "credit_pics/AdamBuchinskyCredits.png","credit_pics/GiovanniMeiserbeegyosh.png","credit_pics/juliancredits.png","credit_pics/NoahVienneau.jpg" };

		void update();
		void render();
		SDL_Texture* rollCredits();
};
#endif
