package com.amcop.app;

import android.app.AlertDialog;
import android.app.NativeActivity;
import android.content.Context;
import android.content.res.AssetManager;
import android.content.res.Configuration;
import android.graphics.Color;
import android.os.Bundle;
import android.os.Handler;
import android.support.v4.view.ViewPager.LayoutParams;
import android.util.Log;
import android.view.Gravity;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.View.OnTouchListener;
import android.view.ViewGroup.MarginLayoutParams;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.PopupWindow;

public class mainActivity extends NativeActivity {

	/*
	 * Loading Navite Activity
	 */
	static {
		System.loadLibrary("native-activity");
	}
	
	/*
	 * For pushEvent Function
	 */
	private enum pushEventType{
		PET_VIEW_EYE_MOVE_RIGHT(0),				// View center 중심으로 eye 를 오른쪽으로 회전
		PET_VIEW_EYE_MOVE_LEFT(1),					// View center 중심으로 eye 를 왼쪽으로 회전
		PET_VIEW_EYE_MOVE_UP(2),					// View center 중심으로 eye 를 위로 회전
		PET_VIEW_EYE_MOVE_DOWN(3),				// View center 중심으로 eye 를 아래로 회전
		PET_VIEW_CENTER_MOVE_RIGHT(4),			// View center 중심을 center to eye 연장선의 90도 방향으로 (x, y axis 값만) 이동, eye 도 같이 이동.
		PET_VIEW_CENTER_MOVE_LEFT(5),			// View center 중심을 center to eye 연장선의 -90도 방향으로 (x, y axis 값만) 이동, eye 도 같이 이동.
		PET_VIEW_CENTER_MOVE_FRONT_SIDE(6),	// View center 중심을 center to eye 연장선의 0도 방향으로 (x, y axis 값만) 이동, eye 도 같이 이동.
		PET_VIEW_CENTER_MOVE_BACK_SIDE(7),	// View center 중심을 center to eye 연장선의 180도 방향으로 (x, y axis 값만) 이동, eye 도 같이 이동.
		PET_VIEW_ZOOM_IN(8),							// View center to eye 연장선 위에서 이동. center 와 eye 의 거리를 줄인다.
		PET_VIEW_ZOOM_OUT(9),						// View center to eye 연장선 위에서 이동. center 와 eye 의 거리를 늘인다.
		PET_VIEW_SET_EYE_LR(10),						// View center 중심으로 eye 위치의 각도 지정.
		PET_VIEW_SET_EYE_UD(11),					// View center 중심으로 eye 높이 각도 지정.
		PET_VIEW_SET_EYE_TOP(12),					// View center 중심으로 eye 높이 각도 지정.
		PET_VIEW_SET_CENTER(13),					// View center 지정.
		PET_VIEW_SET_ZOOM(14),						// View zoom 지정.
		;
		
		pushEventType(int value) {this.value = value; }
		private final int value;
		public int value() {return value; }
	};

	public native static int pushEvent(int type, int wParam, int lParam, Object obj);
    public static native int setAssetManager(Object assetManager);  
    
	private static final int INTERVAL = 16;
	private Handler handler = new Handler();
	
	/*
	 * For Android UI
	 */
	mainActivity	mActivity;
	Context			mContext;
	AssetManager mAssetManager;
	WindowManager	mWm; 
	
	LinearLayout 	mainLayout;

	// eye move button	
	LinearLayout 	btnEyeMoveTopLayout;
	PopupWindow 	btnEyeMoveTopPopUp;
	Button 			btnEyeMoveTopBtn;
	
	// center move button (view target)	
	LinearLayout 	btnCenterMoveTopLayout;
	PopupWindow 	btnCenterMoveTopPopUp;
	Button 			btnCenterMoveTop;
	
	// zoom button	
	LinearLayout 	btnZoomInLayout, btnZoomOutLayout;
	PopupWindow 	btnZoomInPopUp, btnZoomOutPopUp;
	Button 			btnZoomIn, btnZoomOut;
	
	/*
	 * Android Activity Life Cycle
	 */
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
//		requestWindowFeature(Window.FEATURE_NO_TITLE);
		setContentView(R.layout.main);
		
		 // Make your custom init here
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
		
		mActivity = this;
		mContext = getApplicationContext();
		mAssetManager = getResources().getAssets();
		mWm		 = (WindowManager) getSystemService(WINDOW_SERVICE);
//		pushEvent(pushEventType.PET_SET_SCREEN_SIZE.value(), mWm.getDefaultDisplay().getWidth(), mWm.getDefaultDisplay().getHeight(), null);
            
        // set asset manager
        int retVal = setAssetManager(mAssetManager);
        Log.i("mainActivity", String.format("[SET ASSET MANAGER RETVAL : %d]", retVal));
        
