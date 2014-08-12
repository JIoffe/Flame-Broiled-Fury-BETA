package com.Foofles;

//This is one horizontal slice of the game grid
//Lists are separated by object type
public class GameObjectGridSlice {
	final public GameObjectLinkedList[] ObjectLists = new GameObjectLinkedList[4];
	final public DynamicObjectPVSList DynamicPVS = new DynamicObjectPVSList();
	GameObjectGridSlice(){
		for(int i = 0; i < 4; i++){
			ObjectLists[i] = new GameObjectLinkedList();
		}
	}
}
