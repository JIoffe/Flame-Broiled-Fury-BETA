package com.Foofles;


import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;


import android.graphics.Bitmap;
import android.opengl.GLSurfaceView;
import android.opengl.GLU;
import android.opengl.GLUtils;
import android.util.Log;

public class FoofRenderer implements GLSurfaceView.Renderer {
	private static final int[] BGTextures = {R.drawable.forestbg, R.drawable.cavebg };
	private static final int[] LevelTextures = {R.drawable.forestlevel, R.drawable.caveslevel};
	public static int frameWidth = 1;
	public static int frameHeight = 1;
	private Boolean bSupportsVBO;
	private boolean bisSoftwareRenderer;
	private boolean bisOpenGL10;
	private boolean bSupportsDrawTexture;
	
	final int[] Textures = new int[3]; // 0 = BG, 1 = Main Sprite Layer, 2 = GUI;
	
	public FoofRenderer(){

	}
	@Override
	public void onDrawFrame(GL10 gl) {
		//We're drawing a Full screen quad every frame
		//so we don't need to explicitly clear the screen
		//gl.glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
		//gl.glClear(GL10.GL_COLOR_BUFFER_BIT);
    	synchronized(DrawQueue.Lock){
    		for(int i = 0; i < DrawQueue.nSpriteBatches; i++){
    			gl.glBindTexture(GL10.GL_TEXTURE_2D, Textures[i]);
	    		final SpriteBatch spriteBatch = DrawQueue.spriteBatches[i];
				gl.glVertexPointer(3, GL10.GL_FLOAT, 0,
				                spriteBatch.vertexBuffer); 
				gl.glTexCoordPointer(2, GL10.GL_FLOAT, 0, spriteBatch.texBuffer);
				gl.glColorPointer(4, GL10.GL_FLOAT, 0, spriteBatch.colorBuffer);
				gl.glDrawElements(GL10.GL_TRIANGLES, spriteBatch.nIndices,
								GL10.GL_UNSIGNED_SHORT, spriteBatch.indexBuffer);
    		}
    	}
    	
    	
	}

	@Override
	public void onSurfaceChanged(GL10 gl, int width, int height) {
		//Created or resized the rendering surface
    	Log.d("RENDERER", "Resized OGL Frame: " + width + "x" + height);
        gl.glViewport(0, 0, width, height);
		gl.glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
		gl.glClear(GL10.GL_COLOR_BUFFER_BIT);
        gl.glMatrixMode(GL10.GL_PROJECTION);
        gl.glLoadIdentity();
        Globals.ratio = (float)width / (float)height;
        Globals.screenspaceWidth = Globals.ratio * 2;
        frameWidth = width;
        frameHeight = height;
        GLU.gluOrtho2D(gl, -Globals.ratio, Globals.ratio, -1.0f, 1.0f);
        
        //Reset ModelView
        gl.glMatrixMode(GL10.GL_MODELVIEW);
        gl.glLoadIdentity();
        
        //Update the full screen BG quad in the sprite batch and the renderer
        synchronized(DrawQueue.Lock){
        	for(int i = 0; i < DrawQueue.nSpriteBatches; i++){
	        	DrawQueue.spriteBatches[i].UpdateBG();
        	}
        }
        Globals.FSQuadVertices[0] = -Globals.ratio;
        Globals.FSQuadVertices[1] = 1.0f;
        Globals.FSQuadVertices[2] = 0.0f;
        Globals.FSQuadVertices[3] = -Globals.ratio;
        Globals.FSQuadVertices[4] = -1.0f;
        Globals.FSQuadVertices[5] = 0.0f;
        Globals.FSQuadVertices[6] = Globals.ratio;
        Globals.FSQuadVertices[7] = -1.0f;
        Globals.FSQuadVertices[8] = 0.0f;
        Globals.FSQuadVertices[9] = Globals.ratio;
        Globals.FSQuadVertices[10] = 1.0f;
        Globals.FSQuadVertices[11] = 0.0f;        

	}

