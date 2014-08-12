package com.Foofles;



import android.app.Activity;
import android.app.Dialog;
import android.content.DialogInterface;
import android.content.DialogInterface.OnDismissListener;
import android.content.Intent;
import android.content.res.Configuration;
import android.content.res.Resources;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.media.AudioManager;
import android.os.SystemClock;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.SeekBar;
import android.widget.TextView;

/**
 * 
 * This represents the UI thread and entry / bootstrapping
 * point of the game/application
 * @author Jason Ioffe
 *
 */
public class FoofGameActivity extends Activity implements OnDismissListener{
	static final int DIALOG_PAUSE = 1;
	static final int DIALOG_OPTIONS = 2;
	static final int DIALOG_LEVELCOMPLETE = 3;
	
	private GLSurfaceView mGLSurface;
	final private FoofRenderer GameRenderer = new FoofRenderer();
	final private FoofGame Game = new FoofGame();
	
	//Input pointer IDs
	final private int BAD_ID = -1;
	private int DPadPointerID = BAD_ID;
	
    private long LastMotionEventTime;
	final private long msTouchEventDelay = 2;
	//TODO - Find less ghetto way of doing this
	public static int LevelToPlay = -1;
	public final Runnable ShowLevelCompletion = new Runnable() {
	    public void run() {
	    	showDialog(DIALOG_LEVELCOMPLETE);
	    	return;
	    }
	};
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Globals.GameActivity = this;
        CustomToastSystem.Initialize(this);
        //Init sounds
        setVolumeControlStream(AudioManager.STREAM_MUSIC); //Use the "Media Volume" as master volume control
        SoundSystem.Initialize(this);
        //Get our File IO Manager to init its resources
        FileIOManager.InitResources(this);
        //Preallocate our drawQueue
        synchronized(DrawQueue.Lock){
        	DrawQueue.Initialize();
        }
        Game.LoadGameBasics();
        Game.LoadLevel(LevelToPlay);
 
        //INIT RENDERER
        mGLSurface = new GLSurfaceView(this);
        //NO DEPTH BUFFER OR STENCIL. Handle Z Order ourselves.
        mGLSurface.setEGLConfigChooser(false);
       // mGLSurface.setEGLConfigChooser(8, 8, 8, 8, 0, 0); // R8G8B8A8 - NO DEPTH or Stencil
        mGLSurface.setRenderer(GameRenderer);
        
        Game.Start();
        