		initUI();
	}

	@Override
	public void onConfigurationChanged(Configuration newConfig) {
		// TODO Auto-generated method stub
		super.onConfigurationChanged(newConfig);
//		pushEvent(pushEventType.PET_SET_SCREEN_SIZE.value(), mWm.getDefaultDisplay().getWidth(), mWm.getDefaultDisplay().getHeight(), null);
	}
	
	@Override
	protected void onDestroy() {
		// TODO Auto-generated method stub
		super.onDestroy();
				
		btnEyeMoveTopPopUp.dismiss();
		btnCenterMoveTopPopUp.dismiss();
	
		btnZoomInPopUp.dismiss();
		btnZoomOutPopUp.dismiss();
		
	}

	/*
	 * continues button pressed.
	 */
	private Runnable eyeMoveRight = new Runnable() {
		public void run() {
			// TODO Auto-generated method stub
			pushEvent(pushEventType.PET_VIEW_EYE_MOVE_RIGHT.value(), 0, 0, null);
			handler.postDelayed(this, INTERVAL);
		}
	};
	
	private Runnable eyeMoveLeft = new Runnable() {
		public void run() {
			// TODO Auto-generated method stub
			pushEvent(pushEventType.PET_VIEW_EYE_MOVE_LEFT.value(), 0, 0, null);
			handler.postDelayed(this, INTERVAL);
		}
	};
	
	private Runnable eyeMoveUp = new Runnable() {
		public void run() {
			// TODO Auto-generated method stub
			pushEvent(pushEventType.PET_VIEW_EYE_MOVE_UP.value(), 0, 0, null);
			handler.postDelayed(this, INTERVAL);
		}
	};
	
	private Runnable eyeMoveDown = new Runnable() {
		public void run() {
			// TODO Auto-generated method stub
			pushEvent(pushEventType.PET_VIEW_EYE_MOVE_DOWN.value(), 0, 0, null);
			handler.postDelayed(this, INTERVAL);
		}
	};
	
	private Runnable centerMoveRight = new Runnable() {
		public void run() {
			// TODO Auto-generated method stub
			pushEvent(pushEventType.PET_VIEW_CENTER_MOVE_RIGHT.value(), 0, 0, null);
			handler.postDelayed(this, INTERVAL);
		}
	};
	
	private Runnable centerMoveLeft = new Runnable() {
		public void run() {
			// TODO Auto-generated method stub
			pushEvent(pushEventType.PET_VIEW_CENTER_MOVE_LEFT.value(), 0, 0, null);
			handler.postDelayed(this, INTERVAL);
		}
	};
	
	private Runnable centerMoveUp = new Runnable() {
		public void run() {
			// TODO Auto-generated method stub
			pushEvent(pushEventType.PET_VIEW_CENTER_MOVE_FRONT_SIDE.value(), 0, 0, null);
			handler.postDelayed(this, INTERVAL);
		}
	};
	
	private Runnable centerMoveDown = new Runnable() {
		public void run() {
			// TODO Auto-generated method stub
			pushEvent(pushEventType.PET_VIEW_CENTER_MOVE_BACK_SIDE.value(), 0, 0, null);
			handler.postDelayed(this, INTERVAL);
		}
	};
	
	private Runnable zoomIn = new Runnable() {
		public void run() {
			// TODO Auto-generated method stub
			pushEvent(pushEventType.PET_VIEW_ZOOM_IN.value(), 0, 0, null);
			handler.postDelayed(this, INTERVAL);
		}
	};
	
	private Runnable zoomOut = new Runnable() {
		public void run() {
			// TODO Auto-generated method stub
			pushEvent(pushEventType.PET_VIEW_ZOOM_OUT.value(), 0, 0, null);
			handler.postDelayed(this, INTERVAL);
		}
	};
	
	/*
	 * show Android XML UI
	 */
	public void showAndroidView() {
		mActivity.runOnUiThread(new Runnable() {

			public void run() {				
				btnEyeMoveTopPopUp.showAtLocation(mainLayout, Gravity.TOP|Gravity.RIGHT, 0, 50);
				btnCenterMoveTopPopUp.showAtLocation(mainLayout, Gravity.TOP|Gravity.LEFT, 0, 50);
			
				btnZoomInPopUp.showAtLocation(mainLayout, Gravity.BOTTOM|Gravity.LEFT, 0, 0);
				btnZoomOutPopUp.showAtLocation(mainLayout, Gravity.BOTTOM|Gravity.LEFT, 0, 100);
			}
		});
	}
	
	public void showAndroidPopup(final int val, final int objID, final float mx, final float my) {
		mActivity.runOnUiThread(new Runnable() {

			public void run() {
				//Toast.makeText(mContext, String.valueOf(val), Toast.LENGTH_SHORT).show();
			    AlertDialog.Builder b = new AlertDialog.Builder(mainActivity.this)
				.setTitle("CLICK")
				.setMessage(String.format("[%d][%d],  %f, %f", val, objID, mx/100000, my/100000))
				.setPositiveButton("OK", null);
			
		        b.create().show();
			}
		});
	}
	
	public int getScreenWidth() {
		return mWm.getDefaultDisplay().getWidth();
	}
	
	public int getScreenHeight() {
		return mWm.getDefaultDisplay().getHeight();
	}
	
	/*
	 * init layout 
	 */
	private void initUI() {
		mainLayout = new LinearLayout(this);
		
		/*
		 * eye move buttons
		 */
		// eye move top button
		btnEyeMoveTopBtn = new Button(this);
		btnEyeMoveTopBtn.setBackgroundResource(R.drawable.map_current_view_button);
		btnEyeMoveTopBtn.setOnClickListener(new OnClickListener() {
			public void onClick(View v) {
				// TODO Auto-generated method stub
				pushEvent(pushEventType.PET_VIEW_SET_EYE_TOP.value(), 0, 0, null);
			}
		});
				
		btnEyeMoveTopLayout = new LinearLayout(this);
		btnEyeMoveTopLayout.setBackgroundColor(Color.TRANSPARENT);
		btnEyeMoveTopLayout.setOrientation(LinearLayout.VERTICAL);
		btnEyeMoveTopLayout.addView(btnEyeMoveTopBtn, new MarginLayoutParams(LayoutParams.FILL_PARENT, LayoutParams.FILL_PARENT));

		btnEyeMoveTopPopUp = new PopupWindow(btnEyeMoveTopLayout, 100, 100);
		
		/*
		 * center move buttons (view target)
		 */
		// Center move top button
		btnCenterMoveTop = new Button(this);
		btnCenterMoveTop.setBackgroundResource(R.drawable.map_current_view_button);
		btnCenterMoveTop.setOnClickListener(new OnClickListener() {
			public void onClick(View v) {
				// TODO Auto-generated method stub
				pushEvent(pushEventType.PET_VIEW_SET_CENTER.value(), 0, 0, null);
			}
		});
				
		btnCenterMoveTopLayout = new LinearLayout(this);
		btnCenterMoveTopLayout.setBackgroundColor(Color.TRANSPARENT);
		btnCenterMoveTopLayout.setOrientation(LinearLayout.VERTICAL);
		btnCenterMoveTopLayout.addView(btnCenterMoveTop, new MarginLayoutParams(LayoutParams.FILL_PARENT, LayoutParams.FILL_PARENT));

		btnCenterMoveTopPopUp = new PopupWindow(btnCenterMoveTopLayout, 100, 100);
		
		// zoom in button
		btnZoomIn = new Button(this);
		btnZoomIn.setBackgroundResource(R.drawable.map_current_view_button);
		btnZoomIn.setOnTouchListener(new OnTouchListener() {
			public boolean onTouch(View v, MotionEvent event) {
				// TODO Auto-generated method stub
				int i = event.getAction();
				if(i == MotionEvent.ACTION_DOWN) {
					zoomIn.run();
				} else if(i == MotionEvent.ACTION_UP) {
					handler.removeCallbacks(zoomIn);
				}
				return true;
			}
		});
				
		btnZoomInLayout = new LinearLayout(this);
		btnZoomInLayout.setBackgroundColor(Color.TRANSPARENT);
		btnZoomInLayout.setOrientation(LinearLayout.VERTICAL);
		btnZoomInLayout.addView(btnZoomIn, new MarginLayoutParams(LayoutParams.FILL_PARENT, LayoutParams.FILL_PARENT));

		btnZoomInPopUp = new PopupWindow(btnZoomInLayout, 100, 100);
		
		// zoom out button
		btnZoomOut = new Button(this);
		btnZoomOut.setBackgroundResource(R.drawable.map_current_view_button);
		btnZoomOut.setOnTouchListener(new OnTouchListener() {
			public boolean onTouch(View v, MotionEvent event) {
				// TODO Auto-generated method stub
				int i = event.getAction();
				if(i == MotionEvent.ACTION_DOWN) {
					zoomOut.run();
				} else if(i == MotionEvent.ACTION_UP) {
					handler.removeCallbacks(zoomOut);
				}
				return true;
			}
		});
				
		btnZoomOutLayout = new LinearLayout(this);
		btnZoomOutLayout.setBackgroundColor(Color.TRANSPARENT);
		btnZoomOutLayout.setOrientation(LinearLayout.VERTICAL);
		btnZoomOutLayout.addView(btnZoomOut, new MarginLayoutParams(LayoutParams.FILL_PARENT, LayoutParams.FILL_PARENT));

		btnZoomOutPopUp = new PopupWindow(btnZoomOutLayout, 100, 100);
	}
}