#ifndef _BLOCK_H_
#define _BLOCK_H_
#endif

class Block
{
	public:
		// Public Constructor:
		Block(int row, int col, int numRow, int numCol, int mapType);
		// Public Functions:
		void generateBlock();
		void populateBlock();
		bool checkBlock(Block* b);
		int** getBlockMap();
		void setBlock(int s);
		int getSet();
		void setDoor();
		int getRow();
		int getCol();
		//BlockType getType();
		bool isRight();
		bool isLeft();
		bool isAbove();
		bool isBelow();
		int getXcon();
		int getYcon();
		void addPlatforms();
	private:
		// Private Functions:
		void setType();
		void initMetadata();
		// generate blocks (first pass)
		void generateEmpty();
		void generateMiddle();
		void generateFloor();
		void generateCeiling();
		void generateWallL();
		void generateWallR();
		// populate blocks (second pass)
		void populateMiddle();
		void populateFloor();
		void populateCeiling();
		void populateWallL();
		void populateWallR();
		

		// check surrounding?

		// Private variables

		// Row and column position of this block in the blockMap
		int row;
		int col;
		int mapType;

		// Number of rows and columns in the blockMap (for bounds and typing)
		int numRow;
		int numCol;

		// Internal tilemap of this block
		int** map;

		// Metadata and Flags - info on what the block "wants", "has" and "where" it connects to other blocks
		
		// What type of block is this?
		enum class BlockType {
			Empty = -1,
			Middle,
			Floor,
			Ceiling,
			WallL,
			WallR
		} type;
		// * Walls contain corners

		// Is this block fully set in the final tilemap?
		// -1 not set, 0 set but can change, 1 set
		int set;

		// Does this block connect to a block on any side of it?
		bool connectedRight;
		bool connectedLeft;
		bool connectedAbove;
		bool connectedBelow;

		// If this block is connected to another block. Where does it connect?
		// -1 if not connected
		int x;
		int y;

		// Is this block a corner or a door?
		bool corner;
		bool door;

		// has platform bool?
};