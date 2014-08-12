package com.Foofles;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.nio.ShortBuffer;

import android.util.Log;


public class SpriteBatch {
	public FloatBuffer vertexBuffer;
	public ShortBuffer indexBuffer;
	public FloatBuffer texBuffer;
	public FloatBuffer colorBuffer;
	
	public final TextureAtlas mTextureAtlas = new TextureAtlas();
	//private float vertexArray[];
	//private float texArray[];
	
	public int nIndices;
	public final static short MAX_SPRITES_PER_BATCH = 145;
	private final static int VERTEX_STRIDE = 48;
	private final static int INDEX_STRIDE = 12;
	private final static int TEXTURE_STRIDE = 32;
	private final static int COLOR_STRIDE = 64;
	
	private final static float[] PureWhite = {1.0f, 1.0f, 1.0f, 1.0f,
												1.0f, 1.0f, 1.0f, 1.0f,
												1.0f, 1.0f, 1.0f, 1.0f,
												1.0f, 1.0f, 1.0f, 1.0f};
	
	//Pre - allocate an array to use to push data for each sprite
	private final static float[] vertices = new float[12];
	public SpriteBatch(){
		//Texture Atlas is a class that keeps all our image data in one big chunk
		//mTextureAtlas = new TextureAtlas();
		nIndices = 0;

		final ByteBuffer vbb = ByteBuffer.allocateDirect(VERTEX_STRIDE * SpriteBatch.MAX_SPRITES_PER_BATCH);
		final ByteBuffer ibb = ByteBuffer.allocateDirect(INDEX_STRIDE * SpriteBatch.MAX_SPRITES_PER_BATCH);
		final ByteBuffer tbb = ByteBuffer.allocateDirect(TEXTURE_STRIDE * SpriteBatch.MAX_SPRITES_PER_BATCH);
		final ByteBuffer cbb = ByteBuffer.allocateDirect(COLOR_STRIDE * SpriteBatch.MAX_SPRITES_PER_BATCH);
		
		vbb.order(ByteOrder.nativeOrder());
		tbb.order(ByteOrder.nativeOrder());
		ibb.order(ByteOrder.nativeOrder());
		cbb.order(ByteOrder.nativeOrder());

		vertexBuffer = vbb.asFloatBuffer();
		texBuffer = tbb.asFloatBuffer();
		indexBuffer = ibb.asShortBuffer();
		colorBuffer = cbb.asFloatBuffer();
		
		indexBuffer.position(0); 
		//TODO - Think of ways to make this faster to decrease load times
		//Initialize more as game runs?
		//Read precomputed indices from file?
		for(short i = 0; i < SpriteBatch.MAX_SPRITES_PER_BATCH; i++){
			final short SpritesTimesFour = (short)(i * 4); 
			final short i2 = (short) (SpritesTimesFour + 2);
			final short[] indices = { SpritesTimesFour, (short) (SpritesTimesFour + 1), i2, 
					SpritesTimesFour, i2, (short) (SpritesTimesFour + 3) };  
			
			indexBuffer.put(indices);
		}
		indexBuffer.position(0); //AND KEEP IT THERE
		colorBuffer.position(0);
		
		//TODO - Consider removing this or removing color altogether...
		//Initialize the color buffer to be 1,1,1,1 for simplicity during runtime
		final int nColorEntries = 16 * SpriteBatch.MAX_SPRITES_PER_BATCH;
		for(int i = 0; i < nColorEntries; i++){
			colorBuffer.put(1.0f);
		}
		colorBuffer.position(0);
		vertexBuffer.position(0);
	}
	void AddFullScreenSprite(final int TexID, final float[] Color){
		vertexBuffer.put(Globals.FSQuadVertices);
		
		colorBuffer.put(Color);
		//final float[] Tex = { 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f };
		
		texBuffer.put(mTextureAtlas.GetUVsByIndex(TexID, false));
		nIndices += 6;
		return;
	}
	//TODO - We only need one "addsprite" function
	void AddRotatingSprite(final float X, final float Y, final int TexID, final float fScale, final float CosRot, final float SinRot, final boolean bFlip){
		vertices[0] = -fScale;
		vertices[1] = fScale;
		vertices[3] = -fScale;
		vertices[4] = -fScale;
		vertices[6] = fScale;
		vertices[7] = -fScale;
		vertices[9] = fScale;
		vertices[10] = fScale;
		
		for(int v = 0; v < 4; v++){
			final int Vtimes3 = v*3;
			final float xX = vertices[Vtimes3];
			final float yY = vertices[Vtimes3+1];
			vertices[Vtimes3] = (xX*(float)CosRot)
							+	(yY*(float)-SinRot) + X;
			vertices[Vtimes3+1] = (xX*(float)SinRot)
							+	(yY*(float)CosRot) + Y;
		}
		vertexBuffer.put(vertices);
		colorBuffer.put(PureWhite);
		//final float[] Tex = { 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f };
		
		texBuffer.put(mTextureAtlas.GetUVsByIndex(TexID, bFlip));
		nIndices += 6;
		return;
	}
	void AddSprite(final float X, final float Y, final int TexID, final float fScale, final boolean bFlip, final float[] Color){
		//Mainly for the level tiles.
		//vertexBuffer.position(index*12);

		final float Up = fScale + Y;
		final float Down = Y - fScale;
		final float Left = X - fScale;
		final float Right = X + fScale;
		vertices[0] = Left;
		vertices[1] = Up;
		vertices[3] = Left;
		vertices[4] = Down;
		vertices[6] = Right;
		vertices[7] = Down;
		vertices[9] = Right;
		vertices[10] = Up;

		vertexBuffer.put(vertices);
	
		if(Color != null)
			colorBuffer.put(Color);
		else
			colorBuffer.put(PureWhite);
		//final float[] Tex = { 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f };
		
		texBuffer.put(mTextureAtlas.GetUVsByIndex(TexID, bFlip));
		nIndices += 6;
		return;
	}
	void AddWigglingSprite(final float X, final float Y, final int TexID, final float fScale, final boolean bFlip, final float[] Color){
		//Mainly for the level tiles.
		//vertexBuffer.position(index*12);
		final float WiggleFactor = (float)Math.sin(30.0f * Globals.runtimeSeconds) * 0.05f;
		final float Up = fScale + Y;
		final float Down = Y - fScale;
		final float Left = X - fScale;
		final float Right = X + fScale;
		vertices[0] = Left + WiggleFactor;
		vertices[1] = Up;
		vertices[3] = Left - WiggleFactor;
		vertices[4] = Down;
		vertices[6] = Right - WiggleFactor;
		vertices[7] = Down;
		vertices[9] = Right + WiggleFactor;
		vertices[10] = Up;

		vertexBuffer.put(vertices);
	
		if(Color != null)
			colorBuffer.put(Color);
		else
			colorBuffer.put(PureWhite);
		//final float[] Tex = { 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f };
		
		texBuffer.put(mTextureAtlas.GetUVsByIndex(TexID, bFlip));
		nIndices += 6;
		return;
	}
	public void UpdateBG(){
		//When the view is changed the FSQ of the BG needs to be changed.
		vertexBuffer.position(0);
		final float bgVertices[] = {
			      -Globals.ratio,  1.0f, 0.0f,  
			      -Globals.ratio, -1.0f, 0.0f,  
			      Globals.ratio, -1.0f, 0.0f,  
			      Globals.ratio,  1.0f, 0.0f,  
			};
		
		vertexBuffer.put(bgVertices);
		vertexBuffer.position(0);
		if(nIndices == 0){
			nIndices = 6; // If there's nothing on the screen 
						//at least insure that we're drawing the BG
		}
	}
	public void ChangeBGImage(int nImageIndex){
		texBuffer.position(0);
		texBuffer.put(mTextureAtlas.GetUVsByIndex(nImageIndex, false));
		texBuffer.position(0);
		return;
	}
	void ResetBufferPositions(){
		texBuffer.position(0);
		vertexBuffer.position(0);
		colorBuffer.position(0);
		return;
	}
	void Reset(){
		ResetBufferPositions();
		nIndices = 0;
	}
}
