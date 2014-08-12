#pragma once

//Static methods and constants for math

float ClampRotation(float fRot);
float Lerp(float a, float b, float s);
// 180 / Pi
int Clamp(int min, int max, int i);
long Clamp(long min, long max, long i);