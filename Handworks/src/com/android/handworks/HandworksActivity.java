package com.android.handworks;


import java.util.Timer;

import android.app.Activity;
import android.content.Context;
import android.hardware.Camera;
import android.hardware.Camera.CameraInfo;
import android.os.Bundle;
import android.os.PowerManager;
import android.util.Log;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.Toast;


public class HandworksActivity extends Activity {
	
	
	private Camera mCamera;
	private CameraPreview mPreview;
	private DrawSurfaceView mDrawSurfaceView;
	PowerManager powerManager = null;
	PowerManager.WakeLock wakeLock = null;
	
	int mNumberOfCameras;
	int mCameraCurrentlyLocked;
	
	// The first rear facing camera
    int mDefaultCameraId;
    int mScreenWidth, mScreenHeight;
    //private boolean flagScan = false;
    
    private static final String LOG_TAG = "HandworksActivity";
    
    
    @Override
    public void onCreate(Bundle savedInstanceState) {
    	Log.d(LOG_TAG, "onCreate");
        super.onCreate(savedInstanceState);
        
        // 无标题栏状态栏
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
        
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON,
        				WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        
        powerManager = (PowerManager)getSystemService(Context.POWER_SERVICE);
        wakeLock = powerManager.newWakeLock(PowerManager.SCREEN_DIM_WAKE_LOCK, "MyTag");
     
        // 设置布局
        setContentView(R.layout.handworks_activity);
     
        // Create our Preview view and set it as the content of our activity.
        mPreview = new CameraPreview(this);
        mDrawSurfaceView = new DrawSurfaceView(this);
        
        FrameLayout preview = (FrameLayout) findViewById(R.id.camera_preview);
        
        // 将相机预览图加入帧布局里面
        preview.addView(mPreview, 0);
        preview.addView(mDrawSurfaceView, 0);
     
        Button StartButton = (Button) findViewById(R.id.button_start);
        StartButton.setOnClickListener(new View.OnClickListener() {
    	 
        	@Override
        	public void onClick(View v) {
        		try {
        			Log.i(LOG_TAG, "StartButton"+mPreview.isPreview+mPreview.isScan);
        			if((mCamera != null) && mPreview.isPreview && (!mPreview.isScan)) {
        				if (mPreview.timer == null) {
        					mPreview.timer = new Timer();
                			mPreview.isScan = true;
                			mPreview.timer.scheduleAtFixedRate(mPreview.new ScanThread(), 0, 1000);
                		}
        				if (mDrawSurfaceView.timer == null) {
        					mDrawSurfaceView.timer = new Timer();
        					mDrawSurfaceView.isDrawScan = true;
        					mDrawSurfaceView.timer.scheduleAtFixedRate(
        					mDrawSurfaceView.new DrawSurfaceScan(), 0, 1000);
        				}
        			}
        		}
        		catch (Exception e) {
        			e.printStackTrace();
        		}
			
        	}
        });
     
        Button StopButton = (Button) findViewById(R.id.button_stop);
        StopButton.setOnClickListener(new View.OnClickListener() {
    	 
        	@Override
        	public void onClick(View v) {
        		try {
        			Log.i(LOG_TAG, "StopButton"+mPreview.isPreview+mPreview.isScan);
        			if((mCamera != null) && mPreview.isPreview && mPreview.isScan) {
        				//mCamera.setOneShotPreviewCallback(null);
        				//if (mPreview.isScan) {
                			mCamera.setOneShotPreviewCallback(null);
                			mPreview.isScan = false;
                			mPreview.timer.cancel();
                			mPreview.timer = null;
                			mDrawSurfaceView.isDrawScan = false;
                			mDrawSurfaceView.timer.cancel();
                			mDrawSurfaceView.timer = null;
                		//}
        			}
        		}
        		catch (Exception e) {
        			e.printStackTrace();
        		}
			
        	}
        });
     
        // 得到默认的相机ID
        mDefaultCameraId = getDefaultCameraId();
        //mDefaultCameraId = 0;
        mCameraCurrentlyLocked = mDefaultCameraId;
        mDrawSurfaceView.cameraid = mDefaultCameraId;
    }
    
    
    @Override
    protected void onResume() {
    	Log.d(LOG_TAG, "onResume");
    	super.onResume();
    	
    	wakeLock.acquire();
    	
    	mCamera = getCameraInstance(mCameraCurrentlyLocked);
    	mPreview.setCamera(mCamera);
    	/*
    	try {
    		if (null != mCamera) {
    			mCamera.setPreviewDisplay(mPreview.mHolder);
    			mCamera.startPreview();
    			mPreview.isPreview = true;
    			if (flagScan) {
    				if (mPreview.timer == null) {
    					mPreview.timer = new Timer();
            			mPreview.isScan = true;
            			mPreview.timer.scheduleAtFixedRate(mPreview.new ScanThread(), 0, 1000);
            		}
    				if (mDrawSurfaceView.timer == null) {
    					mDrawSurfaceView.timer = new Timer();
    					mDrawSurfaceView.isDrawScan = true;
    					mDrawSurfaceView.timer.scheduleAtFixedRate(
    							mDrawSurfaceView.new DrawSurfaceScan(), 0, 1000);
    				}
    	    	}
    		}
    	} catch (IOException e) {
    		e.printStackTrace();
    		Log.d(LOG_TAG,
                    "Error starting camera preview: " + e.getMessage());
		}*/
    }

    
    @Override
    protected void onPause() {
    	Log.d(LOG_TAG, "onPause");
    	super.onPause();
    	
    	//flagScan = mPreview.isScan;
    	if (null != mCamera) {
			mCamera.setOneShotPreviewCallback(null);
		}
    	if (mPreview.isScan) {
    		mDrawSurfaceView.isDrawScan = false;
			mDrawSurfaceView.timer.cancel();
			mDrawSurfaceView.timer = null;
			mPreview.isScan = false;
			mPreview.timer.cancel();
			mPreview.timer = null;
		}
    	if (null != mCamera && mPreview.isPreview) {
			mCamera.stopPreview();
			mPreview.isPreview = false;
		}
    	
    	if (mCamera != null) {
    		mPreview.setCamera(null);
    		Log.d(LOG_TAG, "onPause --> Realease camera");
    		mCamera.release();
    		mCamera = null;
    	}
    	
    	wakeLock.release();
    	
    }
    
    
    @Override
    protected void onDestroy() {
    	Log.d(LOG_TAG, "onDestroy");
    	//mPreview.surfaceDestroyed(mPreview.mHolder);
    	super.onDestroy();
    }
    
    
    private int getDefaultCameraId() {
    	Log.d(LOG_TAG, "getDefaultCameraId");
    	int defaultId = -1;
    	
    	// Find the total number of cameras available
        mNumberOfCameras = Camera.getNumberOfCameras();
        
        // Find the ID of the default camera
        CameraInfo cameraInfo = new CameraInfo();
        for (int i = 0; i < mNumberOfCameras; i++) {
        	Camera.getCameraInfo(i, cameraInfo);
        	Log.d(LOG_TAG, "camera info: " + cameraInfo.orientation);
        	if (cameraInfo.facing == CameraInfo.CAMERA_FACING_FRONT) {
        		defaultId = i;
        		
        	}
        }
        if (-1 == defaultId) {
        	if (mNumberOfCameras > 0) {
        		defaultId = 0;
        		
        	}
        	else {
        		Toast.makeText(getApplicationContext(), R.string.no_camera,
        				Toast.LENGTH_LONG).show();
        	}
        }
        
        return defaultId;
    }
    

	private static Camera getCameraInstance(int cameraId) {
		Log.d(LOG_TAG, "getCameraInstance");
		Camera c = null;
		
		try {
			// attempt to get a Camera instance
			c = Camera.open(cameraId);
		}
		catch (Exception e) {
			e.printStackTrace();
			Log.e(LOG_TAG, "Camera is not available");
		}
		return c;
	}
    
    
}