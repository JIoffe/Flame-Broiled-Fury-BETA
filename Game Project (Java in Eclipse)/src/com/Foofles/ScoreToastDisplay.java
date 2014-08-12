package com.Foofles;

public class ScoreToastDisplay {
	final public int[] Digits = new int[3]; //For the sake of making things easier
											//We will keep the score toasts within
											//a small number... like 3!
	public ScoreToastDisplay(int Amount){
		Digits[0] = GameGUI.ZeroStartIndex + (int)((Amount) % 10);
		for(int i = 1; i < 3; i++ ){
			Digits[i] = GameGUI.ZeroStartIndex 
				+ (int)((Amount / FoofMath.ExtractTable[i]) % 10);
		}
	}
}
