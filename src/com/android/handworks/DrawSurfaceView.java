package com.android.handworks;

import java.util.List;
import java.util.Timer;
import java.util.TimerTask;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.graphics.Canvas;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.graphics.PixelFormat;
import android.graphics.PorterDuff.Mode;
import android.graphics.PorterDuffXfermode;
import android.graphics.Rect;
import android.hardware.Camera.Size;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class DrawSurfaceView extends SurfaceView implements
		SurfaceHolder.Callback {
	
	protected SurfaceHolder holder;
	private Bitmap rotateImg;
	protected Timer timer;
	
	int widthSize, heightSize;
	List<Integer> List;
	protected static byte[] handData = null;
	protected static Size size;
	protected int cameraid; 
	protected boolean isDrawScan = false;
	protected int[] colors = null;
	private static int colorp[] = {0x00000000, 0xFFFFFFFF, 0xFFFFA500, 0xFFFF0000, 0xFF0000FF};
									//͸������ɫ���ٻơ���ɫ����ɫ
	
	private static final String LOG_TAG = "DrawSurfaceView";

	
	public DrawSurfaceView(Context context) {
		super(context);
		init();
	}

	private void init() {
		Log.d(LOG_TAG, "init"+isDrawScan);
		//rotateImg = BitmapFactory.decodeResource(getResources(), 
		//		R.drawable.ic);
		//rotateImg = Bitmap.createScaledBitmap(rotateImg, 400, 400, true);
		
		holder = getHolder();
		holder.addCallback(this);
		// �������SurfaceView���͸��
		holder.setFormat(PixelFormat.TRANSPARENT);
		
		this.setZOrderOnTop(true);
		
	}

	@Override
	public void surfaceCreated(SurfaceHolder holder) {
		Log.d(LOG_TAG, "surfaceCreated"+isDrawScan);
		
	}

	@Override
	public void surfaceChanged(SurfaceHolder holder, int format, int width,
			int height) {
		Log.d(LOG_TAG, "surfaceChanged"+isDrawScan);
		boolean flagScan = isDrawScan;
		
		if (null == holder.getSurface()) {
			return;
		}
		
		try {
			if (isDrawScan) {
				isDrawScan = false;
				timer.cancel();
       			timer = null;
			}
		}
		catch (Exception e) {
			
		}
		
		if (flagScan) {
			if (timer == null) {
				timer = new Timer();
				isDrawScan = true;
    			timer.scheduleAtFixedRate(new DrawSurfaceScan(), 0, 1000);
			}
		}
		
	}

	@Override
	public void surfaceDestroyed(SurfaceHolder holder) {
		Log.d(LOG_TAG, "surfaceDestroyed"+isDrawScan);
		if (isDrawScan) {
			isDrawScan = false;
			timer.cancel();
			timer = null;
		}
		
	}
	
	@Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec)
    {
		Log.d(LOG_TAG, "onMeasure");
        super.onMeasure(widthMeasureSpec, heightMeasureSpec);

        widthSize = MeasureSpec.getSize(widthMeasureSpec);  
        heightSize = MeasureSpec.getSize(heightMeasureSpec); 
        // We purposely disregard child measurements because act as a
        // wrapper to a SurfaceView that centers the camera preview instead
        // of stretching it.
        final int width = resolveSize(getSuggestedMinimumWidth(),
                widthMeasureSpec);
        final int height = resolveSize(getSuggestedMinimumHeight(),
                heightMeasureSpec);
        setMeasuredDimension(width, height);
        
    }
	
	
	class DrawSurfaceScan extends TimerTask {
		
		private Canvas canvas = null;
		private Paint paint = null;
		int rotate = 0;
		
		private static final String LOG_TAG = "DrawSurfaceView_Scan";

		@Override
		public void run() {
			Log.i(LOG_TAG, "run");
			if(isDrawScan && (handData != null)) {
				//Log.i(LOG_TAG, "handler.run...");
				draw();
						
			}
			
		}
		
		private void draw() {
			Log.i(LOG_TAG, "draw");
			//Log.i(LOG_TAG, "colors"+handData.length+"+"+size.width+size.height+"+"+widthSize+heightSize);
			byte t;
			if(colors == null) {
				//Log.i(LOG_TAG, "colors...");
				colors = new int[handData.length];
			}
			int j = 0;
			int k = handData[0];
			int p1 = handData[1], p2 = handData[2];
			for(int i = 3; i < colors.length; i++) {
				t = handData[i];
				if(t > 0) {
					if(t == k) {
						colors[i]=colorp[2];
					}
					else if(t == 127) {
						colors[i]=colorp[3];
					}
					else if(t == 126) {
						colors[i]=colorp[4];
					}
					else {
						colors[i]=colorp[1];
					}
					j++;
				}
				else {
					colors[i]=colorp[0];
				}
			}
			Log.i(LOG_TAG, "j="+j);
			
			rotateImg = Bitmap.createBitmap(colors, size.width, size.height, Config.ARGB_8888);
			rotateImg = Bitmap.createScaledBitmap(rotateImg, heightSize, widthSize, true);
			
			
			// ��ȡ����
			canvas = holder.lockCanvas(new Rect(0, 0, widthSize, heightSize));
			
			if(canvas != null) {
				Log.i(LOG_TAG, "canvas...");
				paint = new Paint();
				paint.setTextSize(50);
				paint.setXfermode(new PorterDuffXfermode(Mode.CLEAR));
				canvas.drawPaint(paint);
				paint.setXfermode(new PorterDuffXfermode(Mode.SRC));
            
				// ������ת�ı���  
				//canvas.drawBitmap(rotateImg, 0, 0, paint);
            
				// �����������ͼƬ��ת��ƽ��  
				Matrix matrix = new Matrix();
				
				if(cameraid == 1) {
					Log.i(LOG_TAG, "post");
					matrix.postScale(-1, 1);
					matrix.postTranslate(rotateImg.getWidth(), -rotateImg.getHeight());
				}
				else {
					// ������߾���ϱ߾�  
					matrix.postTranslate(rotate, -rotateImg.getHeight());
				}
            
				// ������ת�Ƕ�  
				matrix.postRotate(90, rotate, rotate); 
            
				// ������תͼƬ  
				Log.i(LOG_TAG, "drawbitmap");
				canvas.drawBitmap(rotateImg, matrix, paint);
				
				Log.i(LOG_TAG, "drawtest");
				canvas.drawPosText("" + p1 + p2, new float[]{50, 50, 100, 50, 150, 50, 200, 50}, paint);

			}
			
			rotateImg.recycle();
			rotateImg = null;
            
            // �����������ύ���õ�ͼ��
            holder.unlockCanvasAndPost(canvas);
		}
	}
	
}