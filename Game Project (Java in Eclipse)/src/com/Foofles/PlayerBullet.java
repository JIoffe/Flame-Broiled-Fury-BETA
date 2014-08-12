package com.Foofles;


public class PlayerBullet extends SpriteComponent implements Copyable, UpdateRender{
	public static int GunshotSoundID = 0;
	public static int LaserHit_SoundID = 0;
	public static int SpriteIndex = 0;
	final public static CollisionData BulletCD = new CollisionData(-0.8f, 0.8f, 0.2f, -0.2f);
	final public static float DefaultSpeed = 3.5f;
	final public static float DefaultScale = 0.13f;
	
	final public CollisionData CD = new CollisionData();

	public PlayerBullet(){
		
	}
	@Override 
	public  <T> void Copy(final T Src){
		final PlayerBullet Bullet = (PlayerBullet)Src;
		X = Bullet.X;
		Y = Bullet.Y;
		bFlip = Bullet.bFlip;
		CD.Copy(Bullet.CD);
		return;
	}

	public boolean CheckCollisionInTreeSlice(final GameGraph mGameGraph, final int SliceIndex){
		GameObject go = mGameGraph.ObjectGrid.Slices[SliceIndex].ObjectLists[3].RootObject;
		while(go != null){
			if(CollisionManager.Collision(CD, go.CD) && go.Info.bIsKillable == 1){
				if(go.Info.bIsKillable == 1)
					((DynamicObject)go).TakeDamage(); //TODO - Improve "killable"
				return true;
			}
			go = go.Next;			
		}
		return false;
	}
	//TODO - Try wrapping a "GetExtents()" function
	public boolean HandleCollisions(final GameGraph mGameGraph, final float Delta){
		final int SliceIndex = (int)(CD.Right / GameGraph.LevelTileOffset);
		if(mGameGraph.WithinGridX(SliceIndex) && CheckCollisionInTreeSlice(mGameGraph, SliceIndex)){
			return true;
		}else{
			final int LSliceIndex = (int)(CD.Left / GameGraph.LevelTileOffset);
			if(SliceIndex != LSliceIndex){
				return (mGameGraph.WithinGridX(LSliceIndex) && CheckCollisionInTreeSlice(mGameGraph, LSliceIndex));
			}else{
				return false;
			}
		}
	}
	@Override
	public boolean UpdateAndDraw(final SpriteBatch spriteBatch){
		final float Delta = ((bFlip) ? -PlayerBullet.DefaultSpeed : PlayerBullet.DefaultSpeed) * Globals.secondsSinceLastFrame;
		final GameGraph mGameGraph = Globals.mGameGraph;
		if(CD.Right < mGameGraph.DLeft || CD.Left > mGameGraph.DRight){
			return false;
		}else{
			final float WallSenseHeight = Y;
			if(mGameGraph.CollisionAgainstWall(CD, Delta, WallSenseHeight) || HandleCollisions(mGameGraph, Delta)){
				//Hit a solid object or wall
				mGameGraph.AddExplosion(X, Y, Explosion.PlayerShotImpact, bFlip);
				SoundSystem.PlaySound(PlayerBullet.LaserHit_SoundID);
				return false;
			}
			X += Delta;
			CD.Translate(Delta, 0.0f);
			spriteBatch.AddSprite(X - mGameGraph.CamX, Y - mGameGraph.CamY, SpriteIndex, DefaultScale, bFlip, null);
			return true;
		}
	}
}
