package com.Foofles;

public abstract class GameGUI {
	final static public int nGlobalAnims = 1;
	public static int DPadIndex = 0;
	final static public float DPadX = -1.26f;
	final static public float DPadY = -0.63f;
	final static public float DPadScale = 0.37f;
	
	static public int JumpButtonIndex;
	static public int FireButtonIndex;
	final static public float JumpButtonX = 0.83f;
	final static public float JumpButtonY = -0.72f;
	
	final static public float FireButtonX = 1.37f;
	final static public float FireButtonEdge = FireButtonX - 0.25f;
	final static public float FireButtonY = -0.61f;
	
	static public int ZeroStartIndex;
	
	static public int HealthDisplayIndex;
	static public int HealthTallyIndex;
	
	static public int PureWhiteIndex;
	static public int CheckpointIndex; //TODO - ANIMATE CHECKPOINT
	static public int ExitIndex; //TODO - Yeah...
	
	static public int HamburgerFlareIndex = 0;
	static public float FlareRotCos = 0.0f;
	static public float FlareRotSin = 0.0f;
	static public float FlareRot = 0.0f;
	
	static public int PlayerBulletImpactIndex = 0;
	
	final static public float GUIFadeSpeed = 1.6f;
	public static void UpdateGUIIndicies(final TextureAtlas mTextureAtlas){
		PureWhiteIndex = mTextureAtlas.GetIndexByTitle("purewhite.png");
		DPadIndex = mTextureAtlas.GetIndexByTitle("dir_arrows.png");
		JumpButtonIndex = mTextureAtlas.GetIndexByTitle("jump_button_new.png");
		FireButtonIndex = mTextureAtlas.GetIndexByTitle("fire_button_new.png");
		ZeroStartIndex = mTextureAtlas.GetIndexByTitle("zero.png");
		HealthDisplayIndex = mTextureAtlas.GetIndexByTitle("heatlh1.png");
		HealthTallyIndex = mTextureAtlas.GetIndexByTitle("health_tallybar.png");
		CheckpointIndex = mTextureAtlas.GetIndexByTitle("CHECKPOINT.png");
		ExitIndex = mTextureAtlas.GetIndexByTitle("exit.png");
		HamburgerFlareIndex = mTextureAtlas.GetIndexByTitle("burgerflare.png");
		return;
	}
}
