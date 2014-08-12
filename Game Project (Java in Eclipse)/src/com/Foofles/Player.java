package com.Foofles;

public class Player extends GameObject{
	//Animation list - 
	//  0 = Default Run
	//  1 = Giant Run
	//  2 = Giant Morph
	//  3 = Giant Slam
	public static final AnimationTrack ANIMATION_RUNBASIC = new AnimationTrack();
	public static final float Speed = 1.0f;
	public static final float Scale = 0.2f;
	public static final float SuperScale = 0.4f;
	final static public float JumpVelocity = 4.0f;
	public static float YVelocity = 0.0f;
	public static byte JumpCount = 0;

	public int CurrentSprite = 0;
	public static byte HitPoints = 5;
	final public AnimationComponent mAnimation = new AnimationComponent();
	
	final public static float DamageGracePeriod = 1.2f; //2 seconds between damage 
	public float DamageDelay;
	public float HealthDisplay;
	
	public float DeathCounter;
	
	public boolean bIsSuperBunny = false;
	public static final float TotalSuperTime = 10.0f;
	public float SuperTimer = 0.0f;
	public byte MorphDirection = 0;
	
	private boolean bHitGround = true;
	private float FloorHeight = 0.0f;
	private float PlayerFeet = 0.0f;
	public int PlayerCellX = 0;
	
	//0 = Running
	//1 = Jumping
	//2 = Giant Slam Attack
	private byte CurrentAnimationType = 0;
	
	
	private float LevelTime = 0;
	private float CheckpointTime = -1;
	
	public DynamicObject mMountedPlatform = null;
	
	public void Initialize(){
		if(CheckpointTime > 0.0f){
			LevelTime = CheckpointTime;
		}else{
			LevelTime = 0.0f;
		}
		mAnimation.bIsPlaying = true;
		DeathCounter = 3.0f;
		DamageDelay = 2.0f;
		HealthDisplay = 3.5f;
		mAnimation.SetTrack(0);
		mAnimation.Behaviour = AnimationType.Behaviour_Loop;
		CurrentSprite = mAnimation.AdvanceFrame();
		HitPoints = 5;
		bIsSuperBunny = false;
		InitCD(Player.Scale);
		MorphDirection = 0;
	}
	public void TransformToSuper(){
		bIsSuperBunny = true;
		mAnimation.SetTrack(2);
		mAnimation.Behaviour = AnimationType.Behaviour_OneShot;
		MorphDirection = 1;
		InitCD(Player.SuperScale);
		SuperTimer = TotalSuperTime;
		return;
	}
	public void ReturnToNormal(){
		MorphDirection = -1;
		mAnimation.SetTrack(2);
		mAnimation.Behaviour = AnimationType.Behaviour_PingPong;
		mAnimation.fCurrentFrame = mAnimation.Animations[2].nFrames - 1;
		return;
	}
	public void InitCD(final float fScale){
		CD.Top = 1.0f;
		CD.Bottom = -0.925f;
		final float Width = (fScale == Player.Scale) ? 0.15f : 0.3f;
		CD.Left = -Width;
		CD.Right = Width;
		CD.Scale(fScale);
		CD.Translate(X, Y);
	}
	public void PrepareAnimations(final TextureAtlas mTextureAtlas){
		mAnimation.Behaviour = AnimationType.Behaviour_Loop;
		mAnimation.Animations = new AnimationTrack[4];
		mAnimation.Animations[0] = Player.ANIMATION_RUNBASIC;
		AnimationTrack Anim = mAnimation.Animations[0];
        //Setup Player Animations
        Anim.iStartingFrame = mTextureAtlas.GetIndexByTitle("bunny1.png");
        Anim.fDefaultFPS = 14.0f;
        Anim.nFrames = 9;

        mAnimation.Animations[1] = new AnimationTrack();
        Anim = mAnimation.Animations[1];
        Anim.iStartingFrame = mTextureAtlas.GetIndexByTitle("giant1.png");
        Anim.fDefaultFPS = 6.0f;
        Anim.nFrames = 4;
     
        mAnimation.Animations[2] = new AnimationTrack();
        Anim = mAnimation.Animations[2];
        Anim.iStartingFrame = mTextureAtlas.GetIndexByTitle("morph1.png");
        Anim.fDefaultFPS = 6.0f;
        Anim.nFrames = 7;
        
        mAnimation.Animations[3] = new AnimationTrack();
        Anim = mAnimation.Animations[3];
        Anim.iStartingFrame = mTextureAtlas.GetIndexByTitle("slam1.png");
        Anim.fDefaultFPS = 16.0f;
        Anim.nFrames = 4;
	}
	public void TakeHit(){
		if(DamageDelay <= 0.0f){
			if(HitPoints > 0){
				HitPoints--;
				DamageDelay = DamageGracePeriod;
				HealthDisplay = 3.5f;
				if(HitPoints == 0){
					Globals.mGameGraph.ScreenFadeAmount = 0.0f;
					Globals.mGameGraph.ScreenFadeDirection = 2;
				}
			}
		}
		return;
	}

