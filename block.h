#ifndef _BLOCK_H_
#define _BLOCK_H_
#endif

class Block
{
	public:
		// Public Constructor:
		Block(int row, int col, int numRow, int numCol);
		// Public Functions:
		void generateBlock();
		bool checkBlock(Block* b);
		void setBlock(int s);
		int getSet();
		int getRow();
		int getCol();
		//BlockType getType();
		bool isRight();
		bool isLeft();
		bool isAbove();
		bool isBelow();
		int getXcon();
		int getYcon();
	private:
		// Private Functions:
		void setType();
		void initMetadata();
		void generateEmpty();
		void generateMiddle();
		void generateFloor();
		void generateCeiling();
		void generateWallL();
		void generateWallR();
		

		// check surrounding?

		// Private variables

		// Row and column position of this block in the blockMap
		int row;
		int col;

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
		bool connnectedRight;
		bool connnectedLeft;
		bool connectedAbove;
		bool connectedBelow;

		// If this block is connected to another block. Where does it connect?
		// -1 if not connected
		int x;
		int y;

		// Is this block a corner or a door?
		bool corner;
		bool door;
};