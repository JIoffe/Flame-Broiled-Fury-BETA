package com.Foofles;

public class GameObjectLinkedList {
	public int nSize = 0;
	GameObject RootObject = null;
	GameObject EndingObject = null;
	
	void Add(final GameObject NewObject){
		if(RootObject == null){
			RootObject = NewObject;
			RootObject.Previous = null; //Root is the root!
		}else{
			EndingObject.Next = NewObject;
			NewObject.Previous = EndingObject;
		}
		EndingObject = NewObject;
		EndingObject.Next = null;
		NewObject.ListOwner = this;
		nSize++;
	}
	
	void Remove(final GameObject RemovedObject){
		//TODO - See if we can cut down on ops
		if(RemovedObject.Previous == null){
			if(RemovedObject.Next == null){
				//this is the only object in the list
				RootObject = null;
			}else{
				//This is the first object, but others follow
				RootObject = RemovedObject.Next;
				RemovedObject.Next.Previous = null;
			}
		}else{
			RemovedObject.Previous.Next = RemovedObject.Next;
			if(RemovedObject.Next != null){
				RemovedObject.Next.Previous = RemovedObject.Previous;
			}else{
				//This was the last, make previous last
				EndingObject = RemovedObject.Previous;
			}
		}
		RemovedObject.ListOwner = null; //TODO - Consider removing this if we're constantly removing/ adding
		nSize--;
	}
	
}
