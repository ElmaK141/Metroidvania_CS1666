#ifndef _ENTITY_H_
#define _ENTITY_H_

#include "sprite.h"

#include <vector>
#include <iostream>
#include <fstream>

class Entity{
	
	public:
		Entity(std::string spriteData, SDL_Renderer* context);
		~Entity();
		
		void setCurrFrame(int index);
		Sprite getCurrFrame();
	private:

		void createSprites();
		
		Sprite currFrame;
		SDL_Renderer* context;
		std::string assetLoc;
		std::ifstream spriteFile;
		std::vector<Sprite> frames;
};





#endif