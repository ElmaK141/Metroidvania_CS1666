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
	this->state = 0;
	this->cooldown = 0;
	this->alternate = 0;

	//Eye
	if (this->flag == 1) {
		//Initialize Velocity
		this->xVel = 0.0;
		this->yVel = 0.0;
		this->accel = 5;
		this->damage = 6;

		this->hp = 35;
		this->maxSpeed = 50;
		this->active = true;
	}
	else if (this->flag == 0) { //bossman
		//Initialize Velocity
		this->xVel = 0.0;
		this->yVel = 0.0;
		this->accel = 5;
		this->damage = 23;

		this->hp = 420;
		this->maxSpeed = 50;
		this->active = true;
	}
	// Chaser
	else if (this->flag == 2)
	{
		this->xVel = 0.0;
		this->yVel = 0.0;
		this->accel = 5;
		this->damage = 5;
		this->canJump = false;

		this->hp = 65;
		this->maxSpeed = 15;
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

int Enemy::getState()
{
	return this->state;
}

int Enemy::getCooldown()
{
	return this->cooldown;
}

void Enemy::setState(int s)
{
	this->state = s;
}

void Enemy::setCooldown(int c)
{
	this->cooldown = c;
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

void Enemy::update(Tilemap* t, double delta_time, double playerX, double playerY)
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

		collide(t->getTileMap(), xVel, yVel);
	}
	else if (this->flag == 0) //bossman
	{
		if (this->alternate < 2)
		{
			setCurrFrame(0);
			this->alternate++;
		}
		else if (alternate < 4)
		{
			setCurrFrame(1);
			this->alternate++;
		}
		else
		{
			this->alternate = 0;
		}
		if (getCooldown() > 0)
			setCooldown(fmax(0, getCooldown() - delta_time));
		if (getState() == 3 && getCooldown() <= 200)
		{
			setPosition(getXPosition() + 96, getYPosition());
			this->state = 0;
		}
		if ((getCooldown() == 0 && playerIsNear(playerX, playerY)) || getCooldown() > 200)
		{
			this->alternate = 0;
			this->state = 3;
			
			setCurrFrame(3);
			if (getCooldown() == 0)
			{
				this->setPosition(getXPosition() - 96, getYPosition());
				setCooldown(250);
			}
		}
	}
	// Chaser
	else if (this->flag == 2)
	{
		if (!(this->hasBrain))
		{
			makeBrainway(t);
			this->hasBrain = true;
		}

		if (this->getHP() > 25)
		{
			if (playerX + 7 < this->x)
			{
				if (xVel > 0)
					xVel = 1.5;
				xVel -= accel * delta_time;
				setCurrFrame(0);
			}
			else if (playerX + 7 > this->x)
			{
				if (xVel < 0)
					xVel = -1.5;
				xVel += accel * delta_time;
				setCurrFrame(1);
			}
		}
		else
		{
			if (playerX + 7 < this->x)
			{
				if (xVel < 0)
					xVel = -1.5;
				xVel += accel * delta_time;
				setCurrFrame(3);
			}
			else if (playerX + 7 > this->x)
			{
				if (xVel > 0)
					xVel = 1.5;
				xVel -= accel * delta_time;
				setCurrFrame(2);
			}
		}

		if (!canJump)
			yVel += (accel * 2) * delta_time;

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

		this->canJump = collide(t->getTileMap(), xVel, yVel);
		double ePosY = getYPosition();
		double ePosX = getXPosition();
		int eHeight = getCurrFrame().getHeight();

		int alignedX = ePosX / 16;
		int alignedY = (ePosY + eHeight) / 16;

		if (xVel > 0 && (brainway[alignedY][alignedX] == 5))
		{
			if (canJump)
			{
				this->canJump = false;
				this->setYVel(this->getYVel() - ((this->getPhysics()->getJumpStrength()) * 0.010));
			}
		}
		else if (xVel < 0 && (brainway[alignedY][alignedX] == 7))
		{
			if (canJump)
			{
				this->canJump = false;
				this->setYVel(this->getYVel() - ((this->getPhysics()->getJumpStrength()) * 0.010));
			}
		}
		else if (brainway[alignedY][alignedX] == 9)
		{
			if (canJump)
			{
				this->canJump = false;
				this->setYVel(this->getYVel() - ((this->getPhysics()->getJumpStrength()) * 0.010));
			}
		}
	}
}

