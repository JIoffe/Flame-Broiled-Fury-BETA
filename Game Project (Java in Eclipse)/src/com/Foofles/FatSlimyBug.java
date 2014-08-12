package com.Foofles;

public class FatSlimyBug extends DynamicObject {
	SmartArray<SlimeBlob> BlobArray;	
	private FoofTimer FartTimer = new FoofTimer(2.0f, true);
	public FatSlimyBug(final float newX, final float newY){
		X = newX;
		Y = newY;
		VisibilityMin = X - 0.5f;
		VisibilityMax = X + 0.5f;
		
		try {
			BlobArray = new SmartArray<SlimeBlob>(10, SlimeBlob.class);
		} catch (IllegalAccessException e) {
			//TODO - HANDLE EXCEPTION
		} catch (InstantiationException e) {
		}
	}
	@Override
	public boolean Update(){
		if(HP <= 0){
			Globals.mGameGraph.AddExplosion(X, Y, Explosion.FieryExplosion, false);
			Score.AddPoints(150);
			Globals.mGameGraph.ScoreToaster.AddToast(X, Y, ScoreToast.pts150);
			return false;
		}
		if(DamageFlash > 0.0f){
			DamageFlash -= Globals.secondsSinceLastFrame;
		}		
		if(FartTimer.Update()){
			//Spawn a new baddie
			final SlimeBlob NewBlob = BlobArray.Add();
			if(NewBlob != null){
				final float BaddyX = FoofMath.RandFloat(CD.Left, CD.Right);
				final float BaddyY = FoofMath.RandFloat(CD.Bottom, CD.Top);
				Globals.mGameGraph.AddExplosion(BaddyX, BaddyY, Explosion.SmokeExplosion, false);
				NewBlob.X = BaddyX;
				NewBlob.Y = BaddyY;
				NewBlob.VelocityX = FoofMath.RandFloat(-1.0f, 1.0f);
				NewBlob.InitCD();
				NewBlob.ComputeVisibleArea();
				NewBlob.Owner = BlobArray;
				NewBlob.OwnerIndex = BlobArray.nIndexLast;
				NewBlob.HP = 1;
				Globals.mGameGraph.ObjectGrid.SortObject(NewBlob);
				Globals.mGameGraph.ObjectGrid.SortDynamicPVS(NewBlob);
			}
		}
		if(Globals.mGameGraph.mPlayer.X > X){
			bFlip = true;
			if(VelocityX < 0.05f){
				VelocityX += 0.2f * Globals.secondsSinceLastFrame;
			}
		}else{
			bFlip = false;
			if(VelocityX > -0.05f){
				VelocityX -= 0.2f * Globals.secondsSinceLastFrame;
			}
		}
		final float DeltaX = VelocityX * Globals.secondsSinceLastFrame;
		X += DeltaX;
		CD.Translate(DeltaX, 0.0f);
		ComputeVisibleArea();
		return true;
	}
	@Override
	public void Render(final float CamX, final float CamY, final SpriteBatch mSpriteBatch){
		if(DamageFlash > 0.0f){
			RenderDamageFlash(X-CamX, Y-CamY, Info.nSpriteID+1, Info.fDefaultScale,mSpriteBatch);
		}else{
			super.Render(CamX, CamY, mSpriteBatch);
		}
	}
}
