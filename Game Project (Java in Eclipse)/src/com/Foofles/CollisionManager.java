package com.Foofles;


public class CollisionManager {
	public static LevelTile LastLevelTileHit = null;
	
	public static boolean Collision(final CollisionData A, final CollisionData B){
		if(B.bIgnore != 0){
			if(B.NextCD != null){
				return Collision(A, B.NextCD);
			}else{
				return false;
			}
		}else{
			final boolean Hit = (A.Right > B.Left && 
					   A.Left < B.Right &&
					   A.Bottom  < B.Top && 
					   A.Top  > B.Bottom );
			if(B.NextCD != null && !Hit){
				return Collision(A, B.NextCD);
			}else{
				return Hit;
			}
		}
	}
}
