#include "tile.h"


Tile::Tile(Sprite* sp) {
	this->tile = new Sprite(*sp);
}

Tile::Tile(int x, int y, int w, int h, int s, std::string file, SDL_Renderer* gRender) {

	this->tile = new Sprite(x, y, w, h, s, file, gRender);
}

Tile::Tile()
{

}

Tile::~Tile()
{
	delete this->tile;
}

void Tile::setTilePos(int x, int y)
{
	this->xPos = x;
	this->yPos = y;
}

int Tile::getTileXPos()
{
	return this->xPos;
}

int Tile::getTileYPos()
{
	return this->yPos;
}

void Tile::setTileFlag(char c)
{
	this->flag = c;
}

char Tile::getTileFlag()
{
	return this->flag;
}

Sprite* Tile::getTileSprite()
{
	return this->tile;
}

void Tile::setTileSprite(Sprite* sp)
{
	this->tile = sp;
}
