﻿using UnityEngine;
using System.Collections;

/// <summary>
/// Class for handling the logic for controllable falling blocks
/// </summary>
public class FallingBlock : MonoBehaviour {

	public static float FALL_SPEED = 0.05f;
	public static float SIDE_SPEED = 0.1f;
	public static float SPEED_INCREASE = 0.01f;

	private bool[,] localGrid = new bool[4, 4];
	private bool[,] tempGrid = new bool[4, 4];

	private GameManager gameManager;
	private int targetX = 3, targetY = 17;
	private float speedMultiplier = 1.0f;

	/// <summary>
	/// The local occupied cell grid for the block used
	/// to determine collisions when compared to the game grid
	/// </summary>
	/// <value>The local occupied cell grid for the block</value>
	public bool[,] LocalGrid 
	{
		get 
		{ 
			return localGrid; 
		}
	}

	/// <summary>
	/// The horizontal target position of the block used for
	/// smoothly moving to the next cell
	/// </summary>
	/// <value>The horizontal target position of the block</value>
	public int TargetX
	{
		get
		{
			return targetX;
		}
	}

	/// <summary>
	/// The vertical target position of the block used for
	/// smoothly moving to the next cell
	/// </summary>
	/// <value>The vertical target position of the block</value>
	public int TargetY
	{
		get
		{
			return targetY;
		}
	}

	// Initialization
	void Start() 
	{
		gameManager = GameObject.FindGameObjectWithTag("GameManager").GetComponent<GameManager>();
		for (int i = 0; i < this.transform.childCount; i++) 
		{
			Transform child = this.transform.GetChild(i);
			int x = (int)(child.localPosition.x);
			int y = (int)(child.localPosition.y);
			localGrid[x, y] = true;
		}
	}

	// Update is called once per frame
	void Update () 
	{
		// Key input for moving sideways
		bool left = Input.GetKey(KeyCode.LeftArrow);
		bool right = Input.GetKey(KeyCode.RightArrow);
		if (left && !right)
		{
			if (transform.position.x < targetX) 
			{
				targetX--;
			}
			else if (transform.position.x - targetX <= SIDE_SPEED
			    && gameManager.CanMove(this, GameManager.Direction.LEFT))
			{
				targetX -= 1;
			}
		}
		else if (right && !left)
		{
			if (transform.position.x > targetX)
			{
				targetX++;
			}
			else if (targetX - this.transform.position.x <= SIDE_SPEED
			    && gameManager.CanMove(this, GameManager.Direction.RIGHT))
			{
				targetX += 1;
			}
		}

		// Sideways movement
		float dx = targetX - this.transform.position.x;
		if (dx > 0) 
		{
			this.transform.Translate(Mathf.Min(dx, SIDE_SPEED * speedMultiplier), 0, 0);
		}
		else
		{
			this.transform.Translate(Mathf.Max(dx, -SIDE_SPEED * speedMultiplier), 0, 0);
		}

		// Downward speed
		float speed = FALL_SPEED * speedMultiplier;
		if (Input.GetKey(KeyCode.DownArrow))
		{
			speed *= 4;
		}

		// Downward collisions
		float dy = targetY - this.transform.position.y;
		if (dy >= -speed && gameManager.CanMove(this, GameManager.Direction.DOWN))
		{
			targetY--;
			dy = targetY - this.transform.position.y;
		}

		// Moving downward
		float yChange = Mathf.Max(dy, -speed);
		if (yChange == 0 && targetX == this.transform.position.x)
		{
			gameManager.Merge(this);
			speedMultiplier += SPEED_INCREASE;
		}
		else 
		{
			this.transform.Translate(0, yChange, 0); 
		}

		// Rotating
		if (Input.GetKeyDown(KeyCode.UpArrow))
		{
			// Rotate the local grid
			for (int i = 0; i < 4; i++) 
			{
				for (int j = 0; j < 4; j++)
				{
					tempGrid[i, j] = localGrid[3 - j, i];
				}
			}

			// See if it can rotate
			bool canRotateNormal = CanRotate(0, 0);
			bool canRotateRight = !canRotateNormal && CanRotate(1, 0);
			bool canRotateLeft = !canRotateNormal && CanRotate(-1, 0);
			if (canRotateNormal || canRotateLeft || canRotateRight) 
			{
				// Move if necesssary
				if (canRotateRight)
				{
					this.transform.Translate(1, 0, 0);
					this.targetX++;
				}
				else if (canRotateLeft)
				{
					this.transform.Translate(-1, 0, 0);
					this.targetX--;
				}

				// Apply the rotation
				for (int i = 0; i < this.transform.childCount; i++)
				{
					Transform child = this.transform.GetChild(i);
					child.localPosition = new Vector3(child.localPosition.y, 3 - child.localPosition.x);
				}
				for (int i = 0; i < 4; i++) 
				{
					for (int j = 0; j < 4; j++)
					{
						localGrid[i, j] = tempGrid[i, j];
					}
				}
			}
		}
	}

	/// <summary>
	/// Checks whether or not the block can rotate when moving the given offsets
	/// </summary>
	/// <returns>True if able to rotate, false otherwise</returns>
	/// <param name="xOffset">Horizontal offset</param>
	/// <param name="yOffset">Vertical offset</param>
	private bool CanRotate(int xOffset, int yOffset)
	{
		// Get the bounds
		int xMin = (int)this.transform.position.x + xOffset;
		int yMin = (int)this.transform.position.y + yOffset;
		int xMax = (int)Mathf.Ceil(this.transform.position.x) + xOffset;
		int yMax = (int)Mathf.Ceil(this.transform.position.y) + yOffset;

		// Check for collisions
		bool canOccupy = true;
		for (int i = xMin; i <= xMax; i++)
		{
			for (int j = yMin; j <= yMax; j++)
			{
				canOccupy = canOccupy && gameManager.CanOccupy(tempGrid, i, j);
			}
		}

		// Return the result
		return canOccupy;
	}
}
