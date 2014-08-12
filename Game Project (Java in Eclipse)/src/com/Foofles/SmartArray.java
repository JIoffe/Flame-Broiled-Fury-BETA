package com.Foofles;

import java.lang.reflect.Array;

public class SmartArray<T extends Copyable & UpdateRender> {
	//For lack of a better name...
	//This just holds a fixed Maximum of GameObjects with the ability
	//To quickly add kor remove
	public int nMaximumSize = 0;
	public int nSize = 0;
	public int nIndexLast = -1;
	
	public T[] mGameObjects = null;
	
	@SuppressWarnings("unchecked")
	public SmartArray(final int nMax, Class<T> C) throws IllegalAccessException, InstantiationException{
		nMaximumSize = nMax;
		
		mGameObjects = (T[]) Array.newInstance(C, nMax);
		for(int i = 0; i < nMax; i++){
			mGameObjects[i] = C.newInstance();
		}
	}
	public void Remove(final int nIndex){
		if(nIndex != nIndexLast){
			mGameObjects[nIndex].Copy(mGameObjects[nIndexLast]);
		}
		nSize--;
		nIndexLast--;
	}
	
	public T Add(){
		if(nSize < nMaximumSize){
			nSize++;
			return mGameObjects[++nIndexLast];
		}
		return null;
	}
	
	public void UpdateAndDraw(final SpriteBatch spriteBatch){
		for(int i = 0; i < nSize; i++){
			final T dyn = mGameObjects[i];
			if(!dyn.UpdateAndDraw(spriteBatch)){
				Remove(i);
				i--;
			}
		}
	}
}
