#include "BlockManager.h"

// Initializes the BlockManager given 
// the minimum X coordinate for blocks and the width of each block
BlockManager::BlockManager(XMFLOAT3 pMin, XMFLOAT3 pHoldPos, float pBlockWidth)
{
	min = pMin;
	holdPos = pHoldPos;
	blockWidth = pBlockWidth;
	Block* arr = new Block[GRID_WIDTH * GRID_HEIGHT];
	gameGrid = &arr;
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

// Checks whether or not a move in the given direction can be made
bool BlockManager::canMove(MoveDirection direction)
{
	switch (direction)
	{
	case LEFT:
		return canOccupy(targetX - 1, targetY);
	case RIGHT:
		return canOccupy(targetX + 1, targetY);
	case DOWN:
		return canOccupy(targetX, targetY - 1);
	}
}

// Checks whether or not the active block can fit into the given spot
// with it's current orientation
bool BlockManager::canOccupy(int x, int y)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			// Ignore empty cells of the block
			if (!activeBlock->localGrid[i + j * 4])
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
}

// Spawns a new falling block at the top of the game
void BlockManager::spawnFallingBlock(bool holdReplacement) 
{
	
}

// Tries to move the active block in the given direction
void BlockManager::move(MoveDirection direction)
{
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
	// Rotate the temp grid into the local grid
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			activeBlock->localGrid[i + j * 4] = activeBlock->tempGrid[3 - j + i * 4];
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
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				activeBlock->tempGrid[i + j * 4] = activeBlock->localGrid[i + j * 4];
			}
		}
	}

	// Restore the local grid if it cannot rotate
	else
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				activeBlock->localGrid[i + j * 4] = activeBlock->tempGrid[i + j * 4];
			}
		}
	}
}

// Moves the active block to the held spot and replaces it with a new block
// or the previously held block
void BlockManager::holdBlock()
{
	Block* held = activeBlock;

	// If there is no held block, spawn a new one
	if (heldBlock == NULL)
	{
		spawnFallingBlock(true);
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

// Merges the active block into the game grid
void BlockManager::mergeBlock()
{

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
