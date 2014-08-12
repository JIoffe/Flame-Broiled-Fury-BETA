package com.Foofles;

//Pretty much identical to the other list, but designed for DynamicOBjects
//TODO - Consider merging the two somehow to reduce code size
public class DynamicObjectPVSList {
	public int nSize = 0;
	DynamicObject RootObject = null;
	DynamicObject EndingObject = null;
	
	void Add(final DynamicObject NewObject){
		if(RootObject == null){
			RootObject = NewObject;
			RootObject.PVSPrev = null; //Root is the root!
		}else{
			EndingObject.PVSNext = NewObject;
			NewObject.PVSPrev = EndingObject;
		}
		EndingObject = NewObject;
		EndingObject.PVSNext = null;
		NewObject.PVSOwner = this;
		nSize++;
	}
	
	void Remove(final DynamicObject RemovedObject){
		//TODO - See if we can cut down on ops
		if(RemovedObject.PVSPrev == null){
			if(RemovedObject.PVSNext == null){
				//this is the only object in the list
				RootObject = null;
			}else{
				//This is the first object, but others follow
				RootObject = RemovedObject.PVSNext;
				RemovedObject.PVSNext.PVSPrev = null;
			}
		}else{
			RemovedObject.PVSPrev.PVSNext = RemovedObject.PVSNext;
			if(RemovedObject.PVSNext != null){
				RemovedObject.PVSNext.PVSPrev = RemovedObject.PVSPrev;
			}else{
				//This was the last, make previous last
				EndingObject = RemovedObject.PVSPrev;
			}
		}
		RemovedObject.PVSOwner = null; //TODO - Consider removing this if we're constantly removing/ adding
		nSize--;
	}
	
}
