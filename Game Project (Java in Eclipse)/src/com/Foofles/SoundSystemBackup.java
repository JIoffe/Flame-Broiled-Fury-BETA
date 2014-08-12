package com.Foofles;

import com.Foofles.R.raw;

import android.content.Context;
import android.media.AudioManager;
import android.media.MediaPlayer;
import android.media.SoundPool;

//Static interface for loading, playing, and stopping .mp3 based sounds
// (c) Jason Ioffe

public class SoundSystemBackup {
	final public static int MaxStreams = 16;
	final private static SoundPool mSoundPool = new SoundPool(MaxStreams, AudioManager.STREAM_MUSIC, 100);
	public static float SFXVolume = 1.0f;
	private static MediaPlayer mediaPlayer = null;
	private static Context mAppContext = null;
	public static void Initialize(Context mContext){
		mAppContext = mContext;
		PlayerBullet.GunshotSoundID = mSoundPool.load(mContext, raw.playershota, 1);
		Explosion.SmokeExplosion_SoundID = mSoundPool.load(mContext, raw.smallexplosion, 1);
		PlayerBullet.LaserHit_SoundID = mSoundPool.load(mContext, raw.laserexplosion1, 1);
		Collectible.CollectibleA_SoundID = mSoundPool.load(mContext, raw.pickupa, 1);
		PlayBGM(R.raw.timeforaction, true);
		return;
	}
	public static void PlaySound(final int SoundID){
		mSoundPool.play(SoundID, SFXVolume,SFXVolume, 1, 0, 1f);  
		return;
	}
	
	public static void StopMusic(){
		if(mediaPlayer != null && mediaPlayer.isPlaying()){
			mediaPlayer.stop();
		}
		return;
	}
	
	public static void PauseMusic(){
		if(mediaPlayer != null && mediaPlayer.isPlaying()){
			mediaPlayer.pause();
		}
		return;
	}
	
	public static void ResumeMusic(){
		if(mediaPlayer != null && !mediaPlayer.isPlaying()){
			mediaPlayer.start();
		}
	}
	public static void ChangeBGMVolume(final float BGMVolume){
		if(mediaPlayer != null){
			mediaPlayer.setVolume(BGMVolume, BGMVolume);
		}
		return;
	}
	public static void PlayBGM(final int BGMusicID, final boolean bLoop){
		if(mediaPlayer != null && mediaPlayer.isPlaying()){
			mediaPlayer.stop();
		}
		mediaPlayer = MediaPlayer.create(mAppContext, BGMusicID);
		mediaPlayer.start(); // no need to call 
		mediaPlayer.setLooping(bLoop);
		return;
	}
}
