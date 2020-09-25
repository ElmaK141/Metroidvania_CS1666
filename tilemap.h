#ifndef _TILEMAP_H_
#define _TILEMAP_H_
#endif

#include <string>


// Suggested Modification:
//		- Include tile object and use for the tilemap
//		- Include function to read a tilemap from a function
class Tilemap
{
		public:
			Tilemap();
			int** getTileArray(std::string filename);
			int getMaxWidth();
			int getMaxHeight();
			~Tilemap();
		private:
			int yMax;
			int xMax;

};