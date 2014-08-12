package com.Foofles;

import android.os.SystemClock;


public class GameThread implements Runnable{
	private boolean bRunGame;
	private boolean bIsPaused;
	final private Object mGameLock = new Object();
	protected long iDeltaTime; // in ms
	protected long FrameStart;
	protected long TimeLastFrame;
	protected long iRunTime;
	private long ProfileTime;
	private long ProfileFrames;
	final public GameGraph mGameGraph = new GameGraph();
	
	private final long Profile_Heartbeat_Rate = 3000;
	public GameThread(){
		bRunGame = true;
		bIsPaused = false;

		iRunTime = 0; //Long has a limit but it'll just loop 
		iDeltaTime = 0;
		TimeLastFrame = 0;
		ProfileTime = 0;
		ProfileFrames = 0;
		
		TimeLastFrame = 0;
	}
	@Override
	public void run() {
		//TODO Fix timestamp.
		TimeLastFrame = SystemClock.uptimeMillis();
		while(bRunGame){
			FrameStart = SystemClock.uptimeMillis();
			//Globals.TimeInMS += FrameStart;
			iDeltaTime = FrameStart - TimeLastFrame;
			iDeltaTime = FoofMath.Clamp(0, 100, iDeltaTime);
			Globals.msSinceLastFrame = iDeltaTime;
			Globals.secondsSinceLastFrame = (float)iDeltaTime / 1000.0f;
			Globals.runtimeSeconds += Globals.secondsSinceLastFrame;
			Globals.fGravity = -9.4f * Globals.secondsSinceLastFrame;
			
			mGameGraph.Process();

			TimeLastFrame = SystemClock.uptimeMillis();

			if(iDeltaTime < 16){
				try{
					Thread.sleep(16 - iDeltaTime);
				} catch (InterruptedException e){
					//No problemo, we're not doing anything anyway
				}
			}
			
			synchronized(mGameLock){
				if(bIsPaused){
					while(bIsPaused){
						//ie. do nothing
						try {
							mGameLock.wait();
						} catch (InterruptedException e) {
							//Interruption isn't a big deal,we're not doing anything anyway
						}
						
					}
				}
			}
		}
		return;
	}
	public void KillGame(){
		synchronized(mGameLock){
			bRunGame = false;
			bIsPaused = false;
			mGameLock.notifyAll();
		}
	}
	public void PauseGame(){
		synchronized(mGameLock){
			bIsPaused = true;
		}
	}
	public void ResumeGame(){
		synchronized(mGameLock){
			bIsPaused = false;
			TimeLastFrame = SystemClock.uptimeMillis();
			mGameLock.notifyAll();
		}
	}
	public void RefreshTime(){
		//A bit of a hack to avoid the weird jumping that sometimes happens
		TimeLastFrame = SystemClock.uptimeMillis();	
	}
}
