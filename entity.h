#ifndef _ENTITY_H_
#define _ENTITY_H_

#include "sprite.h"
#include "physics.h"

#include <vector>
#include <iostream>
#include <fstream>

class Entity{
	
	public:
		Entity();
		Entity(std::string spriteData, double xPos, double yPos, int scale, int f, Physics* phys,SDL_Renderer* context);
		~Entity();
		
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

		void createSprites();
		
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
