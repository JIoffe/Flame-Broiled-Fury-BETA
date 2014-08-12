package com.Foofles;

public class EnemyInfo extends GameObjectInfo {
	AnimationTrack mAttackAnimation = null;
	AnimationTrack mDeathAnimation = null;
	float Speed;
	byte ShotType;
	byte ShotDirection;
	byte nShotsPerBurst;
	float ShotDelay;
	byte BehaviourType;
	
	int Agro; //Aggressiveness - 0 = Passive, 1 = Full Agro, 2 = Cautious Agro, 3 = flee?
	byte bDormant;
	float JumpStrength;
}
