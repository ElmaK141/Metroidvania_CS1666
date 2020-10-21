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

#include "entity.h"
#include "tilemap.h"
#include "enemies.h"

class Game {
	public:
		Game(int width, int height);
		//Game(const Game &obj);
		~Game();
		void gameLoop();

		bool detectCollision(Entity &ent, int** t, double x_vel, double y_vel);
		bool checkDoor(int room, double vel, Entity& ent);
	private:
		SDL_Window* gWindow;
		SDL_Renderer* gRenderer;
		int windowWidth;
		int windowHeight;
		int maxHP;
		int playerHP;
		bool running;
		std::vector<std::string> creditFiles = { "assets/credit_pics/AdamBuchinskyCredits.png","credit_pics/GiovanniMeiserbeegyosh.png","credit_pics/juliancredits.png","assets/credit_pics/NoahVienneau.jpg","assets/credit_pics/emk113_credit_image.png"

													,"assets/credit_pics/hicksCredits.png","assets/credit_pics/JoelValentinoCredits.png","assets/credit_pics/JordanBendercredits.png", "assets/credit_pics/KristoferE.png"};
		void drawHP();
		void update();
		void render();
		SDL_Texture* rollCredits();
		void loadStartScreen();	//Loads the start screen
		void loadMainMenu();	//Loads the main menu
		void loadDeathScreen(); //Player dies
		void pauseMenu(int prevGameState);		//Pause game and bring up Pause menu
		void runGame();			//Run the game
		void runDebug();		//Run debug
		void getUserInput(Entity* player);		//Handle input from the user
		void handleCollision(Entity* player, Tilemap* t);	//Handles collision events of the player
		bool checkHitPlayer(Entity* player, Enemy* enemy);
};
#endif
