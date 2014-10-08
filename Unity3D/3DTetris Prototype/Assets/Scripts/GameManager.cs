using UnityEngine;
using System.Collections;
using System.Collections.Generic;

/// <summary>
/// Handles the majority of the game logic
/// </summary>
public class GameManager : MonoBehaviour
{
	public enum Direction { LEFT, RIGHT, DOWN };

	private static readonly int WIDTH = 10;
	private static readonly int HEIGHT = 18;

	public List<GameObject> blockShapes;
	public GameObject cube;

	private GameObject[,] gameGrid = new GameObject[WIDTH, HEIGHT];
	private FallingBlock heldBlock;

	private bool gameOver = false;

	// Use this for initialization
	void Start()
	{
		SpawnFallingBlock();
	} 
	
	// Update is called once per frame
	void Update() 
	{
	}

	/// <summary>
	/// Checks if the cell in the game grid is occupied
	/// </summary>
	/// <returns>True if occupied, false otherwise</returns>
	/// <param name="x">The x coordinate</param>
	/// <param name="y">The y coordinate</param>
	public bool IsOccupied(int x, int y) 
	{
		return gameGrid[x, y] != null;
	}

	/// <summary>
	/// Sets an object in a cell of the game grid
	/// </summary>
	/// <param name="x">The x coordinate</param>
	/// <param name="y">The y coordinate</param>
	/// <param name="go">Game Object to set to the cell</param>
	public void Set(int x, int y, GameObject go)
	{
		gameGrid[x, y] = go;
	}

	/// <summary>
	/// Spawns a new falling block at the top of the game board
	/// </summary>
	public void SpawnFallingBlock(bool holdReplacement = false) 
	{
		int index = (int)(Random.value * blockShapes.Count);
		GameObject go = (GameObject)GameObject.Instantiate(blockShapes[index]);
		go.transform.Translate((WIDTH - 4) / 2, HEIGHT, 0);
		
		if (holdReplacement) 
		{
			FallingBlock block = go.GetComponent<FallingBlock>();
			block.Held = true;
			block.Held = false;
		}
	}

	/// <summary>
	/// Checks whether or not the falling block can move in the given direction.
	/// If a block is already partway moving in that direction to the next cell,
	/// this will check the cell after instead.
	/// </summary>
	/// <returns>true if can move, false otherwise</returns>
	/// <param name="block">the block to check for</param>
	/// <param name="direction">the direction of the movement</param>
	public bool CanMove(FallingBlock block, Direction direction) {

		Vector3 pos = block.gameObject.transform.position;

		int x, y;
		switch (direction) 
		{
		
			case Direction.LEFT:
				
				x = block.TargetX - 1;
				return CanOccupy(block.LocalGrid, x, (int)pos.y)
					&& CanOccupy(block.LocalGrid, x, (int)Mathf.Ceil(pos.y));

			case Direction.RIGHT:

				x = block.TargetX + 1;
				return CanOccupy(block.LocalGrid, x, (int)pos.y)
					&& CanOccupy(block.LocalGrid, x, (int)Mathf.Ceil(pos.y));

			case Direction.DOWN:

				y = block.TargetY - 1;
				return CanOccupy(block.LocalGrid, (int)pos.x, y)
					&& CanOccupy(block.LocalGrid, (int)Mathf.Ceil(pos.x), y);
		}

		return false;
	}

	/// <summary>
	/// Checks whether or not a block can move into the given cell.
	/// If the cell 
	/// </summary>
	/// <returns>true if the grid can occupy the cells, false otherwise</returns>
	/// <param name="localGrid">the local grid of the block trying to occupy the cells</param>
	/// <param name="x">the horizontal coordinate of the block</param>
	/// <param name="y">the vertical coordinate of the block</param>
	public bool CanOccupy(bool[,] localGrid, int x, int y)
	{
		for (int i = 0; i < 4; i++) 
		{
			for (int j = 0; j < 4; j++)
			{
				// Ignore it if the block doesn't have a piece there
				if (!localGrid[i, j]) 
				{
					continue;
				}

				// Limit it to the game board
				if (x + i < 0 || x + i >= WIDTH || y + j < 0) 
				{
					return false;
				}

				// Ignore above the board
				if (y + j >= HEIGHT)
				{
					continue;
				}

				// Check for collision
				if (gameGrid[x + i, y + j])
				{
					return false;
				}
			}
		}

		// No collisions
		return true;
	}

	/// <summary>
	/// Moves the block to being held, freezing it to the side and replacing
	/// it with a new block or the previously held block.
	/// </summary>
	/// <param name="block">Block.</param>
	public void HoldBlock(FallingBlock block) 
	{
		// Get a replacement block
		if (heldBlock == null) 
		{
			SpawnFallingBlock(true);
		}
		else
		{
			heldBlock.Held = false;
			heldBlock.transform.position = new Vector3((WIDTH - 4) / 2, HEIGHT);
			heldBlock.TargetX = (int)heldBlock.transform.position.x;
			heldBlock.TargetY = (int)heldBlock.transform.position.y;
		}
		
		// Hold the new block
		block.Held = true;
		block.transform.position = new Vector3(-5, HEIGHT - 4);
		heldBlock = block;
	}

	/// <summary>
	/// Merges a falling block into the game grid
	/// </summary>
	/// <param name="block">Block to merge into the grid</param>
	public void Merge(FallingBlock block)
	{
		Material mat = block.transform.GetChild(0).gameObject.renderer.material;
		int x = (int)block.transform.position.x;
		int y = (int)block.transform.position.y;

		// Merge the block into the game grid
		bool[,] localGrid = block.LocalGrid;
		int min = HEIGHT, max = 0;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if (localGrid[i, j])
				{
					// Game over
					if (y + j >= HEIGHT)
					{
						Debug.Log("Game Over!");
						gameOver = true;
					}

					// Merge the cell
					else 
					{
						if (y + j < min)
						{
							min = y + j;
						}
						if (y + j > max)
						{
							max = y + j;
						}

						GameObject go = (GameObject)GameObject.Instantiate(cube);
						go.renderer.material = mat;
						go.transform.Translate(x + i, y + j, 0);
						gameGrid[x + i, y + j] = go;
					}
				}
			}
		}

		// Remove the old block
		GameObject.Destroy(block.gameObject);

		// Spawn a new block if not game over
		if (!gameOver) 
		{
			CheckLines(min, max);
			SpawnFallingBlock();
		}
	}

	/// <summary>
	/// Checks the game grid for completed lines
	/// </summary>
	private void CheckLines(int min, int max)
	{
		// Check each line in the bounds
		// Going backwards to prevent moving down complete lines
		for (int j = max; j >= min; j--)
		{
			// See if the individual line is complete
			bool complete = true;
			for (int i = 0; i < WIDTH; i++)
			{
				complete &= gameGrid[i, j];
			}

			// A line is complete
			if (complete)
			{
				// Delete completed row
				for (int i = 0; i < WIDTH; i++)
				{
					if (gameGrid[i, j] != null)
					{
						GameObject.Destroy(gameGrid[i, j]);
					}
				}

				// Move down above rows
				for (int k = j; k < HEIGHT - 1; k++)
				{
					for (int i = 0; i < WIDTH; i++)
					{
						gameGrid[i, k] = gameGrid[i, k + 1];
						gameGrid[i, k + 1] = null;
						if (gameGrid[i, k] != null)
						{
							gameGrid[i, k].transform.Translate(0, -1, 0);
						}
					}
				}
			}
		}
	}
}


