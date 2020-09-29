#include "entity.h"
#include <sstream>


Entity::Entity(std::string spriteData, int xPos, int yPos, int scale, SDL_Renderer* context) {

	this->spriteFile = std::ifstream(spriteData);
	this->context = context;
	this->s = scale;
	this->createSprites();
	this->currFrame = frames[0];
	this->x = xPos;
	this->y = yPos;
	this->index = 0;

}

Entity::~Entity(){
	
}

void Entity::setCurrFrame(int index){
	this->currFrame = frames[index];
	this->index = index;
}

Sprite Entity::getCurrFrame() {
	return currFrame;
}

int Entity::getFrameIndex() {
	return index;
}

double Entity::getXPosition()
{
	return this->x;
}


double Entity::getYPosition()
{
	return this->y;
}

void Entity::movePosition(double xf, double yf) {
	this->x += xf;
	this->y += yf;
}

void Entity::setPosition(double xPos, double yPos) {
	this->x = xPos;
	this->y = yPos;
}

void Entity::createSprites() {

	std::string line;
	std::getline(this->spriteFile, line);
	this->assetLoc = line;

	int a, b, c, d;

	while (this->spriteFile >> a >> b >> c >> d ) {
		Sprite temp(a, b, c, d, s, line, context);
		frames.emplace_back(temp);
	}
}
