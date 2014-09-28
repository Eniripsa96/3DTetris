using UnityEngine;
using System.Collections;
using System.Collections.Generic;

public class GameManager : MonoBehaviour
{
	public enum Direction { LEFT, RIGHT, DOWN };

	private static readonly int WIDTH = 10;
	private static readonly int HEIGHT = 18;

	public List<GameObject> blockShapes;
	public GameObject cube;

	private GameObject[,] gameGrid = new GameObject[WIDTH, HEIGHT];

	private int counter = 0;

	// Use this for initialization
	void Start()
	{
		spawnFallingBlock();
	} 
	
	// Update is called once per frame
	void Update() 
	{
	}

	public bool isOccupied(int x, int y) 
	{
		return gameGrid[x, y] != null;
	}

	public void set(int x, int y, GameObject go)
	{
		gameGrid[x, y] = go;
	}

	public void spawnFallingBlock() 
	{
		int index = (int)(Random.value * blockShapes.Count);
		GameObject go = (GameObject)GameObject.Instantiate(blockShapes[index]);
		go.transform.Translate(3, 17, 0);
	}

	/// <summary>
	/// Checks whether or not the falling block can move in the given direction.
	/// If a block is already partway moving in that direction to the next cell,
	/// this will check the cell after instead.
	/// </summary>
	/// <returns>true if can move, false otherwise</returns>
	/// <param name="block">the block to check for</param>
	/// <param name="direction">the direction of the movement</param>
	public bool canMove(FallingBlock block, Direction direction) {

		Vector3 pos = block.gameObject.transform.position;

		int x, y;
		switch (direction) 
		{
		
			case Direction.LEFT:
				
				x = block.TargetX - 1;
				Debug.Log("Testing X: " + x);
				return canOccupy(block.LocalGrid, x, (int)pos.y)
					&& canOccupy(block.LocalGrid, x, (int)Mathf.Ceil(pos.y));

			case Direction.RIGHT:

				x = block.TargetX + 1;
				return canOccupy(block.LocalGrid, x, (int)pos.y)
					&& canOccupy(block.LocalGrid, x, (int)Mathf.Ceil(pos.y));

			case Direction.DOWN:

				y = block.TargetY - 1;
				return canOccupy(block.LocalGrid, (int)pos.x, y)
					&& canOccupy(block.LocalGrid, (int)Mathf.Ceil(pos.x), y);
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
	private bool canOccupy(bool[,] localGrid, int x, int y)
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
					Debug.Log("Collision with game walls");
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
					Debug.Log("Collision at (" + (x + i) + ", " + (y + j) + ")");
					return false;
				}
			}
		}

		// No collisions
		return true;
	}

	/// <summary>
	/// Merges a falling block into the game grid
	/// </summary>
	/// <param name="block">Block to merge into the grid</param>
	public void Merge(FallingBlock block)
	{
		int x = (int)block.transform.position.x;
		int y = (int)block.transform.position.y;

		bool[,] localGrid = block.LocalGrid;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if (localGrid[i, j])
				{
					GameObject go = (GameObject)GameObject.Instantiate(cube);
					go.transform.Translate(x + i, y + j, 0);
					gameGrid[x + i, y + j] = go;
				}
			}
		}
		GameObject.Destroy(block.gameObject);
		spawnFallingBlock();
	}
}


