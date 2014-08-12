#include "FoofMath.h"

//Static methods and constants for math

float ClampRotation(float fRot){
	//TODO - Make this better.
	if(fRot < 0.0f){
		return 360.0f - fRot;
	}else if(fRot > 360.0f){
		return fRot - 360.0f;
	}else
		return fRot;
}
float Lerp(float a, float b, float s){
	return b - ((1.0f - s) * (b - a));
}
// 180 / Pi
int Clamp(int min, int max, int i){
	if(i > max){
		return max;
	}else if(i < min){
		return min;
	}else{
		return i;
	}
}
long Clamp(long min, long max, long i){
	if(i > max){
		return max;
	}else if(i < min){
		return min;
	}else{
		return i;
	}
}