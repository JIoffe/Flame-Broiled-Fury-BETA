package com.Foofles;

/**
 *  This expands on the particle system to create a "sparkling" effect
 *  eg. bubbling lava, sparkling diamonds, etc. particles that 
 *  randomly appear along an object's surface
 * @author Jason Ioffe
 *
 */
public class SparkleParticleSystem extends DynamicObject {
	public FoofTimer SparkleTimer = null;
	public SmartArray<Particle> mParticles = null;
	
	public SparkleParticleSystem(int Max, float Delay, int newImageID) {
		mAnimation.bIsPlaying = true;
		try {
			mParticles = new SmartArray<Particle>(Max, Particle.class);
			SparkleTimer = new FoofTimer(Delay, true);
		} catch (IllegalAccessException e) {
			// TODO Handle Error - this is a bit of a problem
		} catch (InstantiationException e) {

		}		
	}
	
	@Override
	public boolean Update(){
		if(SparkleTimer.Update()){
			final Particle mNewParticle = mParticles.Add();
			if(mNewParticle != null){
				//TODO - Make this open ended, we're not just here for farting
				mNewParticle.Lifetime = 0.9f;
				mNewParticle.ImageID = Info.nSpriteID;
				mNewParticle.Scale = 0.014f;
				mNewParticle.X = FoofMath.RandFloat(CD.Left, CD.Right);
				mNewParticle.Y = FoofMath.RandFloat(CD.Bottom, CD.Top);
				mNewParticle.ScaleShift = 0.16f;
				mNewParticle.HalfLifespan = mNewParticle.Lifetime * 0.5f;
				mNewParticle.VelocityX = 0.0f;
				mNewParticle.mAnimation.bIsPlaying = true;
				mNewParticle.mAnimation.Animations[0] = mAnimation.Animations[0];
				mNewParticle.mAnimation.fCurrentFrame = 0.0f;
				mNewParticle.VelocityY = 0.0f;
			}
		}		   
		return true;
	}
	
	@Override
	void Render(final float CamX, final float CamY, final SpriteBatch mSpriteBatch){
		mParticles.UpdateAndDraw(mSpriteBatch);
		return;
	}
}
