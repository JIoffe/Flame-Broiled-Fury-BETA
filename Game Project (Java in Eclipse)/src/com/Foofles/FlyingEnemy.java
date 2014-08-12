package com.Foofles;

/**
 * KILL IT! KILL IT WITH FIRE!
 * @author Jason Ioffe
 *
 */
public class FlyingEnemy  extends DynamicObject{
	public float OriginY = 0.0f;
	public float DamageFlash = 1.0f;
	public float YVelocity = 0.0f;
	public float XVelocity = 0.0f;
	public float XAccel = 4.0f;
	public float CautionDistance = 1.05f;
	
	public float ShotBurstDelay = 0.0f;
	public float ShotBulletDelay = 0.0f;
	public int ShotCounter = 0;
	final public static float fInitialChargeDelay = 3.0f;
	public float fChargeDelay = fInitialChargeDelay;
	public boolean bPreviousFrameFlip = !bFlip;
	
	private float bPlayerSighted = 0.0f;
	private float RandSeed = 0.0f;
	
	public FlyingEnemy(){
		Type = GameObjectTypes.Enemy;
		RandSeed = FoofMath.RandFloat(0.0f, 200.0f);
	}
	@Override
	public boolean Update(){
		
		final GameGraph mGameGraph = Globals.mGameGraph;
		final Player mPlayer = mGameGraph.mPlayer;
		if(HP < 1){
			mGameGraph.AddExplosion(X, Y, Explosion.SmokeExplosion, bFlip);
			Score.AddPoints(150);
			mGameGraph.ScoreToaster.AddToast(X, Y, ScoreToast.pts150);
			return false;
		}
		final EnemyInfo EI = (EnemyInfo)Info;
		if(Math.abs(OriginY - mPlayer.Y) < 0.9f){
			final float OldY = Y;
			final float Speed = 0.2f;
			if(bPlayerSighted == 0.0f){
				bFlip = (X < mPlayer.X);
				bPlayerSighted = ((bFlip) ? 1.0f : -1.0f);
			}
			switch(EI.Agro){
				case 1:{
					Y = OriginY + (float)Math.sin(15*(X + RandSeed))*0.2f;
					break;
				}
				default:
					break;
			}
			final float DeltaX = Speed * bPlayerSighted * Globals.secondsSinceLastFrame;
			X += DeltaX;
			CD.Translate(DeltaX, Y - OldY);
			VisibilityMin = X - Info.fDefaultScale;
			VisibilityMax = X + Info.fDefaultScale;
			
			//Shoot!
			if(EI.ShotType > -1){
				if(ShotBurstDelay > EI.ShotDelay){
					if(ShotCounter < EI.nShotsPerBurst){
						if(ShotBulletDelay > 0.2f){
							final GameObjectInfo ProjInfo = mGameGraph.ProjectileSet[EI.ShotType];
							final float ProjectileVelocityX = 1.5f * bPlayerSighted;
							final float ProjectileVelocityY = -1.5f;
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
		}
		return true;
	}
}
