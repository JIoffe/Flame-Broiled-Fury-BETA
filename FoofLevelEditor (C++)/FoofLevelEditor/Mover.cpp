#include "Mover.h"
#include "Globals.h"

Mover::Mover(void){
	bIsSelected = false;
	uFunctionType = Mover::FunctionLin;
	uMoverType = Mover::HorizonalMover;
	fSpeed = 0.55f;
	Width = 0.25f;
	Sync = 0.0f;
	Pause = 0.0f;
}
Mover::~Mover(void){
}
void Mover::Duplicate(PropStruct ** ppPropStruct){
	*ppPropStruct = new Mover;
	memcpy(*ppPropStruct, this, sizeof(Mover));
}
void Mover::Update(){
		if(uMoverType == Slammer){
			float Value = sin( fSpeed * (Globals::uRunTimeSeconds + Sync));
			if(Pause > 0.0f){
				const float WP = Width + Pause;
				Value *= (WP);
				Value += Pause;
				if(Value > Width){
					Value = Width;
				}
			}else{
				Value *= Width;
			}
					Value += OriginY;
					y = Value;
					x = OriginX;
		}else{
			Value = ((2*asin(sin(2.0f * fSpeed * (Globals::uRunTimeSeconds + Sync) * 3.14f)))/3.14f);
			if(Pause > 0.0f){
				Value *= (Width + Pause);
				if(Value > Width){
					Value = Width;
				}else if(Value < -Width){
					Value = -Width;
				}
			}else{
				Value *= Width;
			}
			switch(uMoverType){
				case Mover::HorizonalMover:{
					Value += OriginX;
					x = Value;
					y = OriginY;
					break;
				}
				case Mover::VerticalMover:{
					Value += OriginY;
					y = Value;
					x = OriginX;
					break;
				}
				case Mover::Fader:{

					break;
				}
			}
		}
	return;
}
void Mover::SetSpeed(float fNewSpeed){
	fSpeed = fNewSpeed;
	return;
}
void Mover::SetWidth(float fNewMax){
	Width = fNewMax;
	return;
}
void Mover::SetPause(float fNewPause){
	Pause = fNewPause;
	return;
}
float Mover::GetPause(){
	return Pause;
}
float Mover::GetSpeed(){
	return fSpeed;
}
float Mover::GetWidth(){
	return Width;
}

void Mover::SetFunctionType(unsigned int uNewType){
	uFunctionType = uNewType;
	return;
}
void Mover::SetMoverType(unsigned int uNewType){
	uMoverType = uNewType;
	return;
}

unsigned char Mover::GetFunctionType(){
	return uFunctionType;
}
unsigned int Mover::GetMoverType(){
	return uMoverType;
}
