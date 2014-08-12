package com.Foofles;

import android.app.ListActivity;
import android.content.Intent;
import android.content.res.Configuration;
import android.os.Bundle;
import android.view.View;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.ArrayAdapter;
import android.widget.ListView;

public class FoofWorldMapActivity extends ListActivity {
	private ListView LevelList = null;
	public static FoofWorldMapActivity mActivity = null; 
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mActivity = this;
        setListAdapter(ArrayAdapter.createFromResource(this,
                R.array.LevelNames, R.layout.list_view));
        getListView().setOnItemClickListener(new OnItemClickListener() {
        	 
            @Override
            public void onItemClick(AdapterView<?> parent, View view,
                    int position, long id) {
            	FoofGameActivity.LevelToPlay = position;
            	Intent GameIntent = new Intent(mActivity, com.Foofles.FoofGameActivity.class);
                startActivityForResult(GameIntent, 0);
            	return;
            }

        });
    }
    @Override
    protected void onPause(){
    	super.onPause();
    }
    @Override
    protected void onResume(){
    	super.onResume();
    }
    @Override
    protected void onDestroy(){
    	super.onDestroy();
    }
    @Override
    public void onConfigurationChanged(Configuration newConfig) {
      //Make sure we ignore configuration changes
      //eg. Flipping the phone over. ALWAYS be in landscape mode.
      super.onConfigurationChanged(newConfig);
    }
    @Override 
    protected void onActivityResult (int requestCode, int resultCode, Intent data){
    	//ResultCode == CURRENT LEVEL
    	return;
    }
}
