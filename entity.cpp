#include "entity.h"
#include <sstream>


Entity::Entity(std::string spriteData, int xPos, int yPos, SDL_Renderer* context) {

	this->spriteFile = std::ifstream(spriteData);
	this->context = context;
	this->createSprites();
	this->currFrame = frames[0];
	this->x = xPos;
	this->y = yPos;

}

Entity::~Entity(){
	
}

void Entity::setCurrFrame(int index){
	this->currFrame = frames[index];
}

Sprite Entity::getCurrFrame() {
	return currFrame;
}

int Entity::getXPosition()
{
	return this->x;
}


int Entity::getYPosition()
{
	return this->y;
}

void Entity::movePosition(int xf, int yf) {
	this->x += xf;
	this->y += yf;
}

void Entity::setPosition(int xPos, int yPos) {
	this->x = xPos;
	this->y = yPos;
}

void Entity::createSprites() {

	std::string line;
	std::getline(this->spriteFile, line);
	this->assetLoc = line;

	int a, b, c, d;

	while (this->spriteFile >> a >> b >> c >> d ) {
		Sprite temp(a, b, c, d, 4, line, context);
		frames.emplace_back(temp);
	}
}