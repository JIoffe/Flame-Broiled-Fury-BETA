package com.Foofles;

public class FoofGame extends BaseGame{
	
	final public static GameObjectInfo[] CollectiblesInfo = new GameObjectInfo[10];
	final public static GameObjectInfo CheckPointInfo = new GameObjectInfo();
	final public static GameObjectInfo TokenInfo = new GameObjectInfo();
	final public static GameObjectInfo BunnyHostageInfo = new GameObjectInfo();
	final public static GameObjectInfo BunnyCageInfo = new GameObjectInfo();
	
	final public static GameObjectInfo ExitInfo = new GameObjectInfo();
	final public static int[] LevelList = {  R.raw.caves1, R.raw.forest2, R.raw.forest3, R.raw.forest5};
	final public static int[] TexAtlasUVList = {  R.raw.forestlevel, R.raw.caveslevel};
	final public static int[] BGAtlasUVList = {  R.raw.forestbg, R.raw.cavebg };
	final public static int[] TilesetList = {  R.raw.forest, R.raw.caves};
	
	final public static int[] ThemeIDList = { 1, 1, 0, 1, 2 };
	final public static byte[] BGIndexList = { 5, 0 };
	final public static int nTotalLevels = 1;
	
	final public static int FOREST_THEME = 0;
	final public static int CAVE_THEME = 1;
	final public static int MECH_THEME = 2;

	
	final private GameThread mGamePlayThread = new GameThread();
	final private Thread tGameThread = new Thread(mGamePlayThread, "Game");
	
	private int PreviousTheme = -1;
	private int CurrentLevel = -1;

	static public float CheckPointX = -1.0f;
	static public float CheckPointY = -1.0f;
	