void Enemy::makeBrainway(Tilemap* t)
{
	// Just like with our old tilemap implementation we must
	// first assign some starting values
	int xMax = t->getMaxWidth();
	int yMax = t->getMaxHeight();
	int** currentMap = t->getTileMap();

	// We also need to initialize our tilemap
	this->brainway = new int* [yMax];
	for (int i = 0; i < yMax; i++)
		this->brainway[i] = new int[xMax];

	// Now we copy over the values from the passed
	// tilemap so that we can work with it on our end
	for (int i = 0; i < yMax; i++)
	{
		for (int j = 0; j < xMax; j++)
		{
			// Get input and 
			// add to the array
			// Note: we simplify the numbers we store
			// in this tilemap to be only 0's 1's or 2's
			if (currentMap[i][j] != 0 && currentMap[i][j] != 1 && currentMap[i][j] != 2)
				this->brainway[i][j] = 0;
			else
			{
				this->brainway[i][j] = currentMap[i][j];
			}
		}
	}

	// Declare our query variable
	int query = -1;
	// Now that we have our tilemap representation stored within an int array we can begin 
	// to read it and parse where we need to create our nodes.
	for (int i = 0; i < yMax; i++)
	{
		for (int j = 0; j < xMax; j++)
		{
			query = -1;
			// First we check to see if we are at a piece of
			// walkable terrain
			if (this->brainway[i][j] == 1 || this->brainway[i][j] == 2)
			{
				query = checkTile(i, j, xMax);	// We pass our xMax to avoid testing out of bounds
			}

			// Next we evaluate the result of our query and generate a corresponding node
			if (query == 1)
				this->brainway[i - 1][j] = 5;
			else if (query == 2)
				this->brainway[i - 1][j] = 7;
		}
	}

	// We lastly do a pass from the top down to place additional jumpnodes
	// wherever there is a jumpnode above to compliment the design of our maps
	for (int i = 0; i < yMax; i++)
	{
		for (int j = 0; j < xMax; j++)
		{
			if (this->brainway[i][j] == 5)
			{
				// Iterate downwards until we hit our first
				// collidable, and place a jumpnode there
				for (int y = i; y < yMax; y++)
				{
					if (this->brainway[y][j - 1] == 1 || this->brainway[y][j - 1] == 2)
					{
						this->brainway[y - 1][j - 1] = 9;
						break;
					}
					else if (this->brainway[y][j - 1] == 5 || this->brainway[y][j - 1] == 7)
					{
						this->brainway[y][j - 1] = 9;
						break;
					}
				}
			}

			if (this->brainway[i][j] == 7)
			{
				for (int y = i; y < yMax; y++)
				{
					if (this->brainway[y][j + 1] == 1 || this->brainway[y][j + 1] == 2)
					{
						this->brainway[y - 1][j + 1] = 9;
					}
					else if (this->brainway[y][j + 1] == 5 || this->brainway[y][j + 1] == 7)
					{
						this->brainway[y][j + 1] = 9;
						break;
					}
				}
			}
		}
	}
}

int Enemy::checkTile(int y, int x, int xMax)
{
	// Our return types follow the following key:
// -1 : Block above is solid, no room for a node
//  0 : There are no key features on either side
//  1 : There is a wall to the left
//  2 : There is a wall to the right
//  3 : There are walls on both sides

// Before each one of our checks we need to ensure we remain within
// our bounds, otherwise we will experience some issues
	int result = 0;

	// First check above
	if (int(y - 1) < 0)
		return -1;
	else
		if ((this->brainway[y - 1][x] == 1) || (this->brainway[y - 1][x] == 2))
			return -1;

	// Next check the left
	if (!(int(x - 1) < 0))
	{
		// NOTE: we shouldn't have to catch
		// going out of bounds above since we already checked
		if (this->brainway[y][x - 1] == 0)
			result = 1;
	}

	// Now check the right
	// Note: we include additional checks based on the value of result to include more information in our result

	if (!(int(x + 1) >= xMax))
	{
		// NOTE: we shouldn't have to catch
		// going out of bounds above since we already checked
		// Yes this is ugly
		if (this->brainway[y][x + 1] == 0)
			result = 2;

	}
	// Return our findings
	return result;
}

bool Enemy::playerIsNear(double playerX, double playerY)
{
	if (getXPosition() < playerX + 140 &&
		getXPosition() + getCurrFrame().getWidth() > playerX &&
		getYPosition() < playerY - 10 &&
		getYPosition() + getCurrFrame().getHeight() > playerY)
		return true;
}

bool Enemy::collide(int** tilemap, double xVel, double yVel)
{
	double ePosY = getYPosition();
	double ePosX = getXPosition();
	int eHeight = getCurrFrame().getHeight();
	int eWidth = getCurrFrame().getWidth();		//get enemy height, width, & positions

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
	if (this->flag == 1)
	{
		if (xVel < 0 && yVel < 0)
			setCurrFrame(0);
		else if (xVel < 0 && yVel >= 0)
			setCurrFrame(1);
		else if (xVel >= 0 && yVel >= 0)
			setCurrFrame(2);
		else if (xVel >= 0 && yVel < 0)
			setCurrFrame(3);
	}
	else if (this->flag == 2)
	{
		return land;
	}
}

void Enemy::takeDamage(double xVector, double yVector)
{
	int impactForce = 10;
	setXVel(xVel + xVector * impactForce);
	setYVel(yVel + yVector * impactForce);
	hp -= 10;
}
