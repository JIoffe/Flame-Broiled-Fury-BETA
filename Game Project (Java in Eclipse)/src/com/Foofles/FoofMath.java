package com.Foofles;

import java.util.Random;

//Static methods and constants for math

public class FoofMath {
	final static public Random mRandom = new Random();
	
	static public float ClampRotation(float fRot){
		//TODO - Make this better.
		if(fRot < 0.0f){
			return 360.0f - fRot;
		}else if(fRot > 360.0f){
			return fRot - 360.0f;
		}else
			return fRot;
	}
	static public <T extends Number> boolean DifferingSign(Number A, Number B){
		return (A.floatValue() * B.floatValue() > 0);
	}
	static public float SquardDistance(float Xa, float Ya, float Xb, float Yb){
		final float A = Xb - Xa;
		final float B = Yb - Ya;
		return (A*A) + (B * B);
	}
	static public float Lerp(float a, float b, float s){
		return b - ((1.0f - s) * (b - a));
	}
	// 180 / Pi
	final static public float RadToDeg = 57.295779513082320876798154814105f;
	//Pi / 180
	final static public float DegToRad= 0.01745329251994329576923690768489f;
	
	final static public int[] ExtractTable = { 1, 10, 100, 1000, 10000, 100000};
	final static public double LogBase2 = Math.log(2.0);
	static public int Clamp(int min, int max, int i){
		if(i > max){
			return max;
		}else if(i < min){
			return min;
		}else{
			return i;
		}
	}
	static float SineWave(final float X, final float fSpeed, final float Width, final float Pause, final float Translation){
		float Value = (float)Math.sin(fSpeed * X );
		if(Pause > 0.0f){
			Value *= (Width + Pause);
			Value += Translation;    //eg. translate to avoid clipping
			if(Value > Width){
				Value = Width;
			}else if(Value < -Width){
				Value = -Width;
			}			
		}
		return Value;
	}
	static float TriangleWave(final float X, final float fSpeed, final float Width, final float Pause, final int PauseDirection ){
		//0 = Both, 1 = Pause at top, 2 = Pause at Bottom
		//TODO - Convert to Max(min(sin))
		float Value = (float) ((2*Math.asin(Math.sin(2.0f * fSpeed * X * Math.PI)))/Math.PI);
		//Todo - Make pause less retarded
		if(Pause > 0.0f && (PauseDirection == 0 || Value > 0.0f)){
			Value *= (Width + Pause);
			if(Value > Width){
				Value = Width;
			}else if(Value < -Width){
				Value = -Width;
			}
		}else{
			Value *= Width;
		}

		return Value;
	}
	static public long Clamp(long min, long max, long i){
		if(i > max){
			return max;
		}else if(i < min){
			return min;
		}else{
			return i;
		}
	}
	static public float RandFloat(final float Min, final float Max){
		//Does what its name implies - returns a random float between min and max
		return mRandom.nextFloat() * (Max - Min) + Min;
	}
}
