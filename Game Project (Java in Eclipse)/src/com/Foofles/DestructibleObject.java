package com.Foofles;
/**
 * DestructibleObject represents scenery props that can be destroyed by
 * the player, such as explosive barrels and fruit. 
 * These either give the player points directly,
 * spawn collectibles,
 * spawn enemies,
 * or create a massive explosion that damages all within radius
 * (Player, enemies, and other destructable objects.
 * 
 * @author Jason Ioffe
 *
 */
public class DestructibleObject extends DynamicObject{
	final public static float ExplosionRadius = 0.8f;
	public GameObject ObjectToSpawn = null;
	private float ExplosionDelay = 0.3f;
	private boolean bExplosionTriggered = false;
	DestructibleObject(){
		
	}
	@Override
	public boolean Update(){
		if(bExplosionTriggered){
			ExplosionDelay -= Globals.secondsSinceLastFrame;
		}
		if(HP < 1 || ExplosionDelay < 0.0f){
			//Kaput
			final DestructibleObjectInfo DOI = (DestructibleObjectInfo)Info;
			switch(DOI.SpawnType){
				case 0:{
					//Assplosion. Does up to 4 hits worth of damage.
					//TODO - there are so many things I don't like here.
					//For one, I don't know if I want to subtract the 4 hp from
					//Enemies or just kill them/player outright.
					//Secondly, the "if class ==..." part feels naive.
					//Third, i'm not crazy about the explosion delay, either!
					
					final float ExpRadSq = ExplosionRadius*ExplosionRadius;
					Globals.mGameGraph.AddExplosion(X, Y, Explosion.FieryExplosion, bFlip);
					final int SliceIndexLeft = (int)((X - ExplosionRadius) / GameGraph.LevelTileOffset);
					final int SliceIndexRight = (int)((X + ExplosionRadius) / GameGraph.LevelTileOffset)+1;
					for(int i = SliceIndexLeft; i < SliceIndexRight; i++){
						GameObject go = Globals.mGameGraph.ObjectGrid.Slices[i].ObjectLists[3].RootObject;
						while(go != null){
							if(go.Info.bIsKillable == 1){

								if(FoofMath.SquardDistance(X, Y, go.X, go.Y) < ExpRadSq){
									if(go.getClass() == DestructibleObject.class){
										((DestructibleObject)go).bExplosionTriggered = true;
									}else{
										((DynamicObject)go).HP -= 4;
									}
								}
							}
							go = go.Next;			
						}
					}
					//Are we in range of the player? Instantly remove 4 hp of damage.
					final Player mPlayer = Globals.mGameGraph.mPlayer;
					if(FoofMath.SquardDistance(X, Y, mPlayer.X, mPlayer.Y) < ExpRadSq){
						Player.HitPoints -= 3;
						mPlayer.TakeHit();
					}
					break;
				}
				default:
					break;
			}
			return false;
		}
		return true;
	}
}
