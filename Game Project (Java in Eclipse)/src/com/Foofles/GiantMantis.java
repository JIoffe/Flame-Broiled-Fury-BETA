package com.Foofles;

public class GiantMantis extends DynamicObject{
	
	final public Mover RightClaw = new Mover();
	final public Mover LeftClaw = new Mover();
	
	final private float ClawMinimum = -0.32f;
	private float RShoulderX = 0.0f;
	private float RShoulderY = 0.0f;
	private float RShoulderAngle = 0.0f;
	private float LShoulderAngle = 0.0f;
	private float LClawAngle = 0.0f;
	private float RClawAngle = 0.0f;
	private float LShoulderX = 0.0f;
	private float LShoulderY = 0.0f;
	private float HeadY = 0.0f;
	private int State = 0;
	//0 = Standard Smashing
	//1 = About to bite
	//2 = Head in ground like an ostrich
	
	private float BiteTimer = 0.0f;
	private int PreviousHP = 0;
	public GiantMantis(final float newX, final float newY){
		X = newX;
		Y = newY;
		VisibilityMin = X; 
		VisibilityMax = X;
		
		RightClaw.OriginX = X+0.32f;
		RightClaw.OriginY = Y+0.07f;//Y-0.1f;
		RightClaw.X = RightClaw.OriginX;
		RightClaw.Y = RightClaw.OriginY;
		RightClaw.uMoverType = Mover.Slammer;
		RightClaw.uFunctionType = Mover.FunctionSin;
		RightClaw.Pause = 1.5f;
		RightClaw.Width = 0.32f;
		RightClaw.fSpeed = 2.4f;
		RightClaw.Sync = 0.0f;
		RightClaw.bRender = 0;

		
		LeftClaw.OriginX = X-0.32f;
		LeftClaw.OriginY = Y+0.07f;
		LeftClaw.X = LeftClaw.OriginX;
		LeftClaw.Y = LeftClaw.OriginY;
		LeftClaw.uMoverType = Mover.Slammer;
		LeftClaw.uFunctionType = Mover.FunctionSin;
		LeftClaw.Pause = 1.5f;
		LeftClaw.Width = 0.32f;
		LeftClaw.fSpeed = 2.4f;
		LeftClaw.Sync = 0.5f;		
		LeftClaw.bRender = 0;

		HeadY = Y + 0.4f;
		HP = 12;
		return;
	}
	@Override
	public void InitCD(){
		RightClaw.X = RightClaw.OriginX;
		RightClaw.Y = RightClaw.OriginY;
		LeftClaw.X = LeftClaw.OriginX;
		LeftClaw.Y = LeftClaw.OriginY;
		CD.Left = -0.44f;
		CD.Right = 044f;
		CD.Top = 0.0f;
		CD.Bottom = -0.0f;
		CD.Translate(X, Y);	
		CD.bIgnore = 1;
		
		RightClaw.CD.Left = -0.0f;
		RightClaw.CD.Right = 0.6f;
		RightClaw.CD.Top = 1.0f;
		RightClaw.CD.Bottom = -1.0f;
		RightClaw.CD.Scale(0.2f);
		RightClaw.CD.Translate(RightClaw.X, RightClaw.Y);
		RightClaw.CD.bIgnore = 0;
		
		LeftClaw.CD.Left = -0.6f;
		LeftClaw.CD.Right = 0.0f;
		LeftClaw.CD.Top = 1.0f;
		LeftClaw.CD.Bottom = -1.0f;
		LeftClaw.CD.Scale(0.2f);
		LeftClaw.CD.Translate(LeftClaw.X, LeftClaw.Y);
		LeftClaw.CD.bIgnore = 0;
		
		//CD.NextCD = RightClaw.CD;
		//RightClaw.CD.NextCD = LeftClaw.CD;
		RightClaw.Type = GameObjectTypes.Enemy;
		RightClaw.Info = Info;
		Globals.mGameGraph.ObjectGrid.SortObject(RightClaw);		
		LeftClaw.Type = GameObjectTypes.Enemy;
		LeftClaw.Info = Info;
		Globals.mGameGraph.ObjectGrid.SortObject(LeftClaw);
	}
	@Override 
	public boolean Update(){
		if(HP <= 0){
			Globals.mGameGraph.AddExplosion(X, Y, Explosion.FieryExplosion, false);
			Score.AddPoints(150);
			Globals.mGameGraph.ScoreToaster.AddToast(X, Y, ScoreToast.pts150);
			RightClaw.ListOwner.Remove(RightClaw);
			LeftClaw.ListOwner.Remove(LeftClaw);
			return false;
		}
		switch(State){
			case 0:{
				RightClaw.Update();
				LeftClaw.Update();
				if(Math.abs(X - Globals.mGameGraph.mPlayer.X) < 0.06f){
					State = 1;
					RightClaw.CD.bIgnore = 1;
					LeftClaw.CD.bIgnore = 1;
				}
				break;
			}
			case 1:{
				BiteTimer += Globals.secondsSinceLastFrame;
				if(BiteTimer > 2.5f){
					State = 2;
					PreviousHP = HP;
					BiteTimer = 0.0f;
					
					//We have a CD now!
					CD.Left = -1.0f;
					CD.Right = 1.0f;
					CD.Top = 1.0f;
					CD.Bottom = -1.0f;
					
					CD.Scale(0.15f);
					CD.Translate(X, Y - 0.29f);
					CD.bIgnore = 0;
				}
				break;
			}
			case 2:{
				//In the ground, can get hit a bunch of times
				//or will automatically reset after 5 seconds
				BiteTimer += Globals.secondsSinceLastFrame;
				if(PreviousHP - HP > 6 || BiteTimer > 5.0f){
					State = 0;
					BiteTimer = 0.0f;
					InitCD();
				}
				break;
			}
		}
		return true;
	}
	@Override
	void Render(final float CamX, final float CamY, final SpriteBatch mSpriteBatch){
		final float CenterX = X - CamX;
		final float CenterY = Y - CamY;
		final int SpriteID = (mAnimation.bIsPlaying) ? mAnimation.AdvanceFrame() : Info.nSpriteID;
	
		if(DamageFlash > 0.0f){
			DamageFlash -= Globals.secondsSinceLastFrame;
			RenderDamageFlash(CenterX, CenterY-0.08f, SpriteID+4, 0.65f, mSpriteBatch);
			return;
		}
		mSpriteBatch.AddSprite(CenterX, CenterY, SpriteID+1, 0.4f, bFlip, null);
		
		if(State == 0){
			//TODO - Reduce pointless computations
			final float WiggleFactor = (float)Math.sin(5.0f*Globals.runtimeSeconds);
			final float WiggleAngle = 8.0f * WiggleFactor;
			//Right hand
			///mSpriteBatch.AddSprite(CenterX+0.15f, CenterY+0.14f, SpriteID+3, 0.16f, false, null);
			float S = (RightClaw.Mod - ClawMinimum) / 0.64f;
			RShoulderAngle = FoofMath.Lerp(0.0f, 100.0f - WiggleAngle, S) * FoofMath.DegToRad;
			RShoulderY = FoofMath.Lerp(0.03f, 0.445f, S) + Y;
			mSpriteBatch.AddRotatingSprite(CenterX+0.15f, RShoulderY - CamY, SpriteID+3, 0.16f, (float)Math.cos(RShoulderAngle), (float)Math.sin(RShoulderAngle), false);
			
			float ClawOffset = FoofMath.Lerp(RightClaw.X - 0.085f, RightClaw.X + 0.04f, S);
			RClawAngle = FoofMath.Lerp(-30.0f, WiggleAngle, S) * FoofMath.DegToRad;
			mSpriteBatch.AddRotatingSprite(ClawOffset - CamX + 0.02f * WiggleFactor, RightClaw.Y - CamY, SpriteID, 0.2f, (float)Math.cos(RClawAngle), (float)Math.sin(RClawAngle), false);
			
			//Left Hand
			S = (LeftClaw.Mod - ClawMinimum) / 0.64f;
			LShoulderAngle = FoofMath.Lerp(360.0f, 260.0f -WiggleAngle, S) * FoofMath.DegToRad;
			LShoulderY = FoofMath.Lerp(0.03f, 0.445f, S) + Y;
			mSpriteBatch.AddRotatingSprite(CenterX-0.15f, LShoulderY - CamY, SpriteID+3, 0.16f, (float)Math.cos(LShoulderAngle), (float)Math.sin(LShoulderAngle), true);
			
			ClawOffset = FoofMath.Lerp(LeftClaw.X + 0.085f, LeftClaw.X - 0.04f, S);
			LClawAngle = FoofMath.Lerp(30.0f, WiggleAngle, S) * FoofMath.DegToRad;
			mSpriteBatch.AddRotatingSprite(ClawOffset - CamX + 0.02f * WiggleFactor, LeftClaw.Y - CamY, SpriteID, 0.2f, (float)Math.cos(LClawAngle), (float)Math.sin(LClawAngle),true);
			
			//Head
			HeadY = FoofMath.Lerp(HeadY, Y + 0.4f, 2.5f * Globals.secondsSinceLastFrame);
			mSpriteBatch.AddRotatingSprite(CenterX, HeadY - CamY, SpriteID+2, 0.15f, Collectible.CosRot, Collectible.SinRot, false);
		}else{
			//Pull limbs into a more neutral position
			final float S = Globals.secondsSinceLastFrame;
			LShoulderY = FoofMath.Lerp(LShoulderY, Y+0.25f, S);
			RShoulderY = FoofMath.Lerp(RShoulderY, Y+0.25f, S);
			RightClaw.Y = FoofMath.Lerp(RightClaw.Y, Y+0.07f, S);
			RightClaw.X = FoofMath.Lerp(RightClaw.X, X+0.32f, S);
			
			LeftClaw.Y = FoofMath.Lerp(LeftClaw.Y, Y+0.07f, S);
			LeftClaw.X = FoofMath.Lerp(LeftClaw.X, X-0.32f, S);
			
			LShoulderAngle = FoofMath.Lerp(LShoulderAngle, 360.0f*FoofMath.DegToRad, S);
			RShoulderAngle = FoofMath.Lerp(RShoulderAngle, 0.0f, S);
			RClawAngle = FoofMath.Lerp(RClawAngle, 0.0f, S);
			LClawAngle = FoofMath.Lerp(LClawAngle, 0.0f, S);
			mSpriteBatch.AddRotatingSprite(CenterX-0.15f, LShoulderY - CamY, SpriteID+3, 0.16f, (float)Math.cos(LShoulderAngle), (float)Math.sin(LShoulderAngle), true);
			mSpriteBatch.AddRotatingSprite(LeftClaw.X - CamX, LeftClaw.Y - CamY, SpriteID, 0.2f, (float)Math.cos(LClawAngle), (float)Math.sin(LClawAngle),true);
			//Right
			mSpriteBatch.AddRotatingSprite(CenterX+0.15f, RShoulderY - CamY, SpriteID+3, 0.16f, (float)Math.cos(RShoulderAngle), (float)Math.sin(RShoulderAngle), false);
			mSpriteBatch.AddRotatingSprite(RightClaw.X - CamX, RightClaw.Y - CamY, SpriteID, 0.2f, (float)Math.cos(RClawAngle), (float)Math.sin(RClawAngle), false);
			
			if(State == 1){
				HeadY = FoofMath.Lerp(HeadY, Y + 0.5f, 0.4f * Globals.secondsSinceLastFrame);
				mSpriteBatch.AddSprite(CenterX, HeadY - CamY, SpriteID+2, 0.15f,false,null);
			}else{
				HeadY = FoofMath.Lerp(HeadY, Y - 0.29f, 3.0f * Globals.secondsSinceLastFrame);
				mSpriteBatch.AddSprite(CenterX, HeadY - CamY, SpriteID+2, 0.17f,false,null);
			}
		}

		return;
	}
}
