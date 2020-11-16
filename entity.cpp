#include "entity.h"
#include <sstream>

Entity::Entity() {}

Entity::Entity(std::string spriteData, double xPos, double yPos, int scale, int f, Physics* phys, SDL_Renderer* context) {

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
	this->canShoot = true;
	
	//Player
	if (this->flag == 0) {
		//Initialize Velocity
		this->xVel = 0.0;
		this->yVel = 0.0;

		// set initial flags for powerups
		this->hasDouble = false;
		this->hasGrapple = false;
		this->bonusHealth = 0;

		//Can jump initially
		this->canJump = true;
		this->canShoot = true;

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

void Entity::setFlag(int f) {
	this->flag = f;
}

Physics* Entity::getPhysics()
{
	return this->physics;
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




bool Entity::getShot() {
	return this->canShoot;
}

void Entity::setShot(bool canShoot) {
	this->canShoot = canShoot;
}
void Entity::setPX(double px) {
	this->px = px;
}

void Entity::setPY(double py) {
	this->py = py;
}

double Entity::getPX() {
	return this->px;
}

double Entity::getPY() {
	return this->py;
}

void Entity::setPVelX(double pvelx) {
	this->pvelx = pvelx;
}

void Entity::setPVelY(double pvely) {
	this->pvely = pvely;
}

double Entity::getPVelY() {
	return this->pvely;
}

double Entity::getPVelX() {
	return this->pvelx;
}

// sets the double jump flag true
void Entity::setDouble() {
	this->hasDouble = true;
}

// sets the graple flag true
void Entity::setGrapple() {
	this->hasGrapple = true;
}

// increases "bonus health" by 1 unit;
void Entity::increaseHealth() {
	this->bonusHealth++;
}

// gets the status of hasDouble
bool Entity::getDouble() {
	return this->hasDouble;
}

// gets the status of hadGrapple
bool Entity::getGrapple() {
	return this->hasGrapple;
}

// gets the status of the player's bonus health powerups
int Entity::getBonusHealth() {
	return this->bonusHealth;
}