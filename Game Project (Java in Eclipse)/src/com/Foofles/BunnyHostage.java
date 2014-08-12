package com.Foofles;

/**
 * 
 * The player must free these bunny hostages to obtain 100% completion
 * and a good ending. The cage can take a small amount of damage before 
 * shattering and releasing the cute bunny occupant.
 * 
 * Bunnies will be different colors, to make life easier in organizing
 * what you have and don't have.
 * @author Jason Ioffe
 *
 */
public class BunnyHostage extends DynamicObject{
	int BunnyIndex = 0;
	private boolean bBroken = false;
	public BunnyHostage(final int BunIndex){
		Info = FoofGame.BunnyCageInfo;
		HP = 2;
		Type = GameObjectTypes.Enemy;
		bIsPlatform = 1;
		bBroken = false;
		BunnyIndex = BunIndex;
	}
	@Override
	public boolean Update(){
		if(HP < 1){
			if(!bBroken){
				Globals.mGameGraph.AddExplosion(X, Y, Explosion.CollectionPuftA, bFlip);
				PlayerProgress.RescueBunny();
				VelocityY = 2.5f;
				bBroken = true;
			}else{
				if(Y < Globals.mGameGraph.CamY - 1.0f){
					return false;
				}else{
					VelocityY += Globals.fGravity;
					Y += VelocityY * Globals.secondsSinceLastFrame;
				}
			}
		}
		return true;
	}
	@Override
	public void Render(final float CamX, final float CamY, final SpriteBatch mSpriteBatch){
		final float RenderX = X - CamX;
		final float RenderY = Y - CamY;
		if(!bBroken){
			mSpriteBatch.AddRotatingSprite(RenderX, RenderY, GameGUI.HamburgerFlareIndex, 0.3f, GameGUI.FlareRotCos, GameGUI.FlareRotSin, false);
			mSpriteBatch.AddRotatingSprite( RenderX, RenderY, FoofGame.BunnyHostageInfo.nSpriteID + BunnyIndex, FoofGame.BunnyHostageInfo.fDefaultScale, Collectible.CosRot, Collectible.SinRot, false);
			mSpriteBatch.AddRotatingSprite( RenderX, RenderY, Info.nSpriteID, Info.fDefaultScale, Collectible.CosRot, Collectible.SinRot, false);
		}else{
			mSpriteBatch.AddSprite( RenderX, RenderY, FoofGame.BunnyHostageInfo.nSpriteID + BunnyIndex, FoofGame.BunnyHostageInfo.fDefaultScale, false, null);
		}
		return;
	}
}
