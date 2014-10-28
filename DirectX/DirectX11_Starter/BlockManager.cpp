#include "BlockManager.h"

// Initializes the BlockManager given 
// the minimum coordinates for blocks, the hold position for blocks, 
// and the width of each block
BlockManager::BlockManager(BlockType* pBlocks, int pNumBlocks, Mesh* pCube, XMFLOAT3 pMin, XMFLOAT3 pHoldPos, float pBlockWidth)
{
	blocks = pBlocks;
	numBlocks = pNumBlocks;
	cube = pCube;
	min = pMin;
	holdPos = pHoldPos;
	blockWidth = pBlockWidth;
	Block* arr = new Block[GRID_WIDTH * GRID_HEIGHT];
	gameGrid = &arr;
	typeOrder = new int[numBlocks];
	shuffle();
}

// Clears the pointers used by the block manager on deconstruct
BlockManager::~BlockManager()
{
	if (activeBlock != NULL) delete activeBlock;
	if (heldBlock != NULL) delete heldBlock;
}

// Updates the blocks in the game
void BlockManager::update() 
{
	// Cannot update if the game is not active
	if (activeBlock == NULL) {
		return;
	}

	// Convert block position to grid position
	XMFLOAT3 pos = activeBlock->gameObject->position;
	float x = (pos.x / blockWidth) - min.x;
	float y = (pos.y / blockWidth) - min.y;

	// Apply smooth horizontal movement
	float dx = targetX - x;
	if (dx > 0)
	{
		pos.x += blockWidth * min(dx, SIDE_SPEED);
	}
	else
	{
		pos.x += blockWidth * max(dx, -SIDE_SPEED);
	}

	// Apply "gravity"
	float dy = targetY - y;
	if (dy >= -SLOW_FALL_SPEED && canMove(DOWN))
	{
		targetY--;
		dy = targetY - y;
	}

	// Apply smooth vertical movement
	float yChange = max(dy, -SLOW_FALL_SPEED);
	if (yChange == 0 && targetX == x && rotation == 0)
	{
		mergeBlock();
	}
	else
	{
		pos.y += yChange;
	}

	// Apply smooth rotation
	if (rotation > 0)
	{
		float angle = min(rotation, ROTATION_SPEED);
		activeBlock->gameObject->Rotate(&XMFLOAT3(0, 0, -angle));
		rotation -= angle;
	}
}

// Draws the blocks in the game
void BlockManager::draw()
{
	activeBlock->gameObject->Draw();
	for (int i = 0; i < GRID_HEIGHT * GRID_WIDTH; i++) {
		gameGrid[i]->gameObject->Draw();
	}
}

// Checks whether or not a move in the given direction can be made
bool BlockManager::canMove(MoveDirection direction)
{
	// If the game is not active, no moves can be made
	if (activeBlock == NULL) {
		return false;
	}

	// Check for collisions when active
	switch (direction)
	{
	case LEFT:
		return canOccupy(targetX - 1, targetY);
	case RIGHT:
		return canOccupy(targetX + 1, targetY);
	case DOWN:
		return canOccupy(targetX, targetY - 1);
	default:
		return false;
	}
}

// Checks whether or not the active block can fit into the given spot
// with it's current orientation
bool BlockManager::canOccupy(int x, int y)
{
	// Cannot check if the game is not active
	if (activeBlock == NULL) {
		return false;
	}

	// Compare the active block's local grid with the game grid at the position
	int size = activeBlock->threeByThree ? 3 : 4;
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			// Ignore empty cells of the block
			if (!activeBlock->localGrid[i + j * size])
			{
				continue;
			}

			// Limit it to the game board
			else if (x + i < 0 || x + i >= GRID_WIDTH || y + j < 0)
			{
				return false;
			}

			// Ignore above the board
			else if (y + j >= GRID_HEIGHT)
			{
				continue;
			}

			// Check for collision with other blocks
			else if (gameGrid[x + i + (y + j) * GRID_WIDTH] != NULL)
			{
				return false;
			}
		}
	}

	// No collisions
	return true;
}

// Spawns a new falling block at the top of the game
void BlockManager::spawnFallingBlock() 
{
	Block block;

	// Get a random block
	BlockType type = blocks[typeOrder[typeId++]];
	if (typeId == numBlocks) 
	{
		shuffle();
	}

	// Get the spawn location
	float x = blockWidth * ((GRID_WIDTH - 4) / 2) + min.x;
	float y = blockWidth * GRID_HEIGHT + min.y;
	float z = min.z;

	// Create the block
	block.gameObject = new GameObject(type.mesh, type.material, new XMFLOAT3(x, y, z), new XMFLOAT3(0, 0, 0));
	block.threeByThree = type.threeByThree;
	int size = block.threeByThree ? 9 : 16;
	block.localGrid = new bool[size];
	block.tempGrid = new bool[size];
	copy(type.localGrid, block.localGrid, size);
	copy(type.localGrid, block.tempGrid, size);

	// Make it the active block
	activeBlock = &block;
}

// Tries to move the active block in the given direction
void BlockManager::move(MoveDirection direction)
{
	// Cannot move if the game is not active
	if (activeBlock == NULL) {
		return;
	}

	// Convert block position to grid position
	float x = (activeBlock->gameObject->position.x / blockWidth) - min.x;

	// Ignore it if it's still mid-movement
	float dx = abs(x - targetX);
	if (dx > SIDE_SPEED || !canMove(direction)) return;

	// Apply the move
	switch (direction)
	{
	case LEFT:
		targetX--;
		break;
	case RIGHT:
		targetX++;
		break;
	}
}

