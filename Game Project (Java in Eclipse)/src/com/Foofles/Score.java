package com.Foofles;


public class Score {
	final static public int MaximumScore = 99999;
	static public int ScoreAmt = 0;
	static public float DisplayScore = 0; // LAGS BEHIND TRUE SCORE.
	//Avoid dividing by 10 constantly to get the score.
	final static public int MaxScoreDigits = 5;
	final static public int ScoreNumberSprites[] = new int[MaxScoreDigits];
	final static float ScoreTallySpeed = 63.2f; // in points per second
	final static float ScoreNumberOffset = 0.12f;
	final static float ScoreNumberScale = 0.07f;
	static private int ScoreDigitCounter = 10;
	static public int nScoreDigits = 1;
	
	public static float ScoreAlpha = 1.0f;
	Score(){

	}
	public static void Initialize(){
		for(int i = 0; i < 5; i++){
			ScoreNumberSprites[i] = GameGUI.ZeroStartIndex;
		}
		ScoreAmt = 0;
		DisplayScore = 0;
		nScoreDigits = 1;
		ScoreDigitCounter = 10;
	}
	public static void Update(){
		if(DisplayScore != ScoreAmt){
			ScoreAlpha = 3.0f;
			//Make it pick up faster if we're really far behind
			final float ScoreDifference = ScoreAmt - DisplayScore;
			
			if(ScoreDifference > 200){
				DisplayScore += ScoreTallySpeed * (ScoreDifference/50.0f) * Globals.secondsSinceLastFrame;
			}else{
				DisplayScore += ScoreTallySpeed * Globals.secondsSinceLastFrame;
			}
			if(DisplayScore > ScoreAmt){
				DisplayScore = ScoreAmt;
			}
			if(DisplayScore >= ScoreDigitCounter){
				nScoreDigits++;
				ScoreDigitCounter *= 10;
			}
			ScoreNumberSprites[0] = GameGUI.ZeroStartIndex + (int)((DisplayScore) % 10);
			for(int i = 1; i < Score.nScoreDigits; i++ ){
				ScoreNumberSprites[i] = GameGUI.ZeroStartIndex 
					+ (int)((DisplayScore / FoofMath.ExtractTable[i]) % 10);
			}

		}else{
			//fade score
			if(ScoreAlpha > 0.0f){
				ScoreAlpha -= GameGUI.GUIFadeSpeed * Globals.secondsSinceLastFrame;
			}
		}
	} //End - Update()
	
	static public void AddPoints(int points){
		//Self explanitory!
		ScoreAmt += points;
		if(ScoreAmt > MaximumScore){
			ScoreAmt = MaximumScore;
		}
		return;
	}
}
