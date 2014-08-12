package com.Foofles;

public class EnemyProjectile extends DynamicObject implements Copyable, UpdateRender{
	public int nSAIndex = 0;
	public float VelocityX = 0.0f;
	public float VelocityY = 0.0f;
	
	public EnemyProjectile(){
		Type = GameObjectTypes.Enemy;
	}
	
	@Override 
	public boolean Update(){
		//TODO - IF COLLIDES WITH WORLD
		final float DeltaX = Globals.secondsSinceLastFrame * VelocityX;
		final float DeltaY = Globals.secondsSinceLastFrame * VelocityY;
		final GameGraph mGameGraph = Globals.mGameGraph;
		if(CD.Right < mGameGraph.DLeft || CD.Left > mGameGraph.DRight){
			return false;
		}else{
			X += DeltaX;
			Y += DeltaY;
			CD.Translate(DeltaX, DeltaY);
			return true;
		}
	}
	@Override
	public void Remove(){
		super.Remove();
		//Also remove from its smart array
		Globals.mGameGraph.mEnemyProjectiles.Remove(nSAIndex);
	}
	@Override
	public boolean UpdateAndDraw(final SpriteBatch spriteBatch) {
		
		return false;
	}

	@Override
	public <T> void Copy(final T Src) {
		EnemyProjectile src = (EnemyProjectile)Src;
		X = src.X;
		Y = src.Y;
		Info = src.Info;
		
		bFlip = src.bFlip;
		CD.Copy(src.CD);
	}

}
