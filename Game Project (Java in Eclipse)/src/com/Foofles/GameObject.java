package com.Foofles;

/**
 * Base class for game objects like Props, Collectibles, Enemies, and the player.
 * 
 * @author Jason Ioffe
 *
 */
public class GameObject extends SpriteComponent{
	

	public byte Type = 0;
	public GameObjectInfo Info;
	

	
	final public CollisionData CD = new CollisionData();
	
	public GameObject Next = null;
	public GameObject Previous = null;
	public GameObjectLinkedList ListOwner = null;
	
	public byte bIsPlatform = 0;
	public byte bRender = 1; //Whether or not this should be drawn normally
	public void InitCD(){
		CD.Copy(Info.CD);
		CD.Scale(Info.fDefaultScale);
		CD.Translate(X, Y);
		return;
	}
	public void Copy(GameObject gameObject){
		X = gameObject.X;
		Y = gameObject.Y;
		Info = gameObject.Info;
		
		bFlip = gameObject.bFlip;
		CD.Copy(gameObject.CD);
		return;
	}
	public void Remove(){
		//Just removing it from its list and not putting it another
		// effectively removes the object from the game world.
		ListOwner.Remove(this);
	}
	//Must Override
	void Render(final float CamX, final float CamY, final SpriteBatch mSpriteBatch){
		mSpriteBatch.AddSprite(X - CamX, Y - CamY, Info.nSpriteID, Info.fDefaultScale, false, null);
		return;
	}
}
