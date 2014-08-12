package com.Foofles;

public class DestructibleObjectInfo extends GameObjectInfo{
	public GameObjectInfo SpawnInfo = null;
	public int SpawnID = 0;
	public int SpawnType = 0;
	
	DestructibleObjectInfo(){
		bIsPlatform = 1;
		bIsKillable = 1;
	}
}
