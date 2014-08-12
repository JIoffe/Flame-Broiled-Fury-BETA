package com.Foofles;

public abstract class EnemyProjectileInfo {
	final public static GameObjectInfo HorizontalBullet = new GameObjectInfo();
	public static void Initialize(final TextureAtlas mTextureAtlas){
		HorizontalBullet.Type = 3;
		HorizontalBullet.bIsKillable = 0;
		HorizontalBullet.fDefaultScale = 0.08f;
		HorizontalBullet.bAnimated = false;
		HorizontalBullet.nSpriteID = 24;
		return;
	}
}
