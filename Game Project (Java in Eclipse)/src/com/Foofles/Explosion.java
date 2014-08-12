package com.Foofles;

public class Explosion implements Copyable, UpdateRender {
	public static final AnimationTrack CollectionPuftA = new AnimationTrack();
	public static final AnimationTrack FieryExplosion = new AnimationTrack();
	public static final AnimationTrack SmokeExplosion = new AnimationTrack();
	public static final AnimationTrack PlayerShotImpact = new AnimationTrack();
	public static final AnimationTrack PlayerMuzzle = new AnimationTrack();
	public static int SmokeExplosion_SoundID = 0;
	public static int SmokeExplosion_SpriteIndex = 0;
	public byte uType; //it's not really unsigned but we pretend it is
	public float X;
	public float Y;
	public float Life;
	public float Scale = 0.25f;
	public boolean bFlip = false;
	
	final public AnimationComponent mAnimation = new AnimationComponent();
	
	public static void PrepareAnimationTracks(final TextureAtlas mTextureAtlas){
        SmokeExplosion.iStartingFrame = mTextureAtlas.GetIndexByTitle("smokeexplosion_1.png");
        SmokeExplosion.fDefaultFPS = 12.0f;
        SmokeExplosion.nFrames = 5;
        
        CollectionPuftA.iStartingFrame = mTextureAtlas.GetIndexByTitle("coinpuft1.png");
        CollectionPuftA.fDefaultFPS = 18.0f;
        CollectionPuftA.nFrames = 12;
        
        FieryExplosion.iStartingFrame = mTextureAtlas.GetIndexByTitle("fieryexplosion1.png");
        FieryExplosion.fDefaultFPS = 12.0f;
        FieryExplosion.nFrames = 10;
        
        PlayerShotImpact.iStartingFrame = mTextureAtlas.GetIndexByTitle("laserblast_1.png");
        PlayerShotImpact.fDefaultFPS = 12.0f;
        PlayerShotImpact.nFrames = 5;
        
        PlayerMuzzle.iStartingFrame = mTextureAtlas.GetIndexByTitle("muzzleflash1.png");
        PlayerMuzzle.fDefaultFPS = 18.0f;
        PlayerMuzzle.nFrames = 5;
	}

	public Explosion(){
		mAnimation.Animations = new AnimationTrack[1];
		mAnimation.CurrentTrack = 0;
		mAnimation.Behaviour = AnimationType.Behaviour_OneShot;
	}
	public void Copy(final Explosion src){
		X = src.X;
		Y = src.Y;
		Life = src.Life;
		Scale = src.Scale;
		uType = src.uType;
		mAnimation.Animations[0] = src.mAnimation.Animations[0];
		mAnimation.bIsPlaying = src.mAnimation.bIsPlaying;
		mAnimation.fCurrentFrame = src.mAnimation.fCurrentFrame;
		return;
	}

	@Override
	public boolean UpdateAndDraw(final SpriteBatch spriteBatch) {
		final GameGraph mGameGraph = Globals.mGameGraph;
		if(X < mGameGraph.DRight &&
				X  > mGameGraph.DLeft && mAnimation.bIsPlaying){
			spriteBatch.AddSprite(X - mGameGraph.CamX, Y - mGameGraph.CamY, mAnimation.AdvanceFrame(), Scale, bFlip, null);
			if(mAnimation.Animations[0] == Explosion.PlayerMuzzle){
				if(bFlip){
					X -= 0.3f * Globals.secondsSinceLastFrame;
				}else{
					X += 0.3f * Globals.secondsSinceLastFrame;
				}
			}
			return true;
		}else{	
			return false;
		}
	}

	@Override
	public <T> void Copy(final T Src) {
		final Explosion src = (Explosion)Src;
		X = src.X;
		Y = src.Y;
		Life = src.Life;
		Scale = src.Scale;
		uType = src.uType;
		mAnimation.bIsPlaying = src.mAnimation.bIsPlaying;
		mAnimation.fCurrentFrame = src.mAnimation.fCurrentFrame;
		mAnimation.Animations[0] = src.mAnimation.Animations[0];
		bFlip = src.bFlip;
	}
}
