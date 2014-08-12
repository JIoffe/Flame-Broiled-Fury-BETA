package com.Foofles;

/**
 * This class wraps scrolling background functionality.
 * 
 * 
 * @author Jason Ioffe
 *
 */
public class GameBackground {
	public int nLayers = 0;
	public ParallaxLayer[] BGLayers = null;
	public boolean bDrawLayeredBG = true;
	
	public void UpdateLayersForTheme(final int iTheme){
		switch(iTheme){
			case FoofGame.FOREST_THEME:{
				nLayers = 2;
				BGLayers = new ParallaxLayer[nLayers];
				BGLayers[0] = new ParallaxLayer(2,3,4);
				BGLayers[0].SetLayerSpacing(0.5f, 0.0f, 0.0f, -0.22f);
				BGLayers[1] = new ParallaxLayer(0,1);
				BGLayers[1].SetLayerSpacing(1.0f, 0.4f, -0.15f, -0.32f);
				break;
			}
			case FoofGame.CAVE_THEME:{
				nLayers = 2;
				BGLayers = new ParallaxLayer[nLayers];
				BGLayers[0] = new ParallaxLayer(1,2);
				BGLayers[0].SetLayerSpacing(1.5f, 0.0f, 0.0f, -0.22f);
				BGLayers[1] = new ParallaxLayer(3,4);
				BGLayers[1].SetLayerSpacing(1.0f, 0.4f, -0.15f, -0.32f);
				break;
			}
			default:
				break;
		}
		return;
	}
}