	public void HandleCollisions(final GameGraph mGameGraph, final int SliceIndex){
		final GameObjectGridSlice Slice = mGameGraph.ObjectGrid.Slices[SliceIndex];
		GameObject go = Slice.ObjectLists[0].RootObject;
		while(go != null){
			if(CollisionManager.Collision(CD, go.CD)){
				//TODO - SWITCH TYPE FOR POINTS
				final int Type = go.Info.Type;
				if(Type == GameObjectTypes.LevelExit){
					Globals.GameActivity.runOnUiThread(Globals.GameActivity.ShowLevelCompletion);
				}else if(Type == GameObjectTypes.CheckPoint){
					CheckpointTime = LevelTime;
					FoofGame.TriggerCheckPoint(go.X, go.Y);
				}else{
					
					Score.AddPoints((Type + 1) * 50);
					SoundSystem.PlaySound(Collectible.CollectibleA_SoundID);
					mGameGraph.AddExplosion(go.X, go.Y, Explosion.CollectionPuftA, bFlip);
					go.Remove();
					if(go.Info == FoofGame.CollectiblesInfo[7]){
						//It's morphin' time! (don't hurt me)
						TransformToSuper();
					}else if(go.Info == FoofGame.CollectiblesInfo[8]){
						//Single Energy recovery power-up
						//CustomToastSystem.ShowToast("Yummy!");
						if(HitPoints < 5)
							HitPoints++;
						HealthDisplay = 3.5f;
					}else if(go.Info == FoofGame.TokenInfo){
						PlayerProgress.CollectToken();
					}
				}
				break;
			}
			go = go.Next;
		}
		//Look for hazards
		if(!bIsSuperBunny){
			go = Slice.ObjectLists[2].RootObject;
			while(go != null){
				if(CollisionManager.Collision(CD, go.CD)){
					TakeHit();
					break;
				}else{
					go = go.Next;
				}
			}
		}
		//Look for enemies
		go = Slice.ObjectLists[3].RootObject;
		while(go != null){
			if(CollisionManager.Collision(CD, go.CD)){
				if(go.bIsPlatform == 0){
					//TODO - Make this platform stuff feel like less of a hack
					if(!bIsSuperBunny){
						TakeHit();
					}else if(go.Info.bIsKillable == 1 && CurrentAnimationType == 2){
						if(bFlip){
							if(go.X < X)
								((DynamicObject)go).HP -= 3; //TODO - decide damage output
						}else if(go.X > X){
							((DynamicObject)go).HP -= 3;
						}
					}
				}else{
					if(mMountedPlatform == null && Player.YVelocity < 0.0f){
						if(PlayerFeet < go.CD.Top){
							mMountedPlatform = (DynamicObject)go;
							mMountedPlatform.PlayerSteppedOn();
							FloorHeight = go.CD.Top;
							bHitGround = true;		
						}
					}			
				}
				break;
			}else{
				go = go.Next;
			}
		}
	}
	public void CheckCollisions(final GameGraph mGameGraph){
		final int RIndex = (int)(CD.Right / GameGraph.LevelTileOffset);
		if(mGameGraph.WithinGridX(RIndex))
			HandleCollisions(mGameGraph, RIndex);
		final int LIndex = (int)(CD.Left / GameGraph.LevelTileOffset);
		if(RIndex != LIndex && mGameGraph.WithinGridX(LIndex))
			HandleCollisions(mGameGraph, LIndex);
	}
	public void Update(final GameGraph mGameGraph){
		if(HitPoints > 0){
			//First of all, see if we're not in a bottomless pit
			PlayerCellX = (int)(X / GameGraph.LevelTileOffset);
			final int PlayerCellY = (int)(Y / GameGraph.LevelTileOffset);
			
			//Encountered Bottomless pit or fell through level bounds?
			if(Y < 0.0f || (mGameGraph.WithinGridX(PlayerCellX) && mGameGraph.WithinGridY(PlayerCellY) && mGameGraph.LevelGrid[PlayerCellX][PlayerCellY] == LevelTile.BottomlessPit)){
				HitPoints = 0;
				mGameGraph.ScreenFadeAmount = 0.0f;
				mGameGraph.ScreenFadeDirection = 2;
			}else{
				LevelTime += Globals.secondsSinceLastFrame;
				//If we're morphing, freeze the player and morph
				//TODO - Make this look less ugly...
				if(MorphDirection == 1){
					if(!mAnimation.bIsPlaying){
						mAnimation.SetTrack(1);
						mAnimation.Behaviour = AnimationType.Behaviour_Loop;
						MorphDirection = 0;
					}
					CurrentSprite = mAnimation.AdvanceFrame();
				}else if(MorphDirection == -1){
					if(mAnimation.fCurrentFrame < 1.0f){
						mAnimation.SetTrack(0);
						mAnimation.Behaviour = AnimationType.Behaviour_Loop;
						MorphDirection = 0;
						InitCD(Player.Scale);
						bIsSuperBunny = false;
					}
					CurrentSprite = mAnimation.AdvanceFrame();
				}else{
					float XDelta = 0.0f;
					if(bIsSuperBunny){
						SuperTimer -= Globals.secondsSinceLastFrame;
						if(SuperTimer <= 0.0f){
							ReturnToNormal();
						}
					}
					if(CurrentAnimationType != 0){
						CurrentSprite = mAnimation.AdvanceFrame();
						if(!mAnimation.bIsPlaying){
							if(mAnimation.CurrentTrack == 3){
								mAnimation.SetTrack(1);
								mAnimation.Behaviour = AnimationType.Behaviour_Loop;
								CurrentAnimationType = 0;
								CurrentSprite = mAnimation.AdvanceFrame();
							}
						}
					}
					//Otherwise, allow movement and advance frames if we're moving
					if(GameInputState.DPadRight){
						XDelta = Player.Speed * Globals.secondsSinceLastFrame;
						bFlip = false;
						if(CurrentAnimationType == 0)
							CurrentSprite = mAnimation.AdvanceFrame();
					}else if(GameInputState.DPadLeft){
						XDelta = -Player.Speed * Globals.secondsSinceLastFrame;
						bFlip = true;
						if(CurrentAnimationType == 0)
							CurrentSprite = mAnimation.AdvanceFrame();
					}
					bHitGround = false;
					
					DamageDelay -= Globals.secondsSinceLastFrame;
					HealthDisplay -= Globals.secondsSinceLastFrame;

					if(mMountedPlatform == null){
						Player.YVelocity += Globals.fGravity;
					}else{
						Player.YVelocity = mMountedPlatform.VelocityY;
					}
					float YDelta = Player.YVelocity * Globals.secondsSinceLastFrame;
					PlayerFeet = CD.Bottom + YDelta;
					final float HalfHeight = Y - CD.Bottom;
					final float WallSenseHeight = Y - 0.1f;
					if(mMountedPlatform != null){				
						XDelta += mMountedPlatform.VelocityX;
					}
					if(mGameGraph.CollisionAgainstWall(CD, XDelta, WallSenseHeight) || mGameGraph.CollisionAgainstWall(CD, XDelta, CD.Top)){
						XDelta = 0.0f;
					}else{
						X += XDelta;
					}		
					CheckCollisions(mGameGraph);
					//Ground overrides platforms - platforms don't send you through ground
					if(!bHitGround){
						if(mMountedPlatform != null){
							if(CD.Left > mMountedPlatform.CD.Right || CD.Right < mMountedPlatform.CD.Left){
								mMountedPlatform = null;
							}
						}
						final float TileHeight = mGameGraph.FindFloorIntersectionHeight(CD.Left, CD.Right, PlayerFeet);
						if(PlayerFeet < TileHeight && (Player.YVelocity < 0.0f || CollisionManager.LastLevelTileHit.bIsNotWall == 0)){
							FloorHeight = TileHeight;
							bHitGround = true;
							mMountedPlatform = null;
						}

					}
					if(bHitGround){
						final float NewPos = FloorHeight + HalfHeight + 0.001f;
						YDelta = NewPos - Y;
						Y = NewPos;
						Player.JumpCount = 0;
						Player.YVelocity = 0.0f;
					}else{
						//CEILING TIME!
						final float FutureHead = CD.Top + YDelta;
						final float TileBottom = mGameGraph.FindCeilingIntersectionHeight(CD.Left, CD.Right, FutureHead);
						if(TileBottom > Y && FutureHead > TileBottom){
							//Hit the ceiling, compute new position.
							final float NewPos = TileBottom - HalfHeight - 0.001f; 
							YDelta = NewPos - Y;
							Y = NewPos;
							if(Player.YVelocity > 0.0f){
								Player.YVelocity = 0.0f;
							}
						}else{
							Y += YDelta;
						}
					}
					CD.Translate(XDelta, YDelta);
					if(GameInputState.TapFire){
						GameInputState.TapFire = false;
						//If we're buff, perform a slam
						if(bIsSuperBunny){
							CurrentAnimationType = 2;
							mAnimation.SetTrack(3);
							mAnimation.Behaviour = AnimationType.Behaviour_OneShot;
						}else{ //otherwise, shootttt
							final PlayerBullet Bullet = mGameGraph.mPlayerBullets.Add();
							if(Bullet != null){
								final float OriginX = X + ((bFlip) ? -0.19f : 0.19f);
								final float OriginY = Y - 0.02f;								
								mGameGraph.AddExplosion(OriginX, OriginY, Explosion.PlayerMuzzle, bFlip);
								Bullet.bFlip = bFlip;
								Bullet.CD.Copy(PlayerBullet.BulletCD);
								Bullet.CD.Scale(PlayerBullet.DefaultScale);
								Bullet.CD.Translate(X, Y);
								Bullet.X = OriginX;
								Bullet.Y = OriginY;
								SoundSystem.PlaySound(PlayerBullet.GunshotSoundID);
							}
						}
					}
				}

			}


		}else{
			//Death stuff
			HealthDisplay = 1.0f;
			DeathCounter -= Globals.secondsSinceLastFrame;
			if(DeathCounter > 2.0f){
				mGameGraph.ScreenFadeDirection = 2;
			}
			if(DeathCounter < 0.0f){
				Globals.mGame.RestartLevel();
			}
		}
		return;
	}
	public String GetLevelTime(){
		final int Minutes = (int)(LevelTime / 60.0f);
		final int Seconds = (int) (LevelTime - (Minutes * 60));
		return Minutes + ":" + Seconds;
	}
}
