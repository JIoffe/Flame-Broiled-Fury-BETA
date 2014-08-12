package com.Foofles;

/**
 * PlayerProgress will handle maintaining the player's progress in terms of
*levels, tokens, bunny rescues, etc.
*  Access is purely static. Upon collection of tokens 
*/
public class PlayerProgress {
	private static int nTokensHeld = 0;
	private static int nBunniesRescued = 0;
	/*
	 *  Log the token and displays the current token tally
	 */
	static public void CollectToken(){
		//TODO - RENABLE TOASTS
		//CustomToastSystem.ShowTally(R.drawable.bunnymoney, ++nTokensHeld);
	}
	static public void RescueBunny(){
		//CustomToastSystem.ShowTally(R.drawable.bunnyface, ++nBunniesRescued);
	}
}
