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

		int getXPosition();	
		int getYPosition();

		void movePosition(int xf, int yf);
		void setPosition(int xPos, int yPos);
	private:

		void createSprites();
		
		int x;
		int y;
		int s;

		Sprite currFrame;
		SDL_Renderer* context;
		std::string assetLoc;
		std::ifstream spriteFile;
		std::vector<Sprite> frames;


};






#endif