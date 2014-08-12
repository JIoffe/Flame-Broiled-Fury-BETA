package com.Foofles;

//The grid slices the scene up into uniform horizontal slices, with each slice owning
//a linked list. The Grid class handles sorting objects within the lists
//(C) Jason Ioffe
public class GameObjectGrid {
	//Separate the lists for Enemies, collectibles, hazards, etc.
	// to make collision better... with a small memory overhead
	public GameObjectGridSlice[] Slices = null;
	
	public void InitializeGrid(final int Width){
		Slices = new GameObjectGridSlice[Width];
		for(int i = 0; i < Width; i++){
			Slices[i] = new GameObjectGridSlice();
		}
		return;
	}
	public void SortObject(final GameObject NewObject, final byte Type){
		final float PlayerX = Globals.mGameGraph.mPlayer.X;
		final CollisionData CD = NewObject.CD;
		GameObjectLinkedList NearestList = null;
		if(PlayerX < CD.Left){
			NearestList = GetListAtX(CD.Left, Type);
		}else if(PlayerX > CD.Right){
			NearestList = GetListAtX(CD.Right, Type);
		}else{
			NearestList = GetListAtX(PlayerX, Type);
		}
		if(NewObject.ListOwner != NearestList){
			if(NewObject.ListOwner != null){
				NewObject.ListOwner.Remove(NewObject);
			}
			NearestList.Add(NewObject);
		}
	}
	public void SortObject(final GameObject NewObject){
		final GameObjectLinkedList NearestList = GetNearestList(NewObject);
		if(NewObject.ListOwner != NearestList){
			if(NewObject.ListOwner != null){
				NewObject.ListOwner.Remove(NewObject);
			}
			NearestList.Add(NewObject);
		}
	}
	
	public GameObjectLinkedList GetNearestList(final GameObject go){
		//If play is within the object's AABB, then just match list with player
		final float PlayerX = Globals.mGameGraph.mPlayer.X;
		final CollisionData CD = go.CD;
	
		if(PlayerX < CD.Left){
			return GetListAtX(CD.Left, go.Type);
		}else if(PlayerX > CD.Right){
			return GetListAtX(CD.Right, go.Type);
		}else{
			return GetListAtX(PlayerX, go.Type);
		}
	}
	
	private GameObjectLinkedList GetListAtX(float X, byte Type){
		final int SliceID = (int)(X / GameGraph.LevelTileOffset);
		return Slices[SliceID].ObjectLists[Type];
	}
	
	public void SortDynamicPVS(final DynamicObject Dyn){
		//Sort the PVS of our dynamic object
		final float Left = Dyn.VisibilityMin;
		final float Right = Dyn.VisibilityMax;
		final float PlayerX = Globals.mGameGraph.mPlayer.X;
		float X = 0.0f;
		if(PlayerX < Left){
			X = Left;
		}else if(PlayerX > Right){
			X = Right;
		}else{
			X = PlayerX;
		}
		final int SliceID = (int)(X / GameGraph.LevelTileOffset);
		if(Globals.mGameGraph.WithinGridX(SliceID)){
			final DynamicObjectPVSList PVSList = Slices[SliceID].DynamicPVS;
			if(PVSList != Dyn.PVSOwner){
				if(Dyn.PVSOwner != null){
					Dyn.PVSOwner.Remove(Dyn);
				}
				Slices[SliceID].DynamicPVS.Add(Dyn);
			}
		}
		return;
	}
}