        setContentView(mGLSurface);
    }
    @Override
    protected void onPause(){
    	super.onPause();
    	//showDialog(DIALOG_PAUSE);
    	if(mGLSurface != null){
    		mGLSurface.onPause();
    	}
    	Game.Pause();
    	//System.gc();
    }
    @Override
    protected void onResume(){
    	super.onResume();
    	if(mGLSurface != null){
    		mGLSurface.onResume();
    	}
    	Game.Resume();
    }
    @Override
    protected void onDestroy(){
    	super.onDestroy();
    	Game.End();
    }
    @Override
    public void onConfigurationChanged(Configuration newConfig) {
      //Make sure we ignore configuration changes
      //eg. Flipping the phone over. ALWAYS be in landscape mode.
      super.onConfigurationChanged(newConfig);
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
    	//TODO - MAKE THIS FASTER
    	final int Action = event.getAction();
    	//We need to & with ACTION_MASK to enable multitouch
    	switch(Action & MotionEvent.ACTION_MASK){ 
    		case MotionEvent.ACTION_DOWN:{
    			ProcessUserClick(event.getX(), event.getPointerId(0));
    			break;
    		}
    		case MotionEvent.ACTION_POINTER_DOWN:{
    			final int Index = (Action & MotionEvent.ACTION_POINTER_INDEX_MASK) 
									>> MotionEvent.ACTION_POINTER_INDEX_SHIFT;
									//Player.YVelocity = Player.JumpVelocity;
				ProcessUserClick(event.getX(Index), event.getPointerId(Index));
				break;
    		}
    		case MotionEvent.ACTION_MOVE:{
    			final int Index = (Action & MotionEvent.ACTION_POINTER_INDEX_MASK) 
									>> MotionEvent.ACTION_POINTER_INDEX_SHIFT;
				
				final int PointerID = event.getPointerId(Index);
				final float WorldX = FoofMath.Lerp(-Globals.ratio, Globals.ratio, (event.getX(Index) / FoofRenderer.frameWidth));

				if(WorldX < 0.0f){
				//Left half of the screen
					if(WorldX < GameGUI.DPadX){
						GameInputState.DPadLeft = true;
						GameInputState.DPadRight = false;
					}else{
						GameInputState.DPadLeft = false;
						GameInputState.DPadRight = true;
					}
					DPadPointerID = PointerID;
				}
				
				//Sleep the thread so we don't O D on movement events.
        		final long Time = SystemClock.uptimeMillis();
    	    	if(Time - LastMotionEventTime < msTouchEventDelay){
    		        try {
    		            Thread.sleep(msTouchEventDelay);
    		        } catch (InterruptedException e) {
    		            //Interruption is fine, we're just killing time
    		        }
    	    	}
    	    	LastMotionEventTime = Time;
    	    	
				break;
    		}
    		case MotionEvent.ACTION_UP:{
    			//All fingers left the screen - Just set everything to false.
    			GameInputState.DPadLeft = false;
    			GameInputState.DPadRight = false;
    			GameInputState.TapFire = false;
    			break;
    		}
    		case MotionEvent.ACTION_POINTER_UP:{
    			//Same as Action_Up but while we still have other fingers on the screen.
    			final int Index = (Action & MotionEvent.ACTION_POINTER_INDEX_MASK) 
				>> MotionEvent.ACTION_POINTER_INDEX_SHIFT;
				ProcessUserRelease(event.getPointerId(Index));
    			break;
    		}
    	    case MotionEvent.ACTION_CANCEL: {
    	    	break;
    	    }
    		default:
    			return false;
    	}
    	return true;
    }
    private void ProcessUserClick(final float X, final int PointerID){
   		final float WorldX = FoofMath.Lerp(-Globals.ratio, Globals.ratio, (X / FoofRenderer.frameWidth));
   		//final float WorldY = -FoofMath.Lerp(-1.0f, 1.0f, (Y / FoofRenderer.frameHeight));
   		if(WorldX < 0.0f){
   			//Left half of the screen
   			if(WorldX < GameGUI.DPadX){
   				GameInputState.DPadLeft = true;
   				GameInputState.DPadRight = false;
   			}else{
   				GameInputState.DPadLeft = false;
   				GameInputState.DPadRight = true;
   			}
   			DPadPointerID = PointerID;
   		}else{
   			//We're on the right half of the screen, jump or fire
   			if(WorldX < GameGUI.FireButtonEdge){
   			if(Player.JumpCount < 2){
   				Player.YVelocity = Player.JumpVelocity;
   				Player.JumpCount++;
   				Globals.mGameGraph.mPlayer.mMountedPlatform = null;
   			}
   			}else if(WorldX < Globals.ratio){
   				GameInputState.TapFire = true;
   			}else{
   				//We're touching the menu buttons, pause it
   				showDialog(DIALOG_PAUSE);
   			}
   		}
    	return;
    }
    private void ProcessUserRelease(final int PointerID){
    	if(PointerID == DPadPointerID){
    		GameInputState.DPadLeft = false;
    		GameInputState.DPadRight = false;
    	}
    	return;
    }
    protected Dialog onCreateDialog(int id) {
    	Dialog dialog;
    	switch(id){
    		case DIALOG_PAUSE:{
    			dialog = new Dialog(this);
    			dialog.setContentView(R.layout.pausedialog);
    			final Button options = (Button)dialog.findViewById(R.id.options);
    	        final Button resume = (Button)dialog.findViewById(R.id.resume);
    	        final Button quit = (Button)dialog.findViewById(R.id.quitmenu);
    	        final Button restart = (Button)dialog.findViewById(R.id.restart);
    	        
    	        //Get list of level names from /res/array/
    	        //This causes an allocation each time we create 
    	        //the pause panel, but it's not a big deal vs.
    	        //storing every single level name
    	        //in memory all the time.
    	        final Resources Res = getResources();
    	        final String[] LevelNames = Res.getStringArray(R.array.LevelNames);
    	        final TextView LevelLabel = (TextView)dialog.findViewById(R.id.levelname);
    	        LevelLabel.setText(LevelNames[Game.GetCurrentLevel()]);
    	        resume.setOnClickListener(new View.OnClickListener() {
    	            public void onClick(View v) {
    	                dismissDialog(DIALOG_PAUSE);
    	                Game.Resume();
    	            }
    	        });
    	         
    	        quit.setOnClickListener(new View.OnClickListener() {
    	            public void onClick(View v) {
    	            	dismissDialog(DIALOG_PAUSE);
    	            	ExitToWorldMap();
    	            }
    	        });
    	        
    	        restart.setOnClickListener(new View.OnClickListener() {
    	            public void onClick(View v) {
    	                Globals.mGame.RestartLevel();
    	                dismissDialog(DIALOG_PAUSE);
    	                Game.Resume();
    	            }
    	        });
    	        options.setOnClickListener(new View.OnClickListener() {
    	            public void onClick(View v) {
    	                showDialog(DIALOG_OPTIONS);
    	            	dismissDialog(DIALOG_PAUSE);
    	            }
    	        });
    			dialog.setTitle("Paused");
    			break;
    		}
    		case DIALOG_OPTIONS:{
    			dialog = new Dialog(this);
    			dialog.setContentView(R.layout.optionsdialog);
    	        final Button resume = (Button)dialog.findViewById(R.id.resume);
    	        final SeekBar bgmvolume = (SeekBar)dialog.findViewById(R.id.bgmvolume);
    	        final SeekBar sfxvolume = (SeekBar)dialog.findViewById(R.id.sfxvolume);
    	        //sfxvolume.setProgress((int)(SoundSystem.SFXVolume * 100.0f));
    	        bgmvolume.setProgress(100);
    	        resume.setOnClickListener(new View.OnClickListener() {
    	            public void onClick(View v) {
    	                dismissDialog(DIALOG_OPTIONS);
    	                Game.Resume();
    	            }
    	        });
    	        bgmvolume.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
    	        	@Override
    	        	public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser){
    	        		SoundSystem.ChangeBGMVolume((float)seekBar.getProgress() / 100.0f);
    	        	}

					@Override
					public void onStartTrackingTouch(SeekBar seekBar) {
						return;
						
					}

					@Override
					public void onStopTrackingTouch(SeekBar seekBar) {
						return;	
					}
    	        });
    	        sfxvolume.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
    	        	@Override
    	        	public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser){
    	        		//SoundSystem.SFXVolume = (float)seekBar.getProgress() / 100.0f;
    	        	}

					@Override
					public void onStartTrackingTouch(SeekBar seekBar) {
						return;
						
					}

					@Override
					public void onStopTrackingTouch(SeekBar seekBar) {
						return;	
					}
    	        });
    	        dialog.setTitle("Options");
    	        break;
    		}
    		case DIALOG_LEVELCOMPLETE:{
    			//Shown when player reaches level goal
    			dialog = new Dialog(this);
    			dialog.setContentView(R.layout.level_exit);
    			
    			final GameGraph mGameGraph = Globals.mGameGraph;
    			//TODO - if(mGG == null...)
    			//TODO - Actually make this function
    	        final Resources Res = getResources();
    	        final String[] LevelNames = Res.getStringArray(R.array.LevelNames);
    	        SetDlgItemText(dialog, R.id.lvlname, LevelNames[Game.GetCurrentLevel()]);
    	        SetDlgItemText(dialog, R.id.secretsfound, 3 + " out of " + mGameGraph.nSecrets);
    	        SetDlgItemText(dialog, R.id.time, mGameGraph.mPlayer.GetLevelTime());
    	        final Button resume = (Button)dialog.findViewById(R.id.resume);  			
    	        resume.setOnClickListener(new View.OnClickListener() {
    	            public void onClick(View v) {
    	            	dismissDialog(DIALOG_LEVELCOMPLETE);
    	            	ExitToWorldMap();
    	            }
    	        });
    			break;
    		}
    		default:{
    			dialog = null;
    			break;
    		}
    	}
    	return dialog;
    }
    protected void onPrepareDialog (int id, Dialog dialog, Bundle args){
  		Game.Pause();
		dialog.setOnDismissListener(this);
    	return;
    }
	@Override
	public boolean onKeyDown (int keyCode, KeyEvent event){
		switch(keyCode){
			case KeyEvent.KEYCODE_MENU:{
				showDialog(DIALOG_PAUSE);
				break;
			}
			default:
				return false;
		}
		return true;
	}
	@Override
	public void onDismiss(DialogInterface dialog) {
		//Game.Resume();
		return;
	}

	public void ExitToWorldMap(){
        Intent intent = new Intent();
        setResult(Game.GetCurrentLevel(), intent);
        Game.End();
        finish();
		return;
	}
	private void SetDlgItemText(final Dialog dialog, final int ID, final String text){
		//TV! Yay!
		final TextView tv = (TextView) dialog.findViewById(ID);
		if(tv != null){
			tv.setText(text);
		}
	}
}