#pragma once
#include "PropStruct.h"
class Mover :
	public PropStruct{
	unsigned int uFunctionType;
	unsigned int uMoverType;
	float fSpeed;
	float Width;
	float Pause;
public:
	float OriginX;
	float OriginY;
	float Value;
	float Sync;  //Offset of Time
	static const unsigned int FunctionSin = 1;
	static const unsigned int FunctionLin = 2;
	static const unsigned int HorizonalMover = 0;
	static const unsigned int VerticalMover = 1;
	static const unsigned int Fader = 2;
	static const unsigned int Slammer = 3;

	Mover(void);
	~Mover(void);

	void SetSpeed(float fNewSpeed);
	void SetWidth(float fNewMax);
	void SetPause(float fNewPause);

	float GetPause();
	float GetSpeed();
	float GetWidth();
	
	void SetFunctionType(unsigned int uNewType);
	void SetMoverType(unsigned int uNewType);

	unsigned char GetFunctionType();
	unsigned int GetMoverType();

	void Update();
	void Duplicate(PropStruct ** ppPropStruct);
};

