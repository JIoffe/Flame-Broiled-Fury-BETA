package com.Foofles;



/**
 * The big one. This is responsible for sorting game objects
 * and iterating through them for updates and preparation for the 
 * render queue. Everything will be batched in one shot
 * for a single draw call.
 * 
 * TODO - This is a bit bloated. Something should be chopped into a level scene class
 * @author Jason Ioffe
 *
 */
public class GameGraph {
	/*
	 * All actor - level collisions will be a direct [x][y] array lookup.
	 * 
	 * This might have a small memory overhead but it'll be worth it speed wise.
	 * Each index will just be a byte into an array of pairs of image and collision data.
	 */
	
	final static public float LevelTileScale = 0.15f;
	final static public float LevelTileOffset = LevelTileScale * 2;
	final static public float LevelTileHeightStep = LevelTileOffset / 64.0f;
	public int LevelGridWidth = 16;
	public int LevelGridHeight = 4;
	public LevelTile[][] LevelGrid = null;
	public float LevelGridMeasuredWidth = LevelGridWidth * LevelTileOffset;
	public float LevelGridMeasuredHeight = LevelGridHeight * LevelTileOffset;
	public float LevelGridWLookupMultiplier = LevelGridWidth / LevelGridMeasuredWidth;
	public int MaxW = LevelGridWidth - 1;
	public int MaxH = LevelGridHeight - 1;
	public float CamX = 0.0f;
	public float CamY = 0.0f;
	public float CamYDelta = 0.0f;
	public float DLeft = 0.0f;
	public float DRight = 0.0f;
	
	public LevelTile[] TileSet = null;
	public AnimationTrack[] AnimationTrackSet = null;
	public GameObjectInfo[] DynamicObjectSet = null;
	public GameObjectInfo[] StaticObjectSet = null;  // NOT collectibles or hazards
	public HazardousObjectInfo[] HazardousObjectSet = null;
	public GameObjectInfo[] ProjectileSet = null;
	
	public int nCameraZones = 0;
	public CamZone[] CameraZones = null;
	final public Player mPlayer = new Player();
	final public float[] Color = new float[16];

	final public GameBackground mBackground = new GameBackground();
	
	//Special arrays
	public SmartArray<PlayerBullet> mPlayerBullets;
	public SmartArray<Explosion> mExplosions;
	public SmartArray<EnemyProjectile> mEnemyProjectiles;
	public float ScreenFadeAmount = 0.0f;
	public byte ScreenFadeDirection = 0;
	
	final GameObjectGrid ObjectGrid = new GameObjectGrid();
	
	final public ScoreToastComponent ScoreToaster = new ScoreToastComponent();
	
	//Collision box for level exit
	public GameObject ExitObject = null;
	
