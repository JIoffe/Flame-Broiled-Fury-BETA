package com.Foofles;

public class FallingSpike extends DynamicObject {
	public byte bIsFalling = 0;
	final static public float FallSpeed = 1.0f;
	public FallingSpike(){
		mAnimation.Behaviour = AnimationType.Behaviour_PingPong;
		mAnimation.bIsPlaying = false;
		Type = GameObjectTypes.Enemy;
	}
	@Override
	public boolean Update(){
		if(bIsFalling == 1){
			final float YDelta = -FallSpeed * Globals.secondsSinceLastFrame;
			final float SpikeBottom = CD.Bottom + YDelta;
			final float FloorHeight = Globals.mGameGraph.FindFloorIntersectionHeight(CD.Left, CD.Right, SpikeBottom);
			if(SpikeBottom < FloorHeight){
				Globals.mGameGraph.AddExplosion(X, Y, Explosion.SmokeExplosion, bFlip);
				SoundSystem.PlaySound(Explosion.SmokeExplosion_SoundID);
				Remove();
				return false;
			}else{
				Y += YDelta;
				CD.Translate(0.0f, YDelta);
			}
		}else{
			final float PlayerX = Globals.mGameGraph.mPlayer.X;
			if(PlayerX > CD.Left && PlayerX < CD.Right){
				bIsFalling = 1;
			}
		}
		return true;
	}
}
