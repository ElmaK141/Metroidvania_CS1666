#ifndef _ENEMIES_H_
#define _ENEMIES_H_
#include "sprite.h"
#include "entity.h"
#include "tilemap.h"
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
		void update(Tilemap* t, double delta_time, double playerX, double playerY);

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
		
		int getState();
		int getCooldown();

		void setState(int s);
		void setCooldown(int c);

		bool playerIsNear(double playerX, double playerY);

		void takeDamage(double x, double y);

		Physics* getPhysics();
	private:
		
		bool collide(int** tilemap, double xVel, double yVel);
		void makeBrainway(Tilemap* t);
		int checkTile(int y, int x, int xMax);
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
		bool hasBrain;
		int state;
		int cooldown;
		int alternate;

		Sprite currFrame;
		SDL_Renderer* context;
		std::string assetLoc;
		std::ifstream spriteFile;
		std::vector<Sprite> frames;
		Physics* physics;
		int** brainway;
};
#endif