#ifndef _GAME_H_
#define _GAME_H_

#ifdef __linux__
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#elif _WIN32
#include <SDL.h>
#include <SDL_image.h>
#endif
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
		std::vector<std::string> creditFiles = { "credit_pics/AdamBuchinskyCredits.png","credit_pics/GiovanniMeiserbeegyosh.png","credit_pics/juliancredits.png","credit_pics/NoahVienneau.jpg","credit_pics/emk113_credit_image.png"

													,"credit_pics/hicksCredits.png","credit_pics/JoelValentinoCredits.png","credit_pics/JordanBendercredits.png", "credit_pics/KristoferE.png"};

		void update();
		void render();
		SDL_Texture* rollCredits();
};
#endif
