package com.Foofles;

public class Collectible extends GameObject{
	static public int CollectibleA_SoundID = 0;
	//Dancing collectible vars
	public static float CosRot = 0.0f;
	public static float SinRot = 0.0f;
	public static float BounceY = 0.0f;
	Collectible(){
		Type = GameObjectTypes.Collectible;
	}
	@Override
	void Render(final float CamX, final float CamY, final SpriteBatch mSpriteBatch){
		final float RenderY = Y - CamY + Collectible.BounceY;
		//Draw a flare for really special objects like double cheeseburgers!
		if(Info.bIsSpecial == 1){
			mSpriteBatch.AddRotatingSprite(X - CamX, RenderY, GameGUI.HamburgerFlareIndex, 0.25f, GameGUI.FlareRotCos, GameGUI.FlareRotSin, false);
		}
		mSpriteBatch.AddRotatingSprite( X - CamX, RenderY, Info.nSpriteID, Info.fDefaultScale, Collectible.CosRot, Collectible.SinRot, false);
		return;
	}
}
