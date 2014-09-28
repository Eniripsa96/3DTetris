using UnityEngine;
using System.Collections;

public class FallingBlock : MonoBehaviour {

	public static float FALL_SPEED = 0.05f;
	public static float SIDE_SPEED = 0.1f;

	private bool[,] localGrid = new bool[4, 4];

	private GameManager gameManager;
	private int targetX = 3, targetY = 17;

	public bool[,] LocalGrid 
	{
		get 
		{ 
			return localGrid; 
		}
	}

	public int TargetX
	{
		get
		{
			return targetX;
		}
	}

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
			int x = (int)(child.position.x - this.transform.position.x);
			int y = (int)(child.position.y - this.transform.position.y);
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
			    && gameManager.canMove(this, GameManager.Direction.LEFT))
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
			    && gameManager.canMove(this, GameManager.Direction.RIGHT))
			{
				targetX += 1;
			}
		}

		// Sideways movement
		float dx = targetX - this.transform.position.x;
		if (dx > 0) 
		{
			this.transform.Translate(Mathf.Min(dx, SIDE_SPEED), 0, 0);
		}
		else
		{
			this.transform.Translate(Mathf.Max(dx, -SIDE_SPEED), 0, 0);
		}

		// Downward speed
		float speed = FALL_SPEED;
		if (Input.GetKey(KeyCode.DownArrow))
		{
			speed *= 4;
		}
		Debug.Log("Speed: " + speed);

		// Downward collisions
		float dy = targetY - this.transform.position.y;
		if (dy >= -speed && gameManager.canMove(this, GameManager.Direction.DOWN))
		{
			targetY--;
			dy = targetY - this.transform.position.y;
		}

		// Moving downward
		float yChange = Mathf.Max(dy, -speed);
		if (yChange == 0 && targetX == this.transform.position.x)
		{
			gameManager.Merge(this);
		}
		else 
		{
			this.transform.Translate(0, yChange, 0); 
		}
	}
}
