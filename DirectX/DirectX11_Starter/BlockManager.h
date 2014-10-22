#pragma once

#include "GameObject.h"
#include <stdlib.h>
#include <math.h>

// Values for the game
#define GRID_WIDTH 10
#define GRID_HEIGHT 20
#define SLOW_FALL_SPEED 0.05f
#define FAST_FALL_SPEED 0.2f
#define SIDE_SPEED 0.1f
#define SPEED_INCREASE 0.01f
#define ROTATION_SPEED 12f

// A block object in the game
struct Block
{
	GameObject* gameObject;
	bool* localGrid;
	bool* tempGrid;
};

// A direction to move a block
enum MoveDirection
{
	LEFT,
	RIGHT,
	DOWN
};

// Manages interactions with the blocks in the game
class BlockManager
{
public:
	BlockManager(XMFLOAT3 min, XMFLOAT3 holdPos, float blockWidth);
	~BlockManager();

	void update();

	bool canMove(MoveDirection direction);
	bool canOccupy(int x, int y);

	void spawnFallingBlock() { spawnFallingBlock(false); }
	void spawnFallingBlock(bool holdReplacement);
	void move(MoveDirection direction);
	void rotate();
	void holdBlock();
	void mergeBlock();
	void checkLines(int min, int max);

private:
	Block** gameGrid;
	Block* activeBlock;
	Block* heldBlock;
	
	XMFLOAT3 min;
	XMFLOAT3 holdPos;
	float blockWidth;
	float rotation = 0;
	int targetX;
	int targetY;
};

