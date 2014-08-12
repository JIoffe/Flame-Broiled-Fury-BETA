package com.Foofles;

public class LevelTile {
	//If the height at Future X is this much greater than present X,
	//then we consider this a wall. Any height shorter than this
	//will not barre collision.
	final public static float MinimumWallHeight = 0.035f; 
	final byte[] HeightMask = new byte[64];
	public byte SpriteID;
	public byte bIsCeiling;
	public byte bIsNotWall;
	
	final public static LevelTile BlankTile = new LevelTile();
	final public static LevelTile BottomlessPit = new LevelTile();
	public LevelTile(){
		SpriteID = -1;
		bIsCeiling = 0;
		bIsNotWall = 0;
	}
}
