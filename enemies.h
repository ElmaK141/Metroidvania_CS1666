#ifndef _ENEMIES_H_
#define _ENEMIES_H_
#include "entity.h"

class Enemy : public Entity {
	public:
		Enemy(std::string spriteData, double xPos, double yPos, int scale, int f, Physics* phys, SDL_Renderer* context);
		~Enemy();
		int getHP()
		{
			return hp;
		}
		bool touchPlayer();
	private:
		void createSprites();

		int hp;
		int maxSpeed;
		bool active;
		double x;
		double y;
		int s;
		int index;
		int flag;
		double xVel;
		double yVel;
		bool canJump;

		Sprite currFrame;
		SDL_Renderer* context;
		std::string assetLoc;
		std::ifstream spriteFile;
		std::vector<Sprite> frames;
		Physics* physics;
};
#endif