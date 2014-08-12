package com.Foofles;

public class GameObjectInfo {
	
	public String Name = new String();
	public float fDefaultScale;
	public int Type;
	public int SubType;
	byte HP; //-1 means invulnerable to damage
	public boolean bAnimated;
	public AnimationTrack[] Animations = null;
	public int nAnimationTracks = 0;
	public int nSpriteID; //Use for collisions and to fall back on if anim == null
	public byte bIsKillable = 0;
	public byte bIsPlatform = 0;
	public byte bIsSpecial = 0;
	public byte SpriteBatch = 2;
	final public CollisionData CD =  new CollisionData();
	public GameObjectInfo(){
		bIsSpecial = 0;
		Name = "Object";
		fDefaultScale = 0.15f;
		Type = -1;
		bAnimated = false;
		Animations = null;
		nSpriteID = 0;
		SpriteBatch = 2;
	}
	public GameObjectInfo(final String newName, final float Scale, final int newType, final boolean Animated, final int SpriteID){
		Name = newName;
		fDefaultScale = Scale;
		Type = newType;
		bAnimated = Animated;
		nSpriteID = SpriteID;
	}
	final void NewAnimations(final int nAnims){
		Animations = new AnimationTrack[nAnims];
		nAnimationTracks = nAnims;
		for(int i = 0; i < nAnims; i++){
			Animations[i] = null; //Just default to null for safety
		}
		return;
	}
	final void SetAnimationTrack(final int nAnimIndex, final int nTrackIndex){
		if(nTrackIndex < 0 || nAnimIndex < 0){
			Animations[nAnimIndex] = null;
			return;
		}else{
			Animations[nAnimIndex] = Globals.mGameGraph.AnimationTrackSet[nTrackIndex];
			return;
		}
	}
}
