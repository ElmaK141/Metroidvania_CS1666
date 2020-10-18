#ifndef _PLATFORMINFO_H_
#define _PLATFORMINFO_H_
#endif


class PlatformInfo
{
	public:
		// Public Constructor:
		PlatformInfo(int x, int y);
		// Public Functions:
		bool checkRow(int row);
		int getNumRows();
		int getRowLocation(int i);
		int getNumPlatforms(int i);
		int* getPositions(int i);
		int* getHeights(int i);
		int* getLengths(int i);
	private:
		// Private Functions:
		void generateConditions();
		void generatePlatforms();
		// Private variables
		int xMax;
		int yMax;
		int nRows;
		int* rowLocations;
		int* numPlatforms;
		int** platformPositions;
		int** platformHeights;
		int** platformLengths;
};