	public int GetCurrentLevel(){
		return CurrentLevel;
	}
	public static void TriggerCheckPoint(final float X, final float Y){
		CheckPointX = X;
		CheckPointY = Y;
	}
	public static void ClearCheckPoint(){
		//Negative values means no checkpoint
		CheckPointX = -1.0f;
		CheckPointY = -1.0f;
		return;
	}
	public boolean LoadGameBasics(){
        synchronized(DrawQueue.Lock){
        	final TextureAtlas mTextureAtlas = DrawQueue.spriteBatches[2].mTextureAtlas;	
			mTextureAtlas.ParseUVFromFile(FileIOManager.GetInputStreamToResource(R.raw.gameglobal), false);
			//Setup Global object gameinfos
			CheckPointInfo.fDefaultScale = 0.20f;
			CheckPointInfo.bAnimated = false;
			CheckPointInfo.nSpriteID = mTextureAtlas.GetIndexByTitle("CHECKPOINT.png");
			CheckPointInfo.Type = GameObjectTypes.CheckPoint;
			CheckPointInfo.Name = "CheckPoint";
			
			ExitInfo.fDefaultScale = 0.3f;
			ExitInfo.CD.Scale(0.3f);
			ExitInfo.bAnimated = false;
			ExitInfo.nSpriteID = mTextureAtlas.GetIndexByTitle("exit.png");
			ExitInfo.Type = GameObjectTypes.LevelExit;
			ExitInfo.Name = "Level Exit";
			
			TokenInfo.fDefaultScale = 0.12f;
			TokenInfo.bAnimated = false;
			TokenInfo.nSpriteID = mTextureAtlas.GetIndexByTitle("bunnymoney.png");
			TokenInfo.Type = GameObjectTypes.Token;
			TokenInfo.Name = "Bunny Money";			
			TokenInfo.bIsSpecial = 1;
			
			BunnyHostageInfo.fDefaultScale = 0.15f;
			BunnyHostageInfo.bAnimated = false;
			BunnyHostageInfo.nSpriteID = mTextureAtlas.GetIndexByTitle("babybunny1.png");
			BunnyHostageInfo.Type = GameObjectTypes.BunnyHostage;
			BunnyHostageInfo.Name = "Bunny";			
			
			BunnyCageInfo.fDefaultScale = 0.18f;
			BunnyCageInfo.bAnimated = false;
			BunnyCageInfo.nSpriteID = mTextureAtlas.GetIndexByTitle("cageball.png");
			BunnyCageInfo.Type = GameObjectTypes.BunnyHostage;
			BunnyCageInfo.Name = "Bunny Cage";	
			BunnyCageInfo.bIsKillable = 1;
			
			//TODO COLLECTIBLES
			CollectiblesInfo[0] = new GameObjectInfo("Chocolate Pop", 0.082f, GameObjectTypes.Collectible, false, mTextureAtlas.GetIndexByTitle("choco_pop.png"));
			CollectiblesInfo[1] = new GameObjectInfo("Frosted Donut", 0.095f, GameObjectTypes.Collectible, false, mTextureAtlas.GetIndexByTitle("frosted_donut.png"));
			CollectiblesInfo[2] = new GameObjectInfo("Cake", 0.095f, GameObjectTypes.Collectible, false, mTextureAtlas.GetIndexByTitle("cake.png"));
			CollectiblesInfo[3] = new GameObjectInfo("Cherries", 0.08f, GameObjectTypes.Collectible, false, mTextureAtlas.GetIndexByTitle("cherries.png"));
			CollectiblesInfo[4] = new GameObjectInfo("Brownie", 0.08f, GameObjectTypes.Collectible, false, mTextureAtlas.GetIndexByTitle("brownie.png"));
			CollectiblesInfo[5] = new GameObjectInfo("Cookie", 0.08f, GameObjectTypes.Collectible, false, mTextureAtlas.GetIndexByTitle("cookie.png"));
			CollectiblesInfo[6] = new GameObjectInfo("Jawbreker", 0.08f, GameObjectTypes.Collectible, false, mTextureAtlas.GetIndexByTitle("jawbreaker.png"));
			CollectiblesInfo[7] = new GameObjectInfo("Cosmic Cheeseburger", 0.1f, GameObjectTypes.Collectible, false, mTextureAtlas.GetIndexByTitle("doubleburger.png"));
			CollectiblesInfo[8] = new GameObjectInfo("Single Energy", 0.082f, GameObjectTypes.Collectible, false, mTextureAtlas.GetIndexByTitle("energy.png"));
			CollectiblesInfo[7].bIsSpecial = 1;
			CollectiblesInfo[8].bIsSpecial = 1;

			PlayerBullet.SpriteIndex = mTextureAtlas.GetIndexByTitle("bulletsmall.png");

			GameGUI.UpdateGUIIndicies(mTextureAtlas);

	        mGamePlayThread.mGameGraph.mPlayer.PrepareAnimations(mTextureAtlas);
	        Explosion.PrepareAnimationTracks(mTextureAtlas);
	        
	        ScoreToast.InitializeScoreToasts();
        }
		return true;
	}
	@Override
	public boolean RestartLevel(){
		return LoadLevel(CurrentLevel);
	}
	public boolean LoadLevel(final int LevelID){
		if(CurrentLevel != LevelID){
			ClearCheckPoint();
			CurrentLevel = LevelID;
	        //Update the full screen BG quad in the sprite batch
			//TODO - find a less retarded way of assigning level theme
			Globals.CurrentTheme = ThemeIDList[LevelID];
			final int ThemeID = Globals.CurrentTheme;
			if(ThemeID != PreviousTheme){
		        synchronized(DrawQueue.Lock){
					DrawQueue.spriteBatches[1].mTextureAtlas.ParseUVFromFile(FileIOManager.GetInputStreamToResource(TexAtlasUVList[ThemeID]), false);
					DrawQueue.spriteBatches[0].mTextureAtlas.ParseUVFromFile(FileIOManager.GetInputStreamToResource(BGAtlasUVList[ThemeID]), false);
					DrawQueue.spriteBatches[0].ChangeBGImage(BGIndexList[ThemeID]);
					
					EnemyProjectileInfo.Initialize(DrawQueue.spriteBatches[1].mTextureAtlas);
		        }
		        GameDataIO.LoadLevelTileset(FileIOManager.GetInputStreamToResource(TilesetList[ThemeID]), mGamePlayThread.mGameGraph);

				mGamePlayThread.mGameGraph.mBackground.UpdateLayersForTheme(ThemeID);
			}
		}
	    GameDataIO.LoadLevelResource(FileIOManager.GetInputStreamToResource(LevelList[LevelID]), mGamePlayThread.mGameGraph);
		mGamePlayThread.mGameGraph.mPlayer.Initialize();
			

		Score.Initialize();
		mGamePlayThread.mGameGraph.ScreenFadeAmount = 1.0f;
		mGamePlayThread.mGameGraph.ScreenFadeDirection = 1;
	    System.gc();
	        
	    mGamePlayThread.RefreshTime();
		return true;
	}
	public void Start(){
		Globals.mGame = this;
        tGameThread.start();
		return;
	}
	public void Resume(){
		mGamePlayThread.ResumeGame();
    	SoundSystem.ResumeMusic();
		return;
	}
	public void Pause(){
		mGamePlayThread.PauseGame();
    	SoundSystem.PauseMusic();
		return;
	}
	public void End(){
		//Game over man, game over
		mGamePlayThread.KillGame();
    	SoundSystem.StopMusic();
		return; 
	}
}
