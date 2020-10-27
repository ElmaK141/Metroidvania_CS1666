#ifndef _ENEMIES_H_
#define _ENEMIES_H_
#include "sprite.h"
#include "entity.h"
#include <vector>
#include <iostream>
#include <fstream>

class Enemy {
	public:
		Enemy(std::string spriteData, double xPos, double yPos, int scale, int f, Physics* phys, SDL_Renderer* context);
		~Enemy();
		int getHP()
		{
			return hp;
		}
		int getDamage()
		{
			return damage;
		}
		void update(int** tilemap, double delta_time, double playerX, double playerY);

		void setCurrFrame(int index);
		Sprite getCurrFrame();
		int getFrameIndex();

		double getXPosition();
		double getYPosition();

		void movePosition(double xf, double yf);
		void setPosition(double xPos, double yPos);

		void setXVel(double xVelo);
		void setYVel(double yVelo);
		double getXVel();
		double getYVel();

		void setJump(bool jump);
		bool getJump();

		int getFlag();

		Physics* getPhysics();
	private:
		
		void collide(int** tilemap, double xVel, double yVel);
		void createSprites();
		

		int hp;
		int maxSpeed;
		int damage;
		bool active;
		double accel;
		

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