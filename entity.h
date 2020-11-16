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

		bool getShot();
		void setShot(bool val);
		void setPX(double px);
		void setPY(double py);
		double getPX();
		double getPY();
		void setPVelX(double py);
		void setPVelY(double py);
		double getPVelY();
		double getPVelX();
		Physics* getPhysics();

		void setDouble();
		void setGrapple();
		bool getDouble();
		bool getGrapple();
	private:

		void createSprites();
		
		double x;
		double y;
		int s;
		int index;

		int flag;				// Flag determines what kind of entity this is
								// 0: Player flag
								// 1: Enemy flag (eye)
								// 2-5: Teleporters (Main, Sec1, Sec2, Boss)
		
		double xVel;
		double yVel;
		bool canJump;
		// For projectile
		bool canShoot;
		double px;
		double py;
		double pvelx;
		double pvely;

		// powerup flags - these are unique to the player
		bool hasDouble;
		bool hasGrapple;

		Sprite currFrame;
		SDL_Renderer* context;
		std::string assetLoc;
		std::ifstream spriteFile;
		std::vector<Sprite> frames;
		Physics* physics;

};






#endif
