package com.Foofles;

import android.app.Activity;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

//This feels a bit sloppy but if it works...

public class CustomToastSystem {
	private static TextView text = null;
	private static Toast toast = null;
	
	private static Toast TallyToast = null;
	private static TextView TallyText = null;
	private static ImageView TallyImage = null;
	
	public static void Initialize(final Activity GameActivity){
		final LayoutInflater Inflater = GameActivity.getLayoutInflater();
		View layout = Inflater.inflate(R.layout.info_toast,
		                               (ViewGroup) GameActivity.findViewById(R.id.info_toast_root));

		text = (TextView) layout.findViewById(R.id.text);

		toast = new Toast(GameActivity.getApplicationContext());
		toast.setGravity(Gravity.BOTTOM, 0, 0);
		toast.setView(layout);
		
		// Setup the toasts for tally counts
		layout = Inflater.inflate(R.layout.tally_toast, (ViewGroup) GameActivity.findViewById(R.id.info_tally_root));
		TallyText = (TextView) layout.findViewById(R.id.text);
		TallyImage = (ImageView) layout.findViewById(R.id.image);
		TallyToast = new Toast(GameActivity.getApplicationContext());
		TallyToast.setGravity(Gravity.TOP, 0, 0);
		TallyToast.setView(layout);
	}
	public static void ShowToast(final String ToastText){
		text.setText(ToastText);
		toast.setDuration(Toast.LENGTH_LONG);
		Globals.GameActivity.runOnUiThread(RunToast);
		return;
	}
	public static void ShowTally(final int ImgID, final int nAmount){
		TallyText.setText("X " + nAmount);
		TallyImage.setImageResource(ImgID);
		TallyToast.setDuration(Toast.LENGTH_LONG);
		Globals.GameActivity.runOnUiThread(RunTally);
		return;
	}
	public static final Runnable RunToast = new Runnable() {
	    public void run() {
	    	toast.show();
	    	return;
	    }
	};
	public static final Runnable RunTally = new Runnable() {
	    public void run() {
	    	TallyToast.show();
	    	return;
	    }
	};
}
