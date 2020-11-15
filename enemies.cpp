#include "enemies.h"
#include <sstream>
#include <cmath>

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
	this->accel = 0;
	this->damage = 0;

	//Player
	if (this->flag == 1) {
		//Initialize Velocity
		this->xVel = 0.0;
		this->yVel = 0.0;
		this->accel = 5;
		this->damage = 7;

		this->hp = 35;
		this->maxSpeed = 50;
		this->active = true;
	}
}

Enemy::~Enemy() {

}

void Enemy::setCurrFrame(int index) {
	this->currFrame = frames[index];
	this->index = index;
}

Sprite Enemy::getCurrFrame() {
	return currFrame;
}

int Enemy::getFrameIndex() {
	return index;
}

double Enemy::getXPosition()
{
	return this->x;
}


double Enemy::getYPosition()
{
	return this->y;
}

void Enemy::movePosition(double xf, double yf) {
	this->x += xf;
	this->y += yf;
}

void Enemy::setPosition(double xPos, double yPos) {
	this->x = xPos;
	this->y = yPos;
}

void Enemy::createSprites() {

	std::string line;
	std::getline(this->spriteFile, line);
	this->assetLoc = line;

	int a, b, c, d;

	while (this->spriteFile >> a >> b >> c >> d) {
		Sprite temp(a, b, c, d, s, line, context);
		frames.emplace_back(temp);
	}
}

int Enemy::getFlag() {
	return this->flag;
}

Physics* Enemy::getPhysics()
{
	return this->physics;
}

void Enemy::setXVel(double xVelo) {
	this->xVel = xVelo;
}

void Enemy::setYVel(double yVelo) {
	this->yVel = yVelo;
}

double Enemy::getXVel() {
	return this->xVel;
}

double Enemy::getYVel() {
	return this->yVel;
}

void Enemy::setJump(bool jump) {
	this->canJump = jump;
}

bool Enemy::getJump() {
	return this->canJump;
}

void Enemy::update(int** tilemap, double delta_time, double playerX, double playerY)
{
	//get velocities
	double xVel = getXVel();
	double yVel = getYVel();
	//Eye
	if (this->flag == 1) {
		
		if (playerX + 7 < this->x)
			xVel -= accel * delta_time;
		else if(playerX + 7 > this->x)
			xVel += accel * delta_time;

		if (playerY + 26 < this->y)
			yVel -= accel * delta_time;
		else if (playerY + 26 > this->y)
			yVel += accel * delta_time;

		if (xVel > this->maxSpeed)
			xVel = maxSpeed;
		else if (xVel < -this->maxSpeed)
			xVel = -maxSpeed;

		if (yVel > this->maxSpeed)
			yVel = maxSpeed;
		else if (yVel < -this->maxSpeed)
			yVel = -maxSpeed;

		setXVel(xVel);
		setYVel(yVel);

		collide(tilemap, xVel, yVel);
	}
}

void Enemy::collide(int** tilemap, double xVel, double yVel)
{
	double ePosY = getYPosition();
	double ePosX = getXPosition();
	int eHeight = getCurrFrame().getHeight();
	int eWidth = getCurrFrame().getWidth();		//get player height, width, & positions

	int yBlockD = (int)((ePosY + eHeight) / 16) + 1;
	int yBlockU = (int)(ePosY / 16) - 1;
	int xBlockL = (int)(ePosX / 16) - 1;					//gather adjacent block locations
	int xBlockR = (int)((ePosX + eWidth) / 16) + 1;		//including left, right, up, down
	bool land = false;			//determines if you've landed on something			

	setPosition(getXPosition() + xVel, getYPosition() + yVel);	//set position

	if (yVel >= 0) {
		for (int xAdjust = 1; xAdjust < xBlockR - xBlockL; xAdjust++)
		{	//for every block under the player's width
			if (yBlockD <= 44)
			{	//if you'd pass a solid block
				for (int range = ePosY; range <= getYPosition(); range += 16) //for EVERY block that is passed during movement
				{
					yBlockD = (int)(((range + eHeight) / 16)) + 1;
					if (range + eHeight + yVel > yBlockD * 16 - 1 && (tilemap[yBlockD][xBlockR - xAdjust] != 0 && tilemap[yBlockD][xBlockR - xAdjust] != 3))
					{	//set position to above the block
						setPosition(getXPosition(), yBlockD * 16 - eHeight - 1);
						if (getFlag() == 1)
							setYVel(-yVel + yVel/6);
						ePosY = getYPosition();
						land = true;
						break;
					}
				}
				if (land)
					break;
			}
			else //if you're trying to fall through the bottom of the screen
			{
				setPosition(getXPosition(), 720 - eHeight - 17);
				if(getFlag() == 1)
					setYVel(-yVel + yVel/6);
				ePosY = getYPosition();
				land = true;
				break;
			}
		}
	}

	bool bonk = false;
	if (yVel < 0) {
		for (int xAdjust = 1; xAdjust < xBlockR - xBlockL; xAdjust++)
		{	//bonk head on blocks above
			if (yBlockU >= 0)
			{
				for (int range = ePosY; range >= getYPosition(); range -= 16) //for EVERY block that is passed during movement
				{
					yBlockU = (int)(range / 16) - 1;
					if (range + yVel <= yBlockU * 16 + 16 && (tilemap[yBlockU][xBlockR - xAdjust] != 0 && tilemap[yBlockU][xBlockR - xAdjust] != 3))
					{
						setPosition(getXPosition(), yBlockU * 16 + 16);
						ePosY = getYPosition();
						setYVel(0);
						if (getFlag() == 1)
							setYVel(-yVel + yVel/6);
						bonk = true;
						break;
					}
				}
				if (bonk)
					break;
			}
			else
			{
				setPosition(getXPosition(), 17);
				if (getFlag() == 1)
					setYVel(-yVel + yVel/6);
				ePosY = getYPosition();
				land = true;
				break;
			}
		}
	}

	if (xVel > 0) {
		for (int yAdjust = 1; yAdjust < yBlockD - yBlockU; yAdjust++)
		{	//hit blocks to your right accounting for player height
			if (ePosX + eWidth + xVel >= xBlockR * 16 - 1 && (tilemap[yBlockD - yAdjust][xBlockR] != 0 && tilemap[yBlockD - yAdjust][xBlockR] != 3))
			{
				setPosition(xBlockR * 16 - eWidth - 1, getYPosition());
				if (getFlag() == 1)
					setXVel(-xVel + xVel/6);
				ePosX = getXPosition();
				break;
			}
		}
	}

	if (xVel < 0) {
		for (int yAdjust = 1; yAdjust < yBlockD - yBlockU; yAdjust++)
		{	//hit blocks to your left accounting for player height
			if (ePosX + xVel <= xBlockL * 16 + 16 && (tilemap[yBlockD - yAdjust][xBlockL] != 0 && tilemap[yBlockD - yAdjust][xBlockL] != 3))
			{
				setPosition(xBlockL * 16 + 16, getYPosition());
				if (getFlag() == 1)
					setXVel(-xVel + xVel/6);
				ePosX = getXPosition();
				break;
			}
		}
	}
	if(xVel < 0 && yVel < 0)
		setCurrFrame(0);
	else if (xVel < 0 && yVel >= 0)
		setCurrFrame(1);
	else if (xVel >= 0 && yVel >= 0)
		setCurrFrame(2);
	else if (xVel >= 0 && yVel < 0)
		setCurrFrame(3);
	
}

void Enemy::takeDamage(double xVector, double yVector)
{
	setXVel(xVel + xVector);
	setYVel(yVel + yVector);
	hp -= 5;
}
