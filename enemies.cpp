#include "enemies.h"
#include <sstream>

Enemy::Enemy(std::string spriteData, double xPos, double yPos, int scale, int f, Physics* phys, SDL_Renderer* context) {

	this->spriteFile = std::ifstream(spriteData);
	this->context = context;
	this->s = scale;
	this->createSprites();
	this->currFrame = frames[0];
	this->x = xPos;
	this->y = yPos;
	this->index = 0;
	this->flag = f;
	this->physics = phys;

	//Player
	if (this->flag == 1) {
		//Initialize Velocity
		this->xVel = 0.0;
		this->yVel = 0.0;

		this->hp = 35;
		this->maxSpeed = 20;
		this->active = false;
	}
}

Enemy::~Enemy() {

}

bool Enemy::touchPlayer()
{
	return false;
}

void Enemy::createSprites() 
{
	std::string line;
	std::getline(this->spriteFile, line);
	this->assetLoc = line;

	int a, b, c, d;

	while (this->spriteFile >> a >> b >> c >> d) {
		Sprite temp(a, b, c, d, s, line, context);
		frames.emplace_back(temp);
	}
}