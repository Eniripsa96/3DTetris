using UnityEngine;
using System.Collections;

/// <summary>
/// Class for handling the logic for controllable falling blocks
/// </summary>
public class FallingBlock : MonoBehaviour {

	public static float FALL_SPEED = 0.05f;
	public static float SIDE_SPEED = 0.1f;
	public static float SPEED_INCREASE = 0.01f;
	public static float ROTATION_SPEED = 720f;

	private bool[,] localGrid = new bool[4, 4];
	private bool[,] tempGrid = new bool[4, 4];

	private GameManager gameManager;
	private int targetX = 3, targetY = 17;
	private float speedMultiplier = 1.0f;
	private bool held = false;
	private bool wasHeld = false;
	private float rotation = 0;

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
	/// Whether or not the block is currently being held
	/// </summary>
	/// <value>true if held, false otherwise</value>
	public bool Held 
	{
		get
		{
			return held;
		}
		set
		{
			held = value;
			wasHeld = wasHeld || value;
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
		set
		{
			targetX = value;
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
		set
		{
			targetY = value;
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
		// Do nothing if held
		if (held)
		{
			return;
		}
		
		// Swap out if space is pressed
		if (!wasHeld && Input.GetKey(KeyCode.Space))
		{
			gameManager.HoldBlock(this);
			return;
		}

		// Key input for moving sideways
		bool left = Input.GetKey(KeyCode.LeftArrow);
		bool right = Input.GetKey(KeyCode.RightArrow);
		if (left && !right)
		{
			if (transform.position.x - targetX <= SIDE_SPEED
			    && gameManager.CanMove(this, GameManager.Direction.LEFT))
			{
				targetX--;
			}
		}
		else if (right && !left)
		{
			if (targetX - this.transform.position.x <= SIDE_SPEED
			    && gameManager.CanMove(this, GameManager.Direction.RIGHT))
			{
				targetX++;
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
		if (yChange == 0 && targetX == this.transform.position.x && rotation == 0)
		{
			gameManager.Merge(this);
			speedMultiplier += SPEED_INCREASE;
		}
		else 
		{
			this.transform.Translate(0, yChange, 0); 
		}

		// Rotating
		if (Input.GetKeyDown(KeyCode.UpArrow) && rotation < 1)
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
					//this.transform.Translate(1, 0, 0);
					this.targetX++;
				}
				else if (canRotateLeft)
				{
					//this.transform.Translate(-1, 0, 0);
					this.targetX--;
				}

				// Apply the rotation
				rotation = 90;
				for (int i = 0; i < 4; i++) 
				{
					for (int j = 0; j < 4; j++)
					{
						localGrid[i, j] = tempGrid[i, j];
					}
				}
			}
		}

		// Rotate to the target
		if (rotation > 0) 
		{
			float angle = Mathf.Min(Time.deltaTime * ROTATION_SPEED, rotation);
			for (int i = 0; i < this.transform.childCount; i++)
			{
				Transform child = this.transform.GetChild(i);
				//child.localPosition = new Vector3(child.localPosition.y, 3 - child.localPosition.x);
				child.RotateAround(new Vector3(transform.position.x + 1.5f, transform.position.y + 1.5f, 0), new Vector3(0, 0, 1), -angle);
			}
			rotation -= angle;
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
