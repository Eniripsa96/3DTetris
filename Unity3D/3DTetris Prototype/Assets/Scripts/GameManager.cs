using UnityEngine;
using System.Collections;

public class GameManager : MonoBehaviour
{
	GameObject[,] gameGrid;

	// Use this for initialization
	void Start ()
	{
		gameGrid = new GameObject[10,18];	
	} 
	
	// Update is called once per frame
	void Update () {
	
	}

	public bool isOccupied(int x, int y) 
	{
		return gameGrid[x,y] != null;
	}

	public void set(int x, int y, GameObject go)
	{
		gameGrid[x,y] = go;
	}
}


