#include "entity.h"



Entity::Entity(std::vector<Sprite> f){
	this->frames = f;
	this->currFrame = f[0];
}

Entity::~Entity()
{
}

void Entity::setCurrFrame(int index){
	currFrame = frames[index];
}

