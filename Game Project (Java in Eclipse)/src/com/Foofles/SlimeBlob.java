package com.Foofles;

public class SlimeBlob extends DynamicObject implements UpdateRender, Copyable{
	final FoofTimer HopTimer = new FoofTimer(1.0f, false);
	SmartArray<SlimeBlob> Owner = null;
	int OwnerIndex = 0;
	public SlimeBlob(){
		HP = 1;
		Info = Globals.mGameGraph.DynamicObjectSet[9];
		Type = GameObjectTypes.Enemy;
		bIsPlatform = 0;
		mAnimation.Animations = Info.Animations;
		mAnimation.bIsPlaying = true;
		mAnimation.Behaviour = AnimationType.Behaviour_PingPong;
		VelocityX = 0.0f;
		VelocityY = 0.0f;
		VelocityX = 0.0f;
		//mAnimation.Animations = Globals.mGameGraph.AnimationTrackSet[4];
	}

	@Override
	public boolean UpdateAndDraw(SpriteBatch spriteBatch) {
		//Stub just to use the SmartARray
		return false;
	}

	@Override
	public <T> void Copy(T Src) {
		final SlimeBlob SrcBlob = (SlimeBlob)Src;
		X = SrcBlob.X;
		Y = SrcBlob.Y;
		VelocityY = SrcBlob.VelocityY;
		VelocityX = SrcBlob.VelocityX;
		VisibilityMin = SrcBlob.VisibilityMin;
		VisibilityMax = SrcBlob.VisibilityMax;
		CD.Copy(SrcBlob.CD);
		HP = SrcBlob.HP;
	}
	@Override
	public boolean Update(){
		if(HP < 1){
			Globals.mGameGraph.AddExplosion(X, Y, Explosion.SmokeExplosion, false);
			Owner.Remove(OwnerIndex);
			return false;
		}
		final GameGraph mGameGraph = Globals.mGameGraph;
		if(HopTimer.Update()){
			VelocityY = 1.6f;
			HopTimer.bOn = false;
			if(X < mGameGraph.mPlayer.X){
				VelocityX = 1.4f;
			}else{
				VelocityX = -1.4f;
			}
		}
		VelocityY += Globals.fGravity;
		//Drag? I guess
		VelocityX = FoofMath.Lerp(VelocityX, 0.0f, 0.3f * Globals.secondsSinceLastFrame);
		final float DeltaY = VelocityY * Globals.secondsSinceLastFrame;
		final float DeltaX = VelocityX * Globals.secondsSinceLastFrame;
		final float FutureX = X + DeltaX;
		final float Feet = CD.Bottom + DeltaY;
		if(mGameGraph.GetTileHeight(FutureX, Feet) > Feet){
			VelocityY = 0.0f;
			VelocityX = 0.0f;
			HopTimer.bOn = true;
		}else{
			//Fallllll
			Y += DeltaY;
			X = FutureX;
			CD.Translate(DeltaX, DeltaY);
		}
		ComputeVisibleArea();
		return true;
	}
}
