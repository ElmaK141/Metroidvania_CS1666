#include "tile.h"


Tile::Tile(Sprite* sp) {
	this->tile = new Sprite(*sp);
}

Tile::~Tile()
{
	delete this->tile;
}

Sprite* Tile::getSprite() {
	return tile;
}