	@Override
	public void onSurfaceCreated(GL10 gl, EGLConfig config) {
		//Render Init
		
        // Start off by setting this for performance over quality
        gl.glHint(GL10.GL_PERSPECTIVE_CORRECTION_HINT, GL10.GL_FASTEST);
        gl.glShadeModel(GL10.GL_FLAT);
        gl.glDisable(GL10.GL_DITHER);
        gl.glDisable(GL10.GL_LIGHTING);  // We definitely don't need lighting
        gl.glDisable(GL10.GL_DEPTH_TEST);  // or Z-Buffer in 2D
        //Do enable blending
        gl.glEnable(GL10.GL_BLEND);
        gl.glBlendFunc(GL10.GL_SRC_ALPHA, GL10.GL_ONE_MINUS_SRC_ALPHA);
        
        //Enable VBuffer client states
        gl.glEnableClientState(GL10.GL_VERTEX_ARRAY);
        gl.glEnableClientState(GL10.GL_TEXTURE_COORD_ARRAY);
        gl.glEnableClientState(GL10.GL_COLOR_ARRAY);
        
        /**
         * Initialize Textures
         */
        gl.glEnable(GL10.GL_TEXTURE_2D);
        gl.glGenTextures(3, Textures, 0); 
        LoadTexturesForTheme(gl, Globals.CurrentTheme);
		SetTexture(gl, 2, R.drawable.gameglobal); 
		/**
    	** Fetch Info about the current GL device
		**/
        final String extensions = gl.glGetString(GL10.GL_EXTENSIONS); 
        Log.v("RENDERINIT", "Supported Extensions: " + extensions);
        final String version = gl.glGetString(GL10.GL_VERSION);
        final String renderer = gl.glGetString(GL10.GL_RENDERER);
        
        bisSoftwareRenderer = renderer.contains("PixelFlinger"); // lol
        bisOpenGL10 = version.contains("1.0");
        bSupportsDrawTexture = extensions.contains("draw_texture");
        //Do not support VBOs in software rendering
        //VBO standard 1.1+ so either flag will be good
        bSupportsVBO = !bisSoftwareRenderer && (!bisOpenGL10 || extensions.contains("vertex_buffer_object"));
        
    	Log.v("RENDERINIT", "OpenGL Device V"+ version +" , "
    			+ renderer  + (bSupportsVBO ? "VBO Is Supported" : "VBO Not Supported") +", "
    			+ (bSupportsDrawTexture ? "Draw_Texture Supported" : "Does not support Draw_Texture"));
	}
	public void LoadTexturesForTheme(final GL10 gl, final int nTheme){
		SetTexture(gl, 0, BGTextures[nTheme]);
		SetTexture(gl, 1, LevelTextures[nTheme]);
		return;
	}
	private void SetTexture(final GL10 gl, final int nIndex, final int nTexID){
        gl.glBindTexture(GL10.GL_TEXTURE_2D, Textures[nIndex]);
        //As usual, make Android and GLU do all the work for us.
		Bitmap bmp = FileIOManager.GetBitmapResource(nTexID);
		if(bmp != null){
			//Use linear filtering for upscaling, and clamp to 0,1.
	        //TODO - decide between point and bilinear
			//gl.glTexParameterf(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_MIN_FILTER, GL10.GL_NEAREST);
			//gl.glTexParameterf(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_MAG_FILTER, GL10.GL_NEAREST);
			gl.glTexParameterf(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_MIN_FILTER, GL10.GL_LINEAR);
			gl.glTexParameterf(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_MAG_FILTER, GL10.GL_LINEAR);
			
			gl.glTexParameterf(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_WRAP_S, GL10.GL_CLAMP_TO_EDGE);
			gl.glTexParameterf(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_WRAP_T, GL10.GL_CLAMP_TO_EDGE);
			
			//GL_MODULATE means we multiply the texture by the diffuse color
			gl.glTexEnvx(GL10.GL_TEXTURE_ENV, GL10.GL_TEXTURE_ENV_MODE, GL10.GL_MODULATE);
			GLUtils.texImage2D(GL10.GL_TEXTURE_2D, 0, bmp, 0);
		}
		return;
	}
}
