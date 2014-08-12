package com.Foofles;

public class BasicEnemy extends DynamicObject{
	public float YVelocity = 0.0f;
	public boolean bIsDormant = false;
	public float XVelocity = 0.0f;
	public float XAccel = 4.0f;
	public float CautionDistance = 1.05f;
	
	public float ShotBurstDelay = 0.0f;
	public float ShotBulletDelay = 0.0f;
	public int ShotCounter = 0;
	final public static float fInitialChargeDelay = 3.0f;
	public float fChargeDelay = fInitialChargeDelay;
	public boolean bPreviousFrameFlip = !bFlip;
	public BasicEnemy(){
		Type = GameObjectTypes.Enemy;
	}

	@Override
	public boolean Update(){
		final GameGraph mGameGraph = Globals.mGameGraph;
		if(bIsDormant){
			//TODO - Work around this hack
			mAnimation.bIsPlaying = false;
			final float PlayerX = mGameGraph.mPlayer.X;
			if(FoofMath.SquardDistance(X, Y, PlayerX, mGameGraph.mPlayer.Y) < 0.81f){
				bIsDormant = false;
				mAnimation.bIsPlaying = true;
			}else{
				return true;
			}
		}
		//TODO - Consider putting this in the damage collision cases
		//IT would remove this check every frame and a couple of other lines of code
		if(HP < 1){
			mGameGraph.AddExplosion(X, Y, Explosion.SmokeExplosion, false);
			Score.AddPoints(150);
			mGameGraph.ScoreToaster.AddToast(X, Y, ScoreToast.pts150);
			return false;
		}
		float XDelta = 0.0f;
		
		//Only compute charging/shooting when the enemy
		//is onscreen so we don't gank the player
		final EnemyInfo EI = (EnemyInfo)Info;
		//TODO - Think about wrapping "IsOnScreen()" function

		final int Agro = ((EnemyInfo)Info).Agro;
		//TODO - try to clean this up a bit
		if(Agro > 0 && (CD.Left < mGameGraph.DRight || CD.Right > mGameGraph.DLeft)){
			final float PlayerX = mGameGraph.mPlayer.X;
			bFlip = (X < PlayerX);
			if(Agro == 1){  // 1 = fully aggressive, charge!
				float VelocityAdd = 0.0f;
				if(bFlip){
					if(XVelocity < EI.Speed){
						VelocityAdd = Globals.secondsSinceLastFrame * XAccel;
					}
				}else{
					if(XVelocity > -EI.Speed){
						VelocityAdd = -Globals.secondsSinceLastFrame * XAccel;
					}
				}
				if(VelocityAdd * XVelocity < 0.0f){
					VelocityAdd *= 0.5f; //Drag a bit
				}
				XVelocity += VelocityAdd;
			}else{  // More cautious, maintain distance
					final float SafeSpot = (bFlip) ? PlayerX - CautionDistance : PlayerX + CautionDistance;
					if(X > SafeSpot){
						if(XVelocity > -EI.Speed){
							XVelocity -= Globals.secondsSinceLastFrame * XAccel;
						}
					}else{
						if(XVelocity < EI.Speed){
							XVelocity += Globals.secondsSinceLastFrame * XAccel;
						}
					}
				
			}

			XDelta = XVelocity * Globals.secondsSinceLastFrame;
			if(!((CD.Left + XDelta < VisibilityMin) || (CD.Right + XDelta > VisibilityMax))){
				X += XDelta;
			}else{
				XDelta = 0.0f;
			}		
		}else{
			if(EI.Speed > 0.01f){
				if(bFlip){
					XDelta = EI.Speed * Globals.secondsSinceLastFrame;
				}else{
					XDelta = -EI.Speed * Globals.secondsSinceLastFrame;
				}
			}
			if(!((!bFlip && CD.Left < VisibilityMin) || (bFlip && CD.Right > VisibilityMax))){
				X += XDelta;
			}else{
				XDelta = 0.0f;
				bFlip = !bFlip;
			}			
		}
		bPreviousFrameFlip = bFlip;
		float YDelta = 0.0f;
		//Gravity!
		YVelocity += Globals.fGravity;
		YDelta = YVelocity * Globals.secondsSinceLastFrame;
		final float FutureFeet = CD.Bottom + YDelta;
		final float TileHeight = mGameGraph.FindFloorIntersectionHeight(CD.Left, CD.Right, FutureFeet);
		final float HalfHeight = Y - CD.Bottom;

		if(FutureFeet > TileHeight){
			Y += YDelta;
		}else{
			final float NewPos = TileHeight + HalfHeight + 0.001f; //Add a little to put us a tile higher
			YDelta = NewPos - Y;
			Y = NewPos;
			YVelocity = 0.0f;
		}
		
		CD.Translate(XDelta, YDelta);
		
		//Shoot!
		if(EI.ShotType > -1){
			if(ShotBurstDelay > EI.ShotDelay){
				if(ShotCounter < EI.nShotsPerBurst){
					if(ShotBulletDelay > 0.2f){
						final GameObjectInfo ProjInfo = mGameGraph.ProjectileSet[EI.ShotType];
						final float ProjectileVelocityX = (bFlip) ? 3.0f : -3.0f;
						final float ProjectileVelocityY = 0.0f;
						mGameGraph.AddEnemyProjectile(X, Y, ProjectileVelocityX, ProjectileVelocityY, ProjInfo, bFlip);
						ShotBulletDelay = 0.0f;
						ShotCounter++;
					}else{
						ShotBulletDelay += Globals.secondsSinceLastFrame;
					}
				}else{
					ShotBurstDelay = 0.0f;
					ShotCounter = 0;
				}
			}else{
				ShotBurstDelay += Globals.secondsSinceLastFrame;
			}
		}
		return true;
	}
	@Override
	public void TakeDamage() {
			if(bIsDormant){
				bIsDormant = false; //Wakey wakey, eggs and bac-ey!
				mAnimation.bIsPlaying = true;
			}
			HP--;
	}
}