// Rotates the active block if able to
void BlockManager::rotate()
{
	// Cannot rotate if the game is not active
	if (activeBlock == NULL) {
		return;
	}

	// Rotate the temp grid into the local grid
	int size = activeBlock->threeByThree ? 3 : 4;
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			activeBlock->localGrid[i + j * size] = activeBlock->tempGrid[size - 1 - j + i * size];
		}
	}

	// See if the rotation is valid
	bool canRotateNormal = canOccupy(targetX, targetY);
	bool canRotateRight = !canRotateNormal && canOccupy(targetX + 1, targetY);
	bool canRotateLeft = !canRotateNormal && canOccupy(targetX - 1, targetY);
	if (canRotateNormal || canRotateLeft || canRotateRight)
	{
		// Move if necessary
		if (canRotateRight)
		{
			targetX++;
		}
		else if (canRotateLeft)
		{
			targetX--;
		}

		// Update the temp grid
		rotation = 90.0f;
		copy(activeBlock->localGrid, activeBlock->tempGrid, size);
	}

	// Restore the local grid if it cannot rotate
	else
	{
		copy(activeBlock->tempGrid, activeBlock->localGrid, size);
	}
}

// Moves the active block to the held spot and replaces it with a new block
// or the previously held block
void BlockManager::holdBlock()
{
	// Cannot hold a block if the game is not active
	if (activeBlock == NULL) {
		return;
	}

	if (canSwap)
	{
		Block* held = activeBlock;
		canSwap = false;

		// If there is no held block, spawn a new one
		if (heldBlock == NULL)
		{
			spawnFallingBlock();
		}

		// Otherwise grab the falling block
		else
		{
			targetX = (GRID_WIDTH - 4) / 2;
			targetY = GRID_HEIGHT;
			heldBlock->gameObject->position = XMFLOAT3((targetX + min.x) * blockWidth, (targetY + min.y) * blockWidth, min.z * blockWidth);
			activeBlock = heldBlock;
		}

		// Hold the block
		heldBlock = held;
	}
}

// Merges the active block into the game grid
void BlockManager::mergeBlock()
{
	// Cannot merge a block if the game is not active
	if (activeBlock == NULL) {
		return;
	}

	canSwap = true;

	int minY = GRID_HEIGHT;
	int maxY = 0;
	int size = activeBlock->threeByThree ? 3 : 4;
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			if (activeBlock->localGrid[i + j * size]) 
			{
				// Game over
				if (targetY + j >= GRID_HEIGHT || gameGrid[i + j * size] != NULL)  
				{
					gameOver = true;
					delete activeBlock;
					activeBlock = NULL;
					return;
				}

				// Mark the min/max rows for checking lines
				if (targetY + j < minY)
				{
					minY = targetY + j;
				}
				if (targetY + j > maxY)
				{
					maxY = targetY + j;
				}

				// Merge the cell
				Block block;
				float x = (targetX + i) * blockWidth + min.x;
				float y = (targetY + j) * blockWidth + min.y;
				float z = min.z;
				block.gameObject = new GameObject(cube, activeBlock->gameObject->material, new XMFLOAT3(x, y, z), new XMFLOAT3(0, 0, 0));
				gameGrid[targetX + i + (targetY + j) * size] = &block;
			}
		}
	}

	// Delete the old active block
	delete activeBlock;

	// Check lines for completion
	checkLines(minY, maxY);

	// Spawn a new block
	spawnFallingBlock();
}

// Checks the game grid for cleared lines
void BlockManager::checkLines(int min, int max)
{
	for (int i = max; i >= min; i--)
	{
		// Check each individual line for completion
		bool complete = true;
		for (int j = 0; j < GRID_WIDTH; j++)
		{
			complete &= (gameGrid[j + i * GRID_WIDTH] != NULL);
		}

		// Clear the line and move down above lines if complete
		if (complete)
		{
			// TODO change our gameobjects to an effect if desired

			// Delete the game objects for now
			for (int j = 0; j < GRID_WIDTH; j++)
			{
				delete gameGrid[j + i * GRID_WIDTH];
				gameGrid[j + i * GRID_WIDTH] = NULL;
			}

			// Move down higher rows
			for (int j = i; j < GRID_HEIGHT - 1; j++)
			{
				for (int k = 0; k < GRID_WIDTH; k++)
				{
					int index = k + j * GRID_WIDTH;
					gameGrid[index] = gameGrid[index + GRID_WIDTH];
					gameGrid[index + GRID_WIDTH] = NULL;
					if (gameGrid[index] != NULL)
					{
						gameGrid[index]->gameObject->position.y -= 1;
					}
				}
			}
		}
	}
}

// Copies the elements from the source array to the target array
void BlockManager::copy(bool* src, bool* dest, int num) {
	for (int i = 0; i < num; i++) {
		dest[i] = src[i];
	}
}

// Shuffles the order of block types to spawn
void BlockManager::shuffle() {
	typeId = 0;
	for (int i = 0; i < numBlocks; i++) {
		typeOrder[i] = -1;
	}
	for (int i = 0; i < numBlocks; i++) {
		int index;
		do 
		{
			index = rand() % numBlocks;
		} 
		while (typeOrder[index] != -1);
		typeOrder[index] = i;
	}
}
