#ifndef _ENTITY_H_
#define _ENTITY_H_

#include "sprite.h"

#include <vector>

class Entity{
	
	public:
		Entity(std::vector<Sprite> f);
		~Entity();
		
		void setCurrFrame(int index);
		Sprite currFrame;
	private:
		std::vector<Sprite> frames;
};





#endif