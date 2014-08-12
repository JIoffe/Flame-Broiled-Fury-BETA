package com.Foofles;
/**
 * A simple particle generator that emits particles of random size/speed
 * for a given time. 
 * 
 * @author Jason Ioffe
 *
 */

//TODO - BE MORE FLEXIBLE AND MEMORY EFFICIEN WITH THE IMAGE ID
public class ParticleSystem implements Copyable, UpdateRender {
	final public static byte OneShot = 1; // Dies when all particles die
	final public static byte Flow = 2;

	public FoofTimer SprayTimer = null;
	public SmartArray<Particle> mParticles = null;
	byte Type;
	public float X;
	public float Y;
	
	public float Lifespan;
	public int ImageID;
	
	public ParticleSystem(final int Max, final float Delay, final int newImageID){
		try {
			mParticles = new SmartArray<Particle>(Max, Particle.class);
			SprayTimer = new FoofTimer(0.15f, true);
			ImageID = newImageID;
		} catch (IllegalAccessException e) {
			// TODO Handle Error - this is a bit of a problem
		} catch (InstantiationException e) {

		}
	}
	@Override
	public boolean UpdateAndDraw(SpriteBatch spriteBatch) {
		if(Lifespan > 0.0f){
			Lifespan -= Globals.secondsSinceLastFrame;
			if(SprayTimer.Update()){
				final Particle mNewParticle = mParticles.Add();
				if(mNewParticle != null){
					//TODO - Make this open ended, we're not just here for farting
					mNewParticle.Lifetime = FoofMath.RandFloat(0.3f, 0.7f);
					mNewParticle.ImageID = ImageID;
					mNewParticle.Scale = FoofMath.RandFloat(0.02f, 0.11f);
					mNewParticle.X = X;
					mNewParticle.Y = Y + 0.1f;
					mNewParticle.HalfLifespan = mNewParticle.Lifetime * 0.5f;
					mNewParticle.VelocityX = FoofMath.RandFloat(-0.5f, 0.5f);
					mNewParticle.VelocityY = FoofMath.RandFloat(0.3f, 0.8f);
				}
			}
		}
		mParticles.UpdateAndDraw(spriteBatch);
		return false;
	}

	@Override
	public <T> void Copy(T Src) {
		
	}
	public void Start(final float NewX, final float NewY, final float newLifespan){
			X = NewX;
			Y = NewY;
			Lifespan = newLifespan;
	}
	public void Stop(){
		Lifespan = 0.0f;
	}
}
