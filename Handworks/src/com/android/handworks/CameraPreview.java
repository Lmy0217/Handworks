package com.android.handworks;

import java.io.IOException;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;

import android.content.Context;
import android.hardware.Camera;
import android.hardware.Camera.AutoFocusCallback;
import android.hardware.Camera.PreviewCallback;
import android.hardware.Camera.Size;
import android.os.AsyncTask;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class CameraPreview extends SurfaceView implements 
		SurfaceHolder.Callback,PreviewCallback {
	
	protected SurfaceHolder mHolder;
    private Camera mCamera;
    private HandTask mHandTask;
    protected Timer timer;
    //EncoderCore cc = new EncoderCore();
    //MessageOut msg = new MessageOut();

    Size mPreviewSize;
    List<Size> mSupportedPreviewSizes;
    
    private int previewWidth;
    private int previewHeight;
    protected boolean isPreview = false;
	protected boolean isScan = false;
	
	private static final String libSoName = "Handworks";
    private static final String LOG_TAG = "CameraPreview";

    
	public CameraPreview(Context context) {
		super(context);
		//LayoutInflater.from(context).inflate(R.id.camera_preview, this);
		init();

	}

	@SuppressWarnings("deprecation")
	private void init() {
		Log.d(LOG_TAG, "init");
		// Install a SurfaceHolder.Callback so we get notified when the
        // underlying surface is created and destroyed.
		//SurfaceView surfaceview = (SurfaceView) findViewById(R.id.surface_view);
        mHolder = getHolder();
        mHolder.addCallback(this);
        // deprecated setting, but required on Android versions prior to 3.0
        //mHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
        mHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
        
	}
	
	public void setCamera(Camera camera) {
		Log.d(LOG_TAG, "setCamera");
		mCamera = camera;
        if (mCamera != null)
        {
            mSupportedPreviewSizes = mCamera.getParameters()
            		.getSupportedPreviewSizes();
            requestLayout();
        }
		
	}

	@Override
	public void onPreviewFrame(byte[] data, Camera camera) {
		Log.d(LOG_TAG, "onPreviewFrame");
		if(null != mHandTask) {
			switch(mHandTask.getStatus()) {
			case RUNNING:
                return;
            case PENDING:
            	mHandTask.cancel(false);
                break;
			case FINISHED:
				break;
			default:
				break;
			}
		}
		mHandTask = new HandTask(data);
		mHandTask.execute((Void)null);
		
	}

	@Override
	public void surfaceCreated(SurfaceHolder holder) {
		Log.d(LOG_TAG, "surfaceCreated"+isPreview+isScan);
		// The Surface has been created, now tell the camera where to draw the
		// preview.
		try {
			if (null != mCamera) {
				mCamera.setPreviewDisplay(holder);
				
			}
		}
		catch (IOException e1) {
			e1.printStackTrace();
			Log.d(LOG_TAG,
                    "Error setting camera preview display: " + e1.getMessage());
		}
		try {
			if (null != mCamera) {
				mCamera.startPreview();
				isPreview = true;
			}
			Log.d(LOG_TAG, "surfaceCreated successfully! ");
		}
		catch (Exception e) {
			Log.d(LOG_TAG,
                    "Error setting camera preview: " + e.getMessage());
		}
	}

	@Override
	public void surfaceChanged(SurfaceHolder holder, int format, int width,
			int height) {
		Log.d(LOG_TAG, "surfaceChanged"+isPreview+isScan);
		// If your preview can change or rotate, take care of those events here.
        // Make sure to stop the preview before resizing or reformatting it.
		boolean flagScan = isScan;

        if (null == mHolder.getSurface()) {
        	// preview surface does not exist
            return;
        }
        
        // stop preview before making changes
        try {
        	if (null != mCamera) {
        		if (isScan) {
        			mCamera.setOneShotPreviewCallback(null);
        			isScan = false;
        			timer.cancel();
        			timer = null;
        		}
        		mCamera.stopPreview();
        		isPreview = false;
        	}
        }
        catch (Exception e) {
        	// ignore: tried to stop a non-existent preview
        }
        
        // set preview size and make any resize, rotate or
        // reformatting changes here
        
        if (null != mCamera) {
        	Camera.Parameters parameters = mCamera.getParameters();
            parameters.setPreviewSize(mPreviewSize.width, mPreviewSize.height);
            
            requestLayout();
            
            mCamera.setParameters(parameters);
            mCamera.setDisplayOrientation(90);
            
        }
        // 这里可以用来设置尺寸
        
        // start preview with new settings
        try {
        	if (null != mCamera) {
        		mCamera.setPreviewDisplay(mHolder);
                mCamera.startPreview();
                isPreview = true;
                if (flagScan) {
                	if (timer == null) {
            			timer = new Timer();
            			isScan = true;
            			timer.scheduleAtFixedRate(new ScanThread(), 0, 1000);
            		}
                }
        	}
        }
        catch (Exception e) {
        	Log.d(LOG_TAG,
                    "Error starting camera preview: " + e.getMessage());
        }
		
	}

	@Override
	public void surfaceDestroyed(SurfaceHolder holder) {
		Log.d(LOG_TAG, "surfaceDestroyed");
		if (null != mCamera) {
			mCamera.setOneShotPreviewCallback(null);
		}
		if (isScan) {
			isScan = false;
			timer.cancel();
			timer = null;
		}
		if (null != mCamera) {
			mCamera.stopPreview();
			isPreview = false;
		}
		
	}

	@Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
		Log.d(LOG_TAG, "onMeasure");
        super.onMeasure(widthMeasureSpec, heightMeasureSpec);

        // We purposely disregard child measurements because act as a
        // wrapper to a SurfaceView that centers the camera preview instead
        // of stretching it.
        final int width = resolveSize(getSuggestedMinimumWidth(),
                widthMeasureSpec);
        final int height = resolveSize(getSuggestedMinimumHeight(),
                heightMeasureSpec);
        setMeasuredDimension(width, height);

        if (mSupportedPreviewSizes != null)
        {
            mPreviewSize = getOptimalPreviewSize(mSupportedPreviewSizes, width,
                    height);
        }
    }

    private Size getOptimalPreviewSize(List<Size> sizes, int w, int h) {
    	Log.d(LOG_TAG, "getOptimalPreviewSize");
        final double ASPECT_TOLERANCE = 0.1;
        double targetRatio = (double) w / h;
        if (sizes == null)
            return null;

        Size optimalSize = null;
        double minDiff = Double.MAX_VALUE;

        int targetHeight = h;

        // Try to find an size match aspect ratio and size
        for (Size size : sizes)
        {
            double ratio = (double) size.width / size.height;
            if (Math.abs(ratio - targetRatio) > ASPECT_TOLERANCE)
                continue;
            if (Math.abs(size.height - targetHeight) < minDiff)
            {
                optimalSize = size;
                minDiff = Math.abs(size.height - targetHeight);
            }
        }

        // Cannot find the one match the aspect ratio, ignore the requirement
        if (optimalSize == null)
        {
            minDiff = Double.MAX_VALUE;
            for (Size size : sizes)
            {
                if (Math.abs(size.height - targetHeight) < minDiff)
                {
                    optimalSize = size;
                    minDiff = Math.abs(size.height - targetHeight);
                }
            }
        }
        return optimalSize;
    }
    
    public native byte[] Handworks(int previewWidth,
    		int previewHeight, byte[] YuvData);
    
    static {
    	System.loadLibrary(libSoName);
    }
    
    
    class HandTask extends AsyncTask<Void, Void, Void> {
    	
    	private byte[] mData;
    	private byte[] handData;
    	byte[] bt = new byte[2];
    	private byte num_f, num_l;
    	private static final String LOG_TAG = "CameraPreview.HandTask";

    	public HandTask(byte[] data) {
    		this.mData = data;
    	}

    	@Override
    	protected Void doInBackground(Void... params) {
    		Log.d(LOG_TAG, "doInBackground");
    		//获取预览大小
    		Size size = mCamera.getParameters().getPreviewSize();
    		DrawSurfaceView.size = size;
    		previewWidth = size.width;
    		previewHeight = size.height;
    		
    		//Log.d(LOG_TAG, "size"+previewWidth+"*"+previewHeight);
    		/*
    		final YuvImage image = new YuvImage(mData, ImageFormat.NV21,
    				previewWidth, previewHeight, null);
    		
    		ByteArrayOutputStream os = new ByteArrayOutputStream(mData.length);
    		if(!image.compressToJpeg(new Rect(0, 0, previewWidth, previewHeight),
    				100, os)) {
    			return null;
    		}
    		
    		byte[] tmp = os.toByteArray();
    		Bitmap bmp = BitmapFactory.decodeByteArray(tmp, 0, tmp.length);
    		*/
    		handData = new byte[previewWidth * previewHeight];
    		//实时分析预览帧视频的算法
    		//Log.d(LOG_TAG, "length"+previewWidth+"+"+previewHeight);
    		handData = Handworks(previewWidth, previewHeight, mData);
    		num_f = handData[1];
    		num_l = handData[2];
    		Log.d("Data", "" + num_f + " " + num_l);
    		
    		DrawSurfaceView.handData = handData;
    		
    		if(num_f != 0 || num_l != 0) {
    			bt[0] = num_f;
    			bt[1] = num_l;
    		}
    		//byte[] bts = cc.soundCording(bt);
    		//if(bts!= null) msg.msgStart(bts);
    		
    		return null;
    	}

    }
    
    
    
    class ScanThread extends TimerTask {
    	
    	private AutoFocusCallback mAutoFocusCallback;
    	private int time = 0;
    	private static final String LOG_TAG = "CameraPreview.ScanThread";

		@Override
		public void run() {
			if(isScan) {//!Thread.currentThread().isInterrupted()
				Log.i(LOG_TAG, "ScanThread");
					
					if((null != mCamera) && isPreview) {
						// 自动聚焦变量回调
						
						mAutoFocusCallback = new AutoFocusCallback() {
							@Override
							public void onAutoFocus(boolean success,
									Camera camera) {
								// 对焦成功
								if(success) {
									Log.i(LOG_TAG, "mAutoFocusCallback: success...");
										
								}
								else {
									Log.i(LOG_TAG, "mAutoFocusCallback: fail...");
										//mCamera.autoFocus(mAutoFocusCallback);
								}
							}
							
						};
						if((time % 10) == 0) {
							mCamera.autoFocus(mAutoFocusCallback);
						}
						time++;
						
						Log.i(LOG_TAG, "setOneShotPreviewCallback");
						mCamera.setOneShotPreviewCallback(CameraPreview.this);
					}
				
			}
				
			
		}
    	
    }
    
    
}