	//TODO - This can be enclosed in a "level scene" type class
	public byte nSecrets = 0;
	public byte nBunnies = 0;
	//////////////////////////////////////////////////////////////////////////
	// Methods
	///
	public GameGraph(){
		//Init basic things
		Globals.mGameGraph = this;
		try {
			Initialize();
		} catch (IllegalAccessException e) {
			//TODO - Stack trace all the errors
		} catch (InstantiationException e) {

		}
	}
	private void Initialize() throws IllegalAccessException, InstantiationException{
		mPlayerBullets = new SmartArray<PlayerBullet>(10, PlayerBullet.class);
		mExplosions = new SmartArray<Explosion>(20, Explosion.class);
		mEnemyProjectiles = new SmartArray<EnemyProjectile>(20, EnemyProjectile.class);
		ScoreToaster.mToasts = new SmartArray<ScoreToast>(ScoreToastComponent.MaxScoreToasts, ScoreToast.class);
	}
	public void SetColor(final float R, final float G, final float B, final float A){
		for(int c = 0; c < 4; c++){
			final int ColorIndex = c*4;
			Color[ColorIndex] = R;
			Color[ColorIndex+1] = G;
			Color[ColorIndex+2] = B;
			Color[ColorIndex+3] = A;
		}
	}
	public void Process(){
		if(LevelGrid != null){
			//If there's no level grid, then there's nothing loaded
			/**
			//UPDATE PLAYER - the entire render queue depends on it!
			**/
			mPlayer.Update(this);
			
			
			//TODO - FIX CAMERA SYSTEM
			if(mPlayer.bFlip){
				CamX = FoofMath.Lerp(CamX, mPlayer.X - GameCamera.DefaultHorizontalOffset, 0.04f);
			}else{
				CamX = FoofMath.Lerp(CamX, mPlayer.X + GameCamera.DefaultHorizontalOffset, 0.04f);
			}

			CamZone CurrentCam = null;
			if(nCameraZones > 0){
				for(int i = 0; i < nCameraZones; i++){
					final CamZone Cam = CameraZones[i];
					if(mPlayer.X < Cam.Right && mPlayer.X > Cam.Left
							&& mPlayer.Y > Cam.Down && mPlayer.Y < Cam.Up){
						CurrentCam = Cam;
						break;
					}
				}
			}
			final float OldCamY = CamY;
			if(CurrentCam != null){
				CamY = FoofMath.Lerp(CamY, CurrentCam.Y, 0.02f);
			}else{
				final float PlayerOffsetCamY = mPlayer.Y - CamY;
				if(PlayerOffsetCamY > 0.6f){
					CamY = FoofMath.Lerp(CamY, CamY+(PlayerOffsetCamY - 0.6f), 0.03f);
				}else if(PlayerOffsetCamY < -0.25f){
					CamY = FoofMath.Lerp(CamY, CamY-(-0.25f - PlayerOffsetCamY), 0.03f);
				}
			}
			CamYDelta = CamY - OldCamY;
			/*
			 *   Prepare Level grid tiles for render
			 */
			
			//First determine extents for lookup, no sense going through it ALLLLLL
			//TODO - INCREASE SPEED
			//TODO - Spend Less time in "ADD SPRITE"
			DLeft = CamX - Globals.ratio;
			final int StartX = (DLeft <= 0) ? 0 : (int)((DLeft) / LevelTileOffset);
			DRight = CamX + Globals.ratio; 
			final int EndX = (DRight >= LevelGridMeasuredWidth) ? LevelGridWidth : (int)((DRight) / LevelTileOffset) + 1;
			
			final float DDown = CamY - 1.0f;
			final int StartY = (DDown <= 0) ? 0 : (int)((DDown) / LevelTileOffset);
			final float DUp = CamY + 1.0f;
			final int EndY = (DUp >= LevelGridMeasuredHeight) ? LevelGridHeight : (int)((DUp) / LevelTileOffset) + 1;
			
			final float XTrans = LevelTileScale - CamX;
			final float YTrans = LevelTileScale - CamY;
			
			//Compute how much the collectibles need to dance
			final float RotRad = (float) (Math.sin(Globals.runtimeSeconds * 4.2f) * 30.0f) * FoofMath.DegToRad;
			Collectible.BounceY = (float) (Math.sin(Globals.runtimeSeconds * 5.0f) * 0.04f);
			Collectible.CosRot = (float) Math.cos(RotRad);
			Collectible.SinRot = (float) Math.sin(RotRad);
			
			//And a universal rotation factor for flares
			GameGUI.FlareRot += (float) Globals.secondsSinceLastFrame * 50.4f; 
			final float FlareRad = GameGUI.FlareRot * FoofMath.DegToRad;
			GameGUI.FlareRotCos = (float) Math.cos(FlareRad);
			GameGUI.FlareRotSin = (float) Math.sin(FlareRad);
			/*
			 *  NOW we can lock the draw queue. It's best to keep this locked the least time possible
			 *  to promote asynchonosity between the threads.
			 */
			synchronized(DrawQueue.Lock){
				UpdateBackground(DrawQueue.spriteBatches[0]);
				final SpriteBatch LevelBatch = DrawQueue.spriteBatches[1];
				DrawQueue.spriteBatches[2].Reset();
				LevelBatch.Reset();
				
				for(int x = StartX; x < EndX; x++){
					final float XOffset = x * LevelTileOffset + XTrans;
					for(int y = StartY; y < EndY; y++){
						final byte ID = LevelGrid[x][y].SpriteID;
						if(ID > -1 && ID < 100){
							LevelBatch.AddSprite( XOffset, y * LevelTileOffset + YTrans, ID, LevelTileScale, false, null);
						}
					}
				}
				for(int x = StartX; x < EndX; x++){
					//The dreaded triple loop! TODO - Find a cleaner way
					for(int i = 0; i < 3; i++){
						final GameObjectGridSlice Slice = ObjectGrid.Slices[x];
						GameObject go = Slice.ObjectLists[i].RootObject;
						while(go != null){					
							if(go.bRender == 1){
								go.Render(CamX, CamY, DrawQueue.spriteBatches[go.Info.SpriteBatch]);
							}
							ObjectGrid.SortObject(go);
							go = go.Next;
						}
					}
				}
				for(int x = StartX; x < EndX; x++){
					final GameObjectGridSlice Slice = ObjectGrid.Slices[x];
					DynamicObject dyn = Slice.DynamicPVS.RootObject;
					while(dyn != null){
						final DynamicObject Current = dyn;
						dyn = dyn.PVSNext;
						if(Current.Update()){
							ObjectGrid.SortDynamicPVS(Current);
							ObjectGrid.SortObject(Current);
						}else{
							Current.Remove();
						}
					}
					GameObject go = Slice.ObjectLists[3].RootObject;
					while(go != null){
						if(go.bRender == 1){
							go.Render(CamX, CamY, DrawQueue.spriteBatches[go.Info.SpriteBatch]);
						}
						go = go.Next;
					}
				}
				LevelBatch.ResetBufferPositions();
				//Draw Player, Bullets, Special effects, and HUD
				DrawForeground(DrawQueue.spriteBatches[2]);
			}
		}
		return;
	}
	public boolean CheckAgainstWallsAndEdges(final float Delta, final CollisionData CD, float WallSenseHeight){
		final float FutureX = Delta > 0.0f ? (CD.Right + Delta) : (CD.Left + Delta);
		if(GetTileHeight(FutureX, WallSenseHeight) > WallSenseHeight){
			return true;
		}else{
			return (GetTileHeight(FutureX, CD.Bottom - GameGraph.LevelTileOffset) < 0.01f);
		}
	}
	/*
	 *  // TODO - Try to clean up all the checks VS ceiling and floors
	 *  // try to elliminate the bounds check... a little risky but a slight 
	 *  // speed increase. Try to get it out of gamegraph altogether and into another
	 *  // class
	 */
	//TODO - Get Functionality to interpret slope of floor/ceiling
	public float GetTileHeight(final float X, final float Y){
		float Height = 0.0f; // Init to 0.0f
		
		final int LookupX = (int) (X / LevelTileOffset);
		final int LookupY = (int) (Y / LevelTileOffset);
	
		if(WithinGridY(LookupY) && WithinGridX(LookupX)){
			final LevelTile CurrentTile = LevelGrid[LookupX][LookupY];
			CollisionManager.LastLevelTileHit = CurrentTile;
			if(CurrentTile.bIsCeiling == 0){
				final float TileStart = LookupX * LevelTileOffset;
				final int HeightmapLookup = (int) ((X - TileStart) / LevelTileHeightStep);
				if(HeightmapLookup > 0){
					final float LocalHeight = CurrentTile.HeightMask[HeightmapLookup] * LevelTileHeightStep;
					if(LocalHeight > 0.01f){
						Height = LookupY * LevelTileOffset + LocalHeight;
					}
				}
			}
		}
		return Height;
	}
	public float GetTileCeiling(final float X, final float Y){
		float Height = -1.0f;
		
		final int LookupX = (int) (X / LevelTileOffset);
		final int LookupY = (int) (Y / LevelTileOffset);
		if(WithinGridX(LookupX) && WithinGridY(LookupY)){
			final LevelTile levelTile = LevelGrid[LookupX][LookupY];
			if(levelTile != LevelTile.BlankTile && levelTile.bIsNotWall == 0){
				if(levelTile.bIsCeiling == 1){
					final float TileStart = LookupX * LevelTileOffset;
					final int HeightmapLookup = (int) ((X - TileStart) / LevelTileHeightStep);
					final float LocalHeight = levelTile.HeightMask[HeightmapLookup] * LevelTileHeightStep;
					if(LocalHeight > 0.01f){
						Height = LookupY * LevelTileOffset + LocalHeight;
					}
				}else{
					Height = (LookupY-1) * LevelTileOffset;
				}
			}
		}
		return Height;
	}
	public boolean CollisionAgainstWall(final CollisionData CD, float Delta, final float WallSenseHeight){
		//TODO - try to reduce code size without making it slower
		float Height = 0.0f; // Init to 0.0f
		final float X = Delta > 0.0f ? (CD.Right + Delta) : (CD.Left + Delta);
		final int LookupX = (int) (X / LevelTileOffset);
		final int LookupY = (int) (WallSenseHeight / LevelTileOffset);
	
		if(WithinGridY(LookupY) && WithinGridX(LookupX)){
			final LevelTile CurrentTile = LevelGrid[LookupX][LookupY];
			if(CurrentTile.bIsCeiling == 0 && CurrentTile.bIsNotWall == 0){
				final float TileStart = LookupX * LevelTileOffset;
				final int HeightmapLookup = (int) ((X - TileStart) / LevelTileHeightStep);
				if(HeightmapLookup > 0){
					final float LocalHeight = CurrentTile.HeightMask[HeightmapLookup] * LevelTileHeightStep;
					if(LocalHeight > 0.01f){
						Height = LookupY * LevelTileOffset + LocalHeight;
					}
				}
			}
		}	
		return (Height > WallSenseHeight);
	}
	public float FindCeilingIntersectionHeight(final float XLeft, final float XRight, final float Y){
		final float HeightLeft = GetTileCeiling(XLeft, Y);
		final float HeightRight = GetTileCeiling(XRight, Y);
		float ReturnHeight = -1.0f;
		if(HeightLeft < HeightRight && HeightLeft > 0.01f){
			ReturnHeight = HeightLeft;
		}else{
			if(HeightRight > 0.01f)
				ReturnHeight = HeightRight;
		}
		return ReturnHeight;
	}
	public float FindFloorIntersectionHeight(final float XLeft, final float XRight, final float Y){
		final float HeightLeft = GetTileHeight(XLeft, Y);
		final float HeightRight = GetTileHeight(XRight, Y);
		if(HeightLeft > HeightRight){
			return HeightLeft;
		}else{
			return HeightRight;
		}
	}
	private void UpdateBackground(final SpriteBatch mSpriteBatch){
		//Add Parallax Layers
		mSpriteBatch.colorBuffer.position(16);
		mSpriteBatch.texBuffer.position(8);
		mSpriteBatch.vertexBuffer.position(12);
		mSpriteBatch.nIndices = 6;
		for(int i = 0; i < mBackground.nLayers; i++){
			mBackground.BGLayers[i].UpdateAndRender(CamYDelta, CamX, mSpriteBatch);
		}
		mSpriteBatch.colorBuffer.position(0);
		mSpriteBatch.texBuffer.position(0);
		mSpriteBatch.vertexBuffer.position(0);
		return;
	}
	private void DrawForeground(final SpriteBatch mSpriteBatch){

		//Draw Player!!!!
		final float PlayerScale = (mPlayer.bIsSuperBunny) ? Player.SuperScale : Player.Scale;
		if(mPlayer.DamageDelay > 0.0f){
			SetColor(1.0f, 1.0f, 1.0f, (float)Math.sin(mPlayer.DamageDelay*50.0));
			mSpriteBatch.AddSprite(mPlayer.X - CamX, mPlayer.Y - CamY, mPlayer.CurrentSprite, PlayerScale, mPlayer.bFlip, Color);
		}else{
			mSpriteBatch.AddSprite(mPlayer.X - CamX, mPlayer.Y - CamY, mPlayer.CurrentSprite, PlayerScale, mPlayer.bFlip, null);
		}
		//Draw Bullets, explosions, score toasts etc.
		mPlayerBullets.UpdateAndDraw(mSpriteBatch);
		mExplosions.UpdateAndDraw(mSpriteBatch);
		ScoreToaster.Update(mSpriteBatch);
		/**
		 *  GUI - Buttons, health, score etc.
		 */
		mSpriteBatch.AddSprite(GameGUI.DPadX, GameGUI.DPadY, GameGUI.DPadIndex, GameGUI.DPadScale, false, null);
		mSpriteBatch.AddSprite(GameGUI.JumpButtonX , GameGUI.JumpButtonY, GameGUI.JumpButtonIndex, 0.25f, false, null);
		mSpriteBatch.AddSprite(GameGUI.FireButtonX , GameGUI.FireButtonY, GameGUI.FireButtonIndex, 0.25f, false, null);
		//Update and draw score - I hate having long, ugly functions
		//                        but this will likely run faster than calling another
		Score.Update();
		if(Score.ScoreAlpha > 0.003f){
			float ScoreDigitX = -1.5f + Score.nScoreDigits * Score.ScoreNumberOffset;
			SetColor(1.0f, 1.0f, 1.0f, Score.ScoreAlpha);
			float ScoreY = 0.86f;
			if(Score.ScoreAlpha < 1.0f){
				ScoreY += (1.0f - Score.ScoreAlpha) * 0.1f;
			}
			for(int i = 0; i < Score.nScoreDigits; i++){
				mSpriteBatch.AddSprite(ScoreDigitX , ScoreY, Score.ScoreNumberSprites[i], Score.ScoreNumberScale, false, Color);
				ScoreDigitX -= Score.ScoreNumberOffset;
			}
		}
		
		//Health
		if(mPlayer.HealthDisplay > 0.0f){
			float HealthY = 0.82f;
			if(mPlayer.HealthDisplay < 1.0f){
				HealthY += (1.0f - mPlayer.HealthDisplay) * 0.1f;
			}
			SetColor(1.0f, 1.0f, 1.0f, mPlayer.HealthDisplay);
			mSpriteBatch.AddSprite(1.09f, HealthY, GameGUI.HealthDisplayIndex, 0.24f, false, Color);
			float TallyX = 0.94f;
			for(byte i = 0; i < Player.HitPoints; i++){
				mSpriteBatch.AddSprite(TallyX, HealthY, GameGUI.HealthTallyIndex, 0.06f, false, Color);
				TallyX += 0.076f;
			}
		}
		
		//Full Screen fade?
		if(ScreenFadeDirection != 0){
			SetColor(0.0f,0.0f,0.0f,ScreenFadeAmount);
			mSpriteBatch.AddFullScreenSprite(GameGUI.PureWhiteIndex, Color);
			//Set to 0 before draw
			if(ScreenFadeDirection == 1)
				ScreenFadeAmount -= Globals.secondsSinceLastFrame * 0.4f;
			else
				ScreenFadeAmount += Globals.secondsSinceLastFrame * 0.4f;
		}
		mSpriteBatch.vertexBuffer.position(0);
		mSpriteBatch.texBuffer.position(0);
		mSpriteBatch.colorBuffer.position(0);
	}
	public void AddExplosion(final float X, final float Y, final AnimationTrack ExplosionType, final boolean bFlip){
		final Explosion mExplosion = mExplosions.Add();
		if(mExplosion != null){
			mExplosion.X = X;
			mExplosion.Y = Y;
			mExplosion.mAnimation.bIsPlaying = true;
			mExplosion.mAnimation.fCurrentFrame = 0.0f;
			mExplosion.mAnimation.Animations[0] = ExplosionType;
			mExplosion.bFlip = bFlip;
			if(ExplosionType == Explosion.SmokeExplosion){
				mExplosion.Scale = 0.25f;
				SoundSystem.PlaySound(Explosion.SmokeExplosion_SoundID);
			}else if(ExplosionType == Explosion.CollectionPuftA){
				mExplosion.Scale = 0.11f;
			}else if(ExplosionType == Explosion.FieryExplosion){
				mExplosion.Scale = 0.5f;
			}else if(ExplosionType == Explosion.PlayerShotImpact){
				mExplosion.Scale = 0.2f;
			}else if(ExplosionType == Explosion.PlayerMuzzle){
				mExplosion.Scale = 0.11f;
			}
		}
	}
	public void AddEnemyProjectile(final float X, final float Y, final float VelocityX, final float VelocityY, final GameObjectInfo Info, final boolean bFlip){
		//TODO - These should probably have some kind of failure case
		final EnemyProjectile Bullet = mEnemyProjectiles.Add();
		if(Bullet != null){
			Bullet.nSAIndex = mEnemyProjectiles.nIndexLast;
			Bullet.X = X;
			Bullet.Y = Y;
			Bullet.VelocityX = VelocityX;
			Bullet.VelocityY = VelocityY;
			Bullet.Info = Info;
			Bullet.bFlip = bFlip;
			Bullet.InitCD();
			ObjectGrid.SortObject(Bullet);
			Bullet.VisibilityMax = DRight;
			Bullet.VisibilityMin = DLeft;
			Bullet.mAnimation.Animations = Info.Animations;
			Bullet.mAnimation.bIsPlaying = true;
			Bullet.mAnimation.Behaviour = AnimationType.Behaviour_Loop;
			Bullet.mAnimation.CurrentTrack = 0;
			ObjectGrid.SortDynamicPVS(Bullet);
		}
		return;
	}
	boolean WithinGridY(final int Y){
		return (Y > -1 && Y < LevelGridHeight);
	}
	boolean WithinGridX(final int X){
		return (X > -1 && X <LevelGridWidth);
	}
}
