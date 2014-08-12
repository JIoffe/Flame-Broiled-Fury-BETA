package com.Foofles;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;


import android.util.Log;

/**
 * This class specifically handles read / write operations
 * for level data and player saves. 
 * 
 * Abstract interface
 * 
 * @author Jason Ioffe
 *
 */
public class GameDataIO {
	public static boolean LoadLevelResource(final InputStream is, final GameGraph mGameGraph){
		//TODO - Contemplate making this a little faster by saving values on the stack
		// in final form
		if(mGameGraph != null){
			//Holy long line of code batman.
			final BufferedReader reader = new BufferedReader(new InputStreamReader(is));
			String line = null;
			try{
				line = reader.readLine();
				if(line.equals("Foof72")){
					line = reader.readLine();
					mGameGraph.LevelGridWidth = Integer.parseInt(line);
					line = reader.readLine();
					mGameGraph.LevelGridHeight = Integer.parseInt(line);
					mGameGraph.LevelGrid = new LevelTile[mGameGraph.LevelGridWidth][mGameGraph.LevelGridHeight];
					
					mGameGraph.ObjectGrid.InitializeGrid(mGameGraph.LevelGridWidth);
					mGameGraph.LevelGridMeasuredWidth = mGameGraph.LevelGridWidth * GameGraph.LevelTileOffset;
					mGameGraph.LevelGridMeasuredHeight = mGameGraph.LevelGridHeight * GameGraph.LevelTileOffset;
					
					mGameGraph.MaxW = mGameGraph.LevelGridWidth - 1;
					mGameGraph.MaxH = mGameGraph.LevelGridHeight - 1;
					
					mGameGraph.LevelGridWLookupMultiplier = mGameGraph.LevelGridWidth / mGameGraph.LevelGridMeasuredWidth;
					for(int y = 0; y < mGameGraph.LevelGridHeight; y++){
						for(int x = 0; x < mGameGraph.LevelGridWidth; x++){
							line = reader.readLine();
							final int TileID = Integer.parseInt(line);
							if(TileID > -1){
								if(TileID == 127){
									mGameGraph.LevelGrid[x][y] = LevelTile.BottomlessPit;
								}else{
									mGameGraph.LevelGrid[x][y] = mGameGraph.TileSet[TileID];
								}
							}else{
								mGameGraph.LevelGrid[x][y] = LevelTile.BlankTile;
							}
						}
					}
					//GetPlayer Pos
					line = reader.readLine();
					mGameGraph.mPlayer.X = Float.parseFloat(line);
					line = reader.readLine();
					mGameGraph.mPlayer.Y = Float.parseFloat(line);
					
					if(FoofGame.CheckPointX > 0.0f){
						mGameGraph.mPlayer.X = FoofGame.CheckPointX;
						mGameGraph.mPlayer.Y = FoofGame.CheckPointY;
					}
					Player.YVelocity = 0.0f;
					
					//Get Exit
					mGameGraph.ExitObject = new GameObject();
					mGameGraph.ExitObject.Info = FoofGame.ExitInfo;
					mGameGraph.ExitObject.X = Float.parseFloat(reader.readLine());
					mGameGraph.ExitObject.Y = Float.parseFloat(reader.readLine());
					mGameGraph.ExitObject.InitCD();
					mGameGraph.ObjectGrid.SortObject(mGameGraph.ExitObject);
					
					//CAMERA init
					mGameGraph.CamX = mGameGraph.mPlayer.X;
					mGameGraph.CamY = mGameGraph.mPlayer.Y + 0.6f;
					
					
					//Init the shared resource pool - read in grand total of game objects
					line = reader.readLine();
					//TODO - Reconsider one mass object pool
					
					//Read in props (collectibles, hazards, etc)
					line = reader.readLine();
					final int nProps = Integer.parseInt(line);
					if(nProps > 0){
						for(int i = 0; i < nProps; i++){
							final GameObject go = new Collectible();
							
							line = reader.readLine();
							final int Type = Byte.parseByte(line);
							line = reader.readLine();
							go.X = Float.parseFloat(line);
							line = reader.readLine();
							go.Y = Float.parseFloat(line);
							
							if(Type == GameObjectTypes.CheckPoint){
								go.Info = FoofGame.CheckPointInfo;
							}else{
								go.Info = FoofGame.CollectiblesInfo[Type];
							}
							go.InitCD();
							//Stick it into the grid
							mGameGraph.ObjectGrid.SortObject(go);
						}
					}
					//Read in DYNAMICS, if any
					line = reader.readLine();
					final int nDynamics = Integer.parseInt(line);
					if(nDynamics > 0){
						for(int i = 0; i < nDynamics; i++){
							line = reader.readLine();
							final int Type = Integer.parseInt(line);
							final float X = Float.parseFloat(reader.readLine());
							final float Y = Float.parseFloat(reader.readLine());							
							final GameObjectInfo DynamicInfo = mGameGraph.DynamicObjectSet[Type];
							DynamicObject go = null;
							switch(DynamicInfo.SubType){
								case 1:{ //Flyer
									go = new FlyingEnemy();
									go.VisibilityMin = X - DynamicInfo.fDefaultScale;
									go.VisibilityMax = X + DynamicInfo.fDefaultScale;	
									((FlyingEnemy)go).OriginY = Y;
									break;
								}
								case 2:{ //Mover
									go = new Mover();
									Mover mover = (Mover)go;
									mover.uFunctionType = Byte.parseByte(reader.readLine());
									mover.uMoverType = Byte.parseByte(reader.readLine());
									mover.OriginX = Float.parseFloat(reader.readLine());
									mover.OriginY = Float.parseFloat(reader.readLine());
									mover.fSpeed = Float.parseFloat(reader.readLine());
									mover.Width = Float.parseFloat(reader.readLine());									
									mover.Pause = Float.parseFloat(reader.readLine());
									mover.Sync = Float.parseFloat(reader.readLine());
									mover.ComputeVisibleArea();
									//mover.mAnimation.bIsPlaying = true;
									break;
								}
								case 3:{ //SnapMouth
									go = new Snapmouth();
									go.bIsPlatform = 1;
									go.VisibilityMax = X;
									go.VisibilityMin = X;
									((Snapmouth)go).XOrigin = X;
									break;
								}
								case 8:{ //SPARKLES!!!!
									go = new SparkleParticleSystem(8, 0.35f, DynamicInfo.nSpriteID);
									go.bIsPlatform = 0;
									go.VisibilityMax = X;
									go.VisibilityMin = X;
									
									break;
								}
								case 9:{
									go = new GiantMantis(X, Y);
									break;
								}
								case 10:{
									go = new FatSlimyBug(X, Y);
									break;
								}								
								case 5:{ //Destructible
									go = new DestructibleObject();
									final DestructibleObject DO = (DestructibleObject)go;
									DO.VisibilityMax = Float.parseFloat(reader.readLine());;
									DO.VisibilityMin = Float.parseFloat(reader.readLine());;
									DestructibleObjectInfo DOI = (DestructibleObjectInfo)DynamicInfo;
									if(DOI.SpawnType == 3){ //Spawn enemy object
										//TODO - HAndle spawning enemies and goodies...
									}
									break;
								}
								case 6:{ //Farty plant
									go = new FartPlant();
									go.bIsPlatform = 0;
									go.VisibilityMax = X;
									go.VisibilityMin = X;								
									break;
								}
								case 7:{ //Falling platform
									go = new FallingPlatform();
									go.bIsPlatform = 1;
									go.VisibilityMax = X;
									go.VisibilityMin = X;								
									break;
								}								
								default:
									go = new BasicEnemy();
									final BasicEnemy Enemy = (BasicEnemy)go;
									go.VisibilityMin = Float.parseFloat(reader.readLine());
									go.VisibilityMax = Float.parseFloat(reader.readLine());		
									Enemy.bIsDormant = (((EnemyInfo)DynamicInfo).bDormant == 1);
									if(Enemy.bIsDormant){
										Enemy.mAnimation.bIsPlaying = false;
									}
									break;
							}

							go.X = X;
							go.Y = Y;
							go.Type = GameObjectTypes.Enemy;
							go.Info = DynamicInfo;
							go.bIsPlatform = DynamicInfo.bIsPlatform;
							go.HP = DynamicInfo.HP;
							
							go.mAnimation.Animations = go.Info.Animations;
							if(DynamicInfo.SubType != 3){
								if(go.mAnimation.Animations[0] != null){
									go.mAnimation.bIsPlaying = true;
								}else{
									go.mAnimation.bIsPlaying = false;
								}
							}
							go.InitCD();
							mGameGraph.ObjectGrid.SortObject(go);
							mGameGraph.ObjectGrid.SortDynamicPVS(go);
						}
					}
					//Read in any haz ma
					final int nHazards = Integer.parseInt(reader.readLine());
					if(nHazards > 0){
						
						for(int i = 0; i < nHazards; i++){
							final GameObject go = new GameObject();
							go.Type = GameObjectTypes.Hazard;
							line = reader.readLine();
							final int Type = Integer.parseInt(line);
							line = reader.readLine();
							go.X = Float.parseFloat(line);
							line = reader.readLine();
							go.Y = Float.parseFloat(line);
							
							//Figure out where to stick the damn thing.
							go.Info = mGameGraph.HazardousObjectSet[Type];
							go.InitCD();
							mGameGraph.ObjectGrid.SortObject(go);
						}
					}		
					//Read in any STATIC DECO
					line = reader.readLine();
					final int nDecos = Integer.parseInt(line);
					if(nDecos > 0){
						for(int i = 0; i < nDecos; i++){
							final GameObject go = new GameObject();
							go.Type = GameObjectTypes.StaticDecoration;
							line = reader.readLine();
							final int Type = Byte.parseByte(line);
							line = reader.readLine();
							go.X = Float.parseFloat(line);
							line = reader.readLine();
							go.Y = Float.parseFloat(line);
							
							//Figure out where to stick the damn thing.
							go.Info = mGameGraph.StaticObjectSet[Type];
							go.InitCD();
							mGameGraph.ObjectGrid.SortObject(go);					
						}
					}					
					final int nTokens = Integer.parseInt(reader.readLine());
					//READ TOKENS
					if(nTokens > 0){
						for(int i = 0; i < nTokens; i++){
							final GameObject go = new GameObject();
							go.Type = GameObjectTypes.Collectible;
							line = reader.readLine();
							go.X = Float.parseFloat(line);
							line = reader.readLine();
							go.Y = Float.parseFloat(line);
							
							//Figure out where to stick the damn thing.
							go.Info = FoofGame.TokenInfo;
							go.InitCD();
							mGameGraph.ObjectGrid.SortObject(go);					
						}						
					}
					final int nBunnies = Integer.parseInt(reader.readLine());
					if(nBunnies > 0){
						for(int i = 0; i < nBunnies; i++){
							final BunnyHostage NewBunny = new BunnyHostage(i);
							line = reader.readLine();
							NewBunny.X = Float.parseFloat(line);
							line = reader.readLine();
							NewBunny.Y = Float.parseFloat(line);
							
							NewBunny.InitCD();
							NewBunny.VisibilityMin = NewBunny.X;
							NewBunny.VisibilityMax = NewBunny.X;
							mGameGraph.ObjectGrid.SortObject(NewBunny);
							mGameGraph.ObjectGrid.SortDynamicPVS(NewBunny);
						}
					}
					//Read in CAmera Zones if any
					line = reader.readLine();
					final int nCamZones = Integer.parseInt(line);
					if(nCamZones > 0){
						mGameGraph.nCameraZones = nCamZones;
						mGameGraph.CameraZones = new CamZone[nCamZones];
						for(int i = 0; i < nCamZones; i++){
							mGameGraph.CameraZones[i] = new CamZone();
							final CamZone Cam = mGameGraph.CameraZones[i];
							line = reader.readLine();
							Cam.X = Float.parseFloat(line);
							line = reader.readLine();
							Cam.Y = Float.parseFloat(line);
							line = reader.readLine();
							Cam.Left = Float.parseFloat(line);
							line = reader.readLine();
							Cam.Right = Float.parseFloat(line);
							line = reader.readLine();
							Cam.Up = Float.parseFloat(line);
							line = reader.readLine();
							Cam.Down = Float.parseFloat(line);
							line = reader.readLine();
							Cam.uAxisAlignment = Byte.parseByte(line);						
						}
					}
					return true;
				}
			}
			catch(IOException e){
				//Something went wrong
				Log.d("Game Graph", "Error reading level!");
				try {
					reader.close();
					is.close();
				} catch (IOException e3) {
					//This shouldn't matter, we're closing it, right?
				}
					return false;
			} 
			return false;
		}
		return false;
	}
	public static boolean LoadLevelTileset(final InputStream is, final GameGraph mGameGraph){
		if(mGameGraph != null){
			//Holy long line of code batman.
			final BufferedReader reader = new BufferedReader(new InputStreamReader(is));
			String line = null;
			try{
				line = reader.readLine();
				if(line.equals("Foof72")){
					line = reader.readLine();
					final int nLevelTiles = Integer.parseInt(line);
					if(nLevelTiles > 0){
						mGameGraph.TileSet = new LevelTile[nLevelTiles];
						for(int i = 0; i < nLevelTiles; i++){
							mGameGraph.TileSet[i] = new LevelTile();
							line = reader.readLine();
							final LevelTile levelTile = mGameGraph.TileSet[i];
							levelTile.SpriteID = Byte.parseByte(line);
							levelTile.bIsCeiling = Byte.parseByte(reader.readLine());
							levelTile.bIsNotWall = Byte.parseByte(reader.readLine());
							for(int h = 0; h < 64; h++){
								levelTile.HeightMask[h] = Byte.parseByte(reader.readLine());
							}
							line = reader.readLine();
						}
					}
					line = reader.readLine();
					int nAnimationTracks = Integer.parseInt(line);
					mGameGraph.AnimationTrackSet = new AnimationTrack[nAnimationTracks];
					if(nAnimationTracks > 0){
						for(int i = 0; i < nAnimationTracks; i++){
							mGameGraph.AnimationTrackSet[i] = new AnimationTrack();
							final AnimationTrack Anim = mGameGraph.AnimationTrackSet[i];
							Anim.Name = reader.readLine();
							line = reader.readLine();
							Anim.iStartingFrame = Integer.parseInt(line);
							line = reader.readLine();
							Anim.nFrames = Integer.parseInt(line);
							line = reader.readLine();
							Anim.fDefaultFPS = Float.parseFloat(line);
						}
					}
					
					
					//Add Dynamic Object List
					line = reader.readLine();
					final int nDynamicTypes = Integer.parseInt(line);
					if(nDynamicTypes > 0){
						mGameGraph.DynamicObjectSet = new GameObjectInfo[nDynamicTypes];
						for(int i = 0; i < nDynamicTypes; i++){
							line = reader.readLine();
							final int Type = Integer.parseInt(line);
							switch(Type){
								case 1:{ //Flyer
									mGameGraph.DynamicObjectSet[i] = new EnemyInfo();
									final EnemyInfo EI = (EnemyInfo) mGameGraph.DynamicObjectSet[i];
									EI.NewAnimations(3);
									final int AttackAnimationIndex = Integer.parseInt(reader.readLine());
									final int DeathAnimationIndex = Integer.parseInt(reader.readLine());
									if(AttackAnimationIndex > -1)
										EI.mAttackAnimation = mGameGraph.AnimationTrackSet[AttackAnimationIndex];
									if(DeathAnimationIndex > -1)
										EI.mDeathAnimation = mGameGraph.AnimationTrackSet[DeathAnimationIndex];
									EI.Speed = Float.parseFloat(reader.readLine());
									EI.HP = Byte.parseByte(reader.readLine());
									EI.Agro = Integer.parseInt(reader.readLine());
									EI.bDormant = Byte.parseByte(reader.readLine());
									EI.ShotType = Byte.parseByte(reader.readLine());
									EI.ShotDirection = Byte.parseByte(reader.readLine());
									EI.ShotDelay = Float.parseFloat(reader.readLine());
									EI.nShotsPerBurst = Byte.parseByte(reader.readLine());
									break;									
								}
								case 2:{ // MOVER
									mGameGraph.DynamicObjectSet[i] = new MoverInfo();
									final MoverInfo MI = (MoverInfo) mGameGraph.DynamicObjectSet[i];
									MI.NewAnimations(2);
									//TODO - Wrap this into a function to reduce code size by half
									MI.SetAnimationTrack(1, Integer.parseInt(reader.readLine()));
									
									MI.HP = Byte.parseByte(reader.readLine());
									MI.bIsHazard = Boolean.parseBoolean(reader.readLine());
									if(!MI.bIsHazard){
										MI.bIsPlatform = 1;
									}
									break;
								}
								case 3:{ //SnapMouth - Eh whatever I'll hardcode it
									mGameGraph.DynamicObjectSet[i] = new GameObjectInfo();
									mGameGraph.DynamicObjectSet[i].NewAnimations(1);
									mGameGraph.DynamicObjectSet[i].bIsPlatform = 1;
									mGameGraph.DynamicObjectSet[i].bIsKillable = 0;
									break;
								}
								case 8:{ // Sparkles!!!!
									mGameGraph.DynamicObjectSet[i] = new GameObjectInfo();
									mGameGraph.DynamicObjectSet[i].NewAnimations(1);
									mGameGraph.DynamicObjectSet[i].bIsPlatform = 0;
									mGameGraph.DynamicObjectSet[i].bIsKillable = 0;
									break;
								}
								case 6:{ //fartplant - Eh whatever I'll hardcode it
									mGameGraph.DynamicObjectSet[i] = new GameObjectInfo();
									mGameGraph.DynamicObjectSet[i].NewAnimations(1);
									mGameGraph.DynamicObjectSet[i].bIsPlatform = 0;
									mGameGraph.DynamicObjectSet[i].bIsKillable = 0;
									break;
								}					
								case 7:{ //falling platform- Eh whatever I'll hardcode it
									mGameGraph.DynamicObjectSet[i] = new GameObjectInfo();
									mGameGraph.DynamicObjectSet[i].NewAnimations(1);
									mGameGraph.DynamicObjectSet[i].bIsPlatform = 1;
									mGameGraph.DynamicObjectSet[i].bIsKillable = 0;
									break;
								}
								case 9:{ //MANTIS
									mGameGraph.DynamicObjectSet[i] = new GameObjectInfo();
									final GameObjectInfo GOI = mGameGraph.DynamicObjectSet[i]; 
									GOI.NewAnimations(1);
									GOI.bIsPlatform = 0;
									GOI.bIsKillable = 1;
									GOI.HP = 12;
									break;
								}		
								case 10:{ //Fat Bug
									mGameGraph.DynamicObjectSet[i] = new GameObjectInfo();
									final GameObjectInfo GOI = mGameGraph.DynamicObjectSet[i]; 
									GOI.NewAnimations(1);
									GOI.bIsPlatform = 0;
									GOI.bIsKillable = 1;
									GOI.HP = 20;
									break;
								}
								case 5:{ // Destructible
									mGameGraph.DynamicObjectSet[i] = new DestructibleObjectInfo();
									final DestructibleObjectInfo DI = (DestructibleObjectInfo) mGameGraph.DynamicObjectSet[i];
									DI.NewAnimations(1);
									DI.HP = Byte.parseByte(reader.readLine());
									DI.SpawnType = Integer.parseInt(reader.readLine());
									DI.SpawnID = Integer.parseInt(reader.readLine());
									break;
								}
								default: { //Ground based enemy
									mGameGraph.DynamicObjectSet[i] = new EnemyInfo();
									final EnemyInfo EI = (EnemyInfo) mGameGraph.DynamicObjectSet[i];
									EI.NewAnimations(3);
									final int AttackAnimationIndex = Integer.parseInt(reader.readLine());
									final int DeathAnimationIndex = Integer.parseInt(reader.readLine());
									if(AttackAnimationIndex > -1)
										EI.mAttackAnimation = mGameGraph.AnimationTrackSet[AttackAnimationIndex];
									if(DeathAnimationIndex > -1)
										EI.mDeathAnimation = mGameGraph.AnimationTrackSet[DeathAnimationIndex];
									EI.JumpStrength = Float.parseFloat(reader.readLine());
									EI.Speed = Float.parseFloat(reader.readLine());
									EI.HP = Byte.parseByte(reader.readLine());
									EI.Agro = Integer.parseInt(reader.readLine());
									EI.bDormant = Byte.parseByte(reader.readLine());
									EI.ShotType = Byte.parseByte(reader.readLine());
									EI.ShotDirection = Byte.parseByte(reader.readLine());
									EI.ShotDelay = Float.parseFloat(reader.readLine());
									EI.nShotsPerBurst = Byte.parseByte(reader.readLine());
									break;
								}
									
							}
							final GameObjectInfo GOI = mGameGraph.DynamicObjectSet[i];
							if(GOI.HP > -1){
								GOI.bIsKillable = 1;
							}
							GOI.SpriteBatch = 1;
							GOI.SubType = Type;
							GOI.Type = 1;
							GOI.Name = reader.readLine();
							GOI.SetAnimationTrack(0, Integer.parseInt(reader.readLine()));							
							GOI.nSpriteID = Integer.parseInt(reader.readLine());
							GOI.fDefaultScale = Float.parseFloat(reader.readLine());
							GOI.CD.Left = Float.parseFloat(reader.readLine());
							GOI.CD.Right = Float.parseFloat(reader.readLine());
							GOI.CD.Top = Float.parseFloat(reader.readLine());
							GOI.CD.Bottom = Float.parseFloat(reader.readLine());
						}
					}
					//Add Hazardous Object List
					line = reader.readLine();
					final int nHazTypes = Integer.parseInt(line);
					if(nHazTypes > 0){
						mGameGraph.HazardousObjectSet = new HazardousObjectInfo[nHazTypes];
						for(int i = 0; i < nHazTypes; i++){
							mGameGraph.HazardousObjectSet[i] = new HazardousObjectInfo();
							final HazardousObjectInfo Haz = mGameGraph.HazardousObjectSet[i];
							Haz.SpriteBatch = 1;
							Haz.Type = GameObjectTypes.Hazard;
							line = reader.readLine();
							Haz.Name = line;
							line = reader.readLine();
							Haz.nSpriteID = Integer.parseInt(line);
							line = reader.readLine();
							Haz.Damage = Byte.parseByte(line);
							line = reader.readLine();
							Haz.fDefaultScale = Float.parseFloat(line);
							Haz.CD.Left = Float.parseFloat(reader.readLine());
							Haz.CD.Right = Float.parseFloat(reader.readLine());
							Haz.CD.Top = Float.parseFloat(reader.readLine());
							Haz.CD.Bottom = Float.parseFloat(reader.readLine());
						}
					}		
					//Add Static Deco Object List
					line = reader.readLine();
					final int nDecoTypes = Integer.parseInt(line);
					if(nDecoTypes > 0){
						mGameGraph.StaticObjectSet = new GameObjectInfo[nDecoTypes];
						for(int i = 0; i < nDecoTypes; i++){
							mGameGraph.StaticObjectSet[i] = new GameObjectInfo();
							final GameObjectInfo Deco = mGameGraph.StaticObjectSet[i];
							Deco.SpriteBatch = 1;
							Deco.Type = GameObjectTypes.StaticDecoration;
							line = reader.readLine();
							Deco.Name = line;
							line = reader.readLine();
							Deco.nSpriteID = Integer.parseInt(line);
							line = reader.readLine();
							Deco.fDefaultScale = Float.parseFloat(line);
							Deco.CD.Left = Float.parseFloat(reader.readLine());
							Deco.CD.Right = Float.parseFloat(reader.readLine());
							Deco.CD.Top = Float.parseFloat(reader.readLine());
							Deco.CD.Bottom = Float.parseFloat(reader.readLine());
						}
					}			
					final int nProjectiles = Integer.parseInt(reader.readLine());
					if(nProjectiles > 0){
						mGameGraph.ProjectileSet = new GameObjectInfo[nProjectiles];
						for(int i = 0; i < nProjectiles; i++){
							mGameGraph.ProjectileSet[i] = new GameObjectInfo();
							final GameObjectInfo Proj = mGameGraph.ProjectileSet[i];
							Proj.SpriteBatch = 1;
							Proj.Name = reader.readLine();
							Proj.nAnimationTracks = 1;
							Proj.NewAnimations(1);
							Proj.SetAnimationTrack(0, Integer.parseInt(reader.readLine()));
							Proj.fDefaultScale = Float.parseFloat(reader.readLine());
							Proj.CD.Left = Float.parseFloat(reader.readLine());
							Proj.CD.Right = Float.parseFloat(reader.readLine());
							Proj.CD.Top = Float.parseFloat(reader.readLine());
							Proj.CD.Bottom = Float.parseFloat(reader.readLine());		
						}
					}
				}
			}catch(IOException e){
				//Something went wrong
				Log.d("Game Graph", "Error reading level!");
				try {
					reader.close();
					is.close();
				} catch (IOException e3) {
					//This shouldn't matter, we're closing it, right?
				}
					return false;
			} 
			return false;
		}
		return false;
	}
}
