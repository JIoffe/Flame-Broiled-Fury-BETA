#include "Player.h"
#include "CamZone.h"
#include "FoofMath.h"

float Player::Speed = 1.0f;
float Player::Scale = 0.2f;
float Player::JumpVelocity = 4.0f;
int Player::BASICRUN_SPRITEINDEX = 0;
int Player::ANIMATION_RUNBASIC = 0;
bool Player::MOVE_LEFT = false;
bool Player::MOVE_RIGHT = false;
unsigned char Player::JumpCount = 0;
float Player::YVelocity = 0.0f;


Player::Player(void)
{
}


Player::~Player(void)
{
}

void Player::Initialize(){
	Player::JumpCount = 0;
	CurrentSprite = Player::BASICRUN_SPRITEINDEX;
	X = 0.0;
	Y = 0.0;
	CD.Top = 1.0f;
	CD.Bottom = -1.0f;
	CD.Left = -0.15f;
	CD.Right = 0.15f;
	CD.Scale(Player::Scale);

	return;
}
void Player::PrepareAnimations(){
		mAnimation.Behaviour = AnimationType::Behaviour_Loop;
		mAnimation.pTrack = &BasicRunTrack;
        //Setup Player Animations
		BasicRunTrack.iStartingFrame = Player::BASICRUN_SPRITEINDEX;
		BasicRunTrack.fDefaultSpeed = 14.0f;
		BasicRunTrack.nFrames = 9;
		return;
}
void Player::Update(){
	if(Globals::bPreviewPlay){
		float XDelta = 0.0f;
		float YDelta = 0.0f;
		if(Player::MOVE_LEFT){
			XDelta = -Player::Speed * Globals::SecondsSinceLastFrame;
			bFlip = true;
		}else if(Player::MOVE_RIGHT){
			XDelta = Player::Speed * Globals::SecondsSinceLastFrame;
			bFlip = false;
		}

		Player::YVelocity += Globals::fGravity;
		YDelta = Player::YVelocity * Globals::SecondsSinceLastFrame;
		const float FutureFeet = CD.Bottom + YDelta;
		const float TileHeight = FindFloorIntersectionHeight(CD.Left, CD.Right, FutureFeet);
		const float HalfHeight = Y - CD.Bottom;
		const float WallSenseHeight = Y - 0.1f;
		if(XDelta != 0.0f){
			CurrentSprite = mAnimation.AdvanceFrame();
			if(!CollisionAgainstLevelHorizontally(XDelta, &CD, WallSenseHeight)){
				X += XDelta;
			}else{
				XDelta = 0.0f;
			}
		}

		if(FutureFeet > TileHeight){
			//Check against ceilings
			float FutureHead = CD.Top + YDelta;
			float TileBottom = FindCeilingIntersectionHeight(CD.Left, CD.Right, FutureHead);
			if(TileBottom > 0.0f && FutureHead > TileBottom){
				//Hit the ceiling, compute new position.
				const float TopHalf = CD.Top - Y;
				const float NewPos = TileBottom - HalfHeight - 0.001f; //Add a little to put us a tile higher
				YDelta = NewPos - Y;
				Y = NewPos;
				if(Player::YVelocity > 0.0f){
					Player::YVelocity = 0.0f;
				}
			}else{
				Y += YDelta;
			}
		}else{
			const float NewPos = TileHeight + HalfHeight + 0.001f; //Add a little to put us a tile higher
			YDelta = NewPos - Y;
			Y = NewPos;
			Player::YVelocity = 0.0f;
			Player::JumpCount = 0;
		}
		CD.Translate(XDelta, YDelta);

		if(bFlip){
			Globals::CamX = Lerp(Globals::CamX, X - 0.25f, 0.04f);
		}else{
			Globals::CamX = Lerp(Globals::CamX, X + 0.25f, 0.04f);
		}

		CamZone * CurrentCam = GetCurrentCamZone(X,Y);

		if(CurrentCam != NULL){
			Globals::CamY = Lerp(Globals::CamY, CurrentCam->y, 0.02f);
		}else{
			const float PlayerOffsetCamY = Y - Globals::CamY;
			if(PlayerOffsetCamY > 0.6f){
				Globals::CamY = Lerp(Globals::CamY, Globals::CamY+(PlayerOffsetCamY - 0.6f), 0.03f);
			}else if(PlayerOffsetCamY < -0.25f){
				Globals::CamY = Lerp(Globals::CamY, Globals::CamY-(-0.25f - PlayerOffsetCamY), 0.03f);
			}
		}
	}
	return;
}