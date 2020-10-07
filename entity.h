#ifndef _ENTITY_H_
#define _ENTITY_H_

#include "sprite.h"

#include <vector>
#include <iostream>
#include <fstream>

class Entity{
	
	public:
		Entity(std::string spriteData, int xPos, int yPos, int scale, SDL_Renderer* context);
		~Entity();
		
		void setCurrFrame(int index);
		Sprite getCurrFrame();
		int getFrameIndex();

		double getXPosition();	
		double getYPosition();

		void movePosition(double xf, double yf);
		void setPosition(double xPos, double yPos);
	private:

		void createSprites();
		
		double x;
		double y;
		int s;
		int index;

		Sprite currFrame;
		SDL_Renderer* context;
		std::string assetLoc;
		std::ifstream spriteFile;
		std::vector<Sprite> frames;


};






#endif
