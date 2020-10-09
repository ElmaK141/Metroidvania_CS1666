#include "entity.h"
#include <sstream>


Entity::Entity(std::string spriteData, double xPos, double yPos, int scale, int f, SDL_Renderer* context) {

	this->spriteFile = std::ifstream(spriteData);
	this->context = context;
	this->s = scale;
	this->createSprites();
	this->currFrame = frames[0];
	this->x = xPos;
	this->y = yPos;
	this->index = 0;
	this->flag = f;
	
	//Player
	if (this->flag == 0) {
		//Initialize Velocity
		this->xVel = 0.0;
		this->yVel - 0.0;

		//Can jump initially
		this->canJump = true;
	}
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

int Entity::getFlag() {
	return this->flag;
}

void Entity::setXVel(double xVelo) {
	this->xVel = xVelo;
}

void Entity::setYVel(double yVelo) {
	this->yVel = yVelo;
}

double Entity::getXVel() {
	return this->xVel;
}

double Entity::getYVel() {
	return this->yVel;
}

void Entity::setJump(bool jump) {
	this->canJump = jump;
}

bool Entity::getJump() {
	return this->canJump;
}