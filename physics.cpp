#include "physics.h"


Physics::Physics()
{
	this->airState = false;
	this->grappleState = false;
	this->grappleDampen = 0.97;
	this->grappleStrength = 0.03 * 3600 * 1.75;
	this->airDrag = 0.03;
}

//Initialize the entity's physics properties
Physics::Physics(double jstr, double maxX, double maxY, double accel)
{
	this->jumpStrength = jstr;
	this->maxXSPeed = maxX;
	this->maxYSpeed = maxY;
	this->acceleration = accel;
	this->airState = false;
	this->grappleState = false;
	this->grappleDampen = 0.97;
	this->grappleStrength = 0.03 * 3600 * 1.75;
	this->airDrag = 0.03;
}

double Physics::getAirDrag()
{
		return this->airDrag;
}

double Physics::getJumpStrength()
{
	return this->jumpStrength;
}

void Physics::setJumpStrength(double jstr)
{
	this->jumpStrength = jstr;
}

double Physics::getMaxX()
{
	return this->maxXSPeed;
}

double Physics::getMaxY()
{
	return this->maxYSpeed;
}

void Physics::setMaxSpeeds(double x, double y)
{
	this->maxXSPeed = x;
	this->maxYSpeed = y;
}

double Physics::getAcceleration()
{
	return this->acceleration;
}

void Physics::setAcceleration(double accel)
{
	this->acceleration = accel;
}

bool Physics::inAir()
{
	return this->airState;
}

void Physics::setAirState(bool state)
{
	this->airState = state;
}

bool Physics::isGrappling()
{
	return this->grappleState;
}

void Physics::setGrappleState(bool state)
{
	this->grappleState = state;
}

double Physics::getDampen()
{
	return this->grappleDampen;
}

double Physics::getGrappleStr()
{
	return this->grappleStrength;
}
