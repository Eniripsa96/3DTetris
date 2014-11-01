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
#define ROTATION_SPEED 12.0f

// A block object in the game
struct Block
{
	GameObject* gameObject;
	bool threeByThree;
	bool* localGrid;
	bool* tempGrid;
	bool* grid;
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
	BlockManager(Block* blocks, int numBlocks, Mesh* cube, XMFLOAT3 min, XMFLOAT3 holdPos, float blockWidth);
	~BlockManager();

	void update();
	void draw();

	bool canMove(MoveDirection direction);
	bool canOccupy(int x, int y);

	void spawnFallingBlock();
	void move(MoveDirection direction);
	void rotate();
	void holdBlock();
	void mergeBlock();
	void checkLines(int min, int max);
	bool isGameOver() { return gameOver; };

private:
	Block* blocks;
	Block** gameGrid;
	Mesh* cube;
	int* typeOrder;
	
	XMFLOAT3 min;
	XMFLOAT3 holdPos;
	float blockWidth;
	float rotation = 0;
	bool canSwap = true;
	bool gameOver = false;
	int numBlocks;
	int targetX;
	int targetY;
	int activeId = -1;
	int heldId = -1;

	void copy(bool* src, bool* dest, int num);
	void shuffle();
};

