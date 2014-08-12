package com.Foofles;

// A simple timer that will count up and return "True" when its target is hit
public class FoofTimer {
	public boolean bOn;
	public float Interval;
	public float CurrentTime;
	public FoofTimer(final float NewInterval, final boolean bStart){
		Interval = NewInterval;
		CurrentTime = NewInterval;
		bOn = bStart;
	}
	public boolean Update(){
		if(bOn){
			if(CurrentTime < Interval){
				CurrentTime += Globals.secondsSinceLastFrame;
				return false;
			}else{
				Reset();
				return true;
			}
		}
		return false;
	}
	public void Reset(){
		CurrentTime = 0.0f;
		bOn = true;
		return;
	}
}
