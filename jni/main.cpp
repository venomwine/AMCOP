#include "main.h"

static int engine_change_display(struct engine* engine) {

	engine->width = getScreenWidth();
	engine->height = getScreenHeight();

	// Initialize World
	OGL.initWorld(engine->width, engine->height);

	return 0;
}
/**
 * Initialize an EGL context for the current display.
 */
static int engine_init_display(struct engine* engine) {
	 // initialize OpenGL ES and EGL

	/*
	 * Here specify the attributes of the desired configuration.
	 * Below, we select an EGLConfig with at least 8 bits per color
	 * component compatible with on-screen windows
	 */

    const EGLint attribs[] = {
    		EGL_SURFACE_TYPE,
    		EGL_WINDOW_BIT,
    		EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_DEPTH_SIZE, 16,
            EGL_NONE
    };
    EGLint w, h, dummy, format;
    EGLint numConfigs;
    EGLConfig config;
    EGLSurface surface;
    EGLContext context;

    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    eglInitialize(display, 0, 0);

    /* Here, the application chooses the configuration it desires. In this
     * sample, we have a very simplified selection process, where we pick
	 * the first EGLConfig that matches our criteria */
    eglChooseConfig(display, attribs, &config, 1, &numConfigs);

    /* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
	 * guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
	 * As soon as we picked a EGLConfig, we can safely reconfigure the
	 * ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
    eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);

    ANativeWindow_setBuffersGeometry(engine->app->window, 0, 0, format);

    surface = eglCreateWindowSurface(display, config, engine->app->window, NULL);
    context = eglCreateContext(display, config, NULL, NULL);

    if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
        LOGW("[OGL]Unable to eglMakeCurrent");
        return -1;
    }

    eglQuerySurface(display, surface, EGL_WIDTH, &w);
    eglQuerySurface(display, surface, EGL_HEIGHT, &h);

    engine->display = display;
    engine->context = context;
    engine->surface = surface;

    engine_change_display(engine);

    return 0;
}

/**
 * Just the current frame in the display.
 */
static void engine_draw_frame(struct engine* engine) {

    if (engine->display == NULL) {
    	// No display.
    	return;
    }

    // check old touch
    TG.flag += TG.flag < TOUCH_FLAG ? 1 : 0;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    OGL.m_VC->calcSmoothView();
    OGL.draw3DWorld();
    OGL.draw2DInfo();
    eglSwapBuffers(engine->display, engine->surface);
}

/**
 * Tear down the EGL context currently associated with the display.
 */
static void engine_term_display(struct engine* engine) {
    if (engine->display != EGL_NO_DISPLAY) {
        eglMakeCurrent(engine->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (engine->context != EGL_NO_CONTEXT) {
            eglDestroyContext(engine->display, engine->context);
        }
        if (engine->surface != EGL_NO_SURFACE) {
            eglDestroySurface(engine->display, engine->surface);
        }
        eglTerminate(engine->display);
    }
    engine->animating = 0;
    engine->display = EGL_NO_DISPLAY;
    engine->context = EGL_NO_CONTEXT;
    engine->surface = EGL_NO_SURFACE;
}

/**
 * Process the next input event.
 */
static int32_t engine_handle_input(struct android_app* app,
        AInputEvent* event) {
    struct engine* engine = (struct engine*) app->userData;
    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {

    	// Touch Screen
        size_t count = AMotionEvent_getPointerCount(event);
        if(count > POINT_MAX)
            count = POINT_MAX;

        // check touch action count
        if(TG.action == count) {
			TG.count += TG.count < TOUCH_ACTION_COUNT ? 1 : 0;
		} else {
			TG.action = count;
			TG.count = TOUCH_INIT;
		}

        // get new touch points
        TouchPoint point[POINT_MAX];
        for (int i = 0; i < count; i++){
            point[i].x = AMotionEvent_getX(event, i);
            point[i].y = AMotionEvent_getY(event, i);
//            LOGI("touch point(%d): x=%d y=%d", i, point[i].x, point[i].y);
        }

        // calc points distance
		int moveXVal = 0;
		int moveYVal = 0;
		float moveDist = 0;

        // touch action
        size_t action = AMotionEvent_getAction(event) & AMOTION_EVENT_ACTION_MASK;
		switch(action) {

		case AMOTION_EVENT_ACTION_DOWN:
//			LOGI("[MAP]CLICK DOWN");
			TG.click = 1;

			// Inertia motion.
			clock_gettime(CLOCK_REALTIME, &touchCheck[0].time);
			touchCheck[0].point.x = point[0].x;
			touchCheck[0].point.y = point[0].y;

			break;

		case AMOTION_EVENT_ACTION_UP:
			if(TG.click == 1) {
//				LOGI("[MAP]CLICK");
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				OGL.drawPicking3DWorld();
				OGL.drawPicking2DInfo();

				GLfloat mx = 0, my = 0;
				unsigned int objID = 0;
				int retval = OGL.getObjInfo(point[0].x, point[0].y, objID, mx, my);
				if(retval > 0) {
//					showAndroidPopup(retval, objID, mx, my);
//					TG.popup = TOUCH_POPUP;
				}
			} else if (TG.count >= TOUCH_ACTION_COUNT) {
//				LOGI("[MAP]CLICK UP");
				// Inertia motion.
				clock_gettime(CLOCK_REALTIME, &touchCheck[1].time);
				touchCheck[1].point.x = point[0].x;
				touchCheck[1].point.y = point[0].y;

				int touchMoveXVal = touchCheck[1].point.x - touchCheck[0].point.x;
				int touchMoveYVal = touchCheck[1].point.y - touchCheck[0].point.y;
				float touchMoveDist = sqrt((float)(pow((double)touchMoveXVal, 2) + pow((double)touchMoveYVal, 2)));

				double firstTime = touchCheck[0].time.tv_sec + ((double)touchCheck[0].time.tv_nsec / TIME_DIV_VALUE);
				double upTime = touchCheck[1].time.tv_sec + ((double)touchCheck[1].time.tv_nsec / TIME_DIV_VALUE);

				if(upTime - firstTime < touchMoveDist / MAP_INERTIA_UNIT) {
					if(0.0f < touchMoveDist) {
						float moveDegree = OGL.m_VC->getTanDegree((float)touchMoveXVal, (float)touchMoveYVal);
						OGL.m_VC->changeViewCenter(moveDegree, touchMoveDist * MAP_INERTIA_VALUE * (OGL.m_VC->getEyeDistance3D(CURRENT_VIEW) / INIT_ZOOM_VAL));	// map ?�리�?조절 ?�요
					}
				}
			}
			break;

		case AMOTION_EVENT_ACTION_MOVE:
			moveXVal = point[0].x - oldPoint[0].x;
			moveYVal = point[0].y - oldPoint[0].y;
			moveDist = sqrt((float)(pow((double)moveXVal, 2) + pow((double)moveYVal, 2)));
			if(moveDist > 1.0f)
				TG.click = 0;

			if(TG.flag < TOUCH_FLAG 								// 터치 튐 방지
					&& TG.count >= TOUCH_ACTION_COUNT 	// CENTER MOVE <-> ZOOM <-> EYE MOVE 간 튐 방지
					&& TG.popup != TOUCH_POPUP				// Popup 이 아닐 때
					) {
				switch(count) {
				case 1:	// CENTER MOVE
					{
						if(0.0f < moveDist) {
//							LOGI("[MAP]CENTER MOVE");
							float moveDegree = OGL.m_VC->getTanDegree((float)moveXVal, (float)moveYVal);
							OGL.m_VC->changeViewCenter(moveDegree, moveDist * (OGL.m_VC->getEyeDistance3D(CURRENT_VIEW) / INIT_ZOOM_VAL));	// map ?�리�?조절 ?�요
						}
					}
					break;
				case 2:	// ZOOM
					{
//						LOGI("[MAP]ZOOM");
						float oldTouchDistance = sqrt(pow((double)(oldPoint[0].x - oldPoint[1].x), 2) + pow((double)(oldPoint[0].y - oldPoint[1].y), 2));
						float newTouchDistance = sqrt(pow((double)(point[0].x - point[1].x), 2) + pow((double)(point[0].y - point[1].y), 2));
						float zoomVal = oldTouchDistance - newTouchDistance;

						float moveXVal = point[0].x - oldPoint[0].x;
//						float moveYVal = -(point[0].y - oldPoint[0].y);

						if(point[0].y > point[1].y)
							moveXVal = -moveXVal;

						// ZOOM
						OGL.m_VC->setEyeDistance(CHANGE_VIEW, zoomVal * OGL.m_VC->getEyeDistance3D(CURRENT_VIEW)/INIT_ZOOM_VAL*10.0f);
					}
					break;
				case 3:	// EYE MOVE
					{
						LOGI("[MAP]EYE MOVE");
						float moveXVal = point[0].x - oldPoint[0].x;
						float moveYVal = -(point[0].y - oldPoint[0].y);

						OGL.m_VC->setEyeTurnAngle(CHANGE_VIEW, moveXVal/2.0f);
						OGL.m_VC->setEyeTiltAngle(CHANGE_VIEW, moveYVal/2.0f);	// zoom 레벨에 따른 tilt 각도 고정.
					}
					break;
				default:
					break;
				}
			}
			break;
		}

        // save old touch points
        for (int i = 0; i < count; i++){
			oldPoint[i].x = point[i].x;
			oldPoint[i].y = point[i].y;
		}

        TG.flag = TOUCH_INIT;

        return 1;
    }
    return 0;
}

/**
 * Process the next main command.
 */
static void engine_handle_cmd(struct android_app* app, int32_t cmd) {
    struct engine* engine = (struct engine*) app->userData;
    switch (cmd) {

    case APP_CMD_INPUT_CHANGED:
    	LOGI("[APP_CMD]_INPUT_CHANGED");
    	break;

    case APP_CMD_INIT_WINDOW:
		LOGI("[APP_CMD]_INIT_WINDOW");
		// The window is being shown, get it ready.
		if (engine->app->window != NULL) {
			engine_init_display(engine);
			engine->animating = 1;
			engine_draw_frame(engine);

			// show Android Activity
			showAndroidView();
		}
		break;

    case APP_CMD_TERM_WINDOW:
		LOGI("[APP_CMD]_TERM_WINDOW");
		// The window is being hidden or closed, clean it up.
		engine_term_display(engine);
		break;

    case APP_CMD_WINDOW_RESIZED:
    	LOGI("[APP_CMD]_WINDOW_RESIZED");
    	break;

    case APP_CMD_WINDOW_REDRAW_NEEDED:
		LOGI("[APP_CMD]_WINDOW_REDRAW_NEEDED");
		break;

    case APP_CMD_CONTENT_RECT_CHANGED:
		LOGI("[APP_CMD]_CONTENT_RECT_CHANGED");
		break;

    case APP_CMD_GAINED_FOCUS:
		LOGI("[APP_CMD]_GAINED_FOCUS");
		// When our app gains focus, we start monitoring the accelerometer.
		TG.popup = TOUCH_INIT;
		engine->animating = 1;
		engine_draw_frame(engine);
		break;

    case APP_CMD_LOST_FOCUS:
		LOGI("[APP_CMD]_LOST_FOCUS");
		// When our app loses focus, we stop monitoring the accelerometer.
		// Also stop animating.
		engine->animating = 0;
		engine_draw_frame(engine);
		break;

    case APP_CMD_CONFIG_CHANGED:
		LOGI("[APP_CMD]_CONFIG_CHANGED");
		// The window is being shown, get it ready.
		if(engine->animating == 1) {
			engine_change_display(engine);
			engine_draw_frame(engine);
		}
		break;

    case APP_CMD_LOW_MEMORY:
		LOGI("[APP_CMD]_LOW_MEMORY");
		break;

    case APP_CMD_START:
		LOGI("[APP_CMD]_START");
		break;

    case APP_CMD_RESUME:
		LOGI("[APP_CMD]_RESUME");
		break;

    case APP_CMD_SAVE_STATE:
    	LOGI("[APP_CMD]_SAVE_STATE");
    	// The system has asked us to save our current state.  Do so.
        engine->app->savedState = malloc(sizeof(saved_state2));
        *((saved_state2*) engine->app->savedState) = engine->state;
        engine->app->savedStateSize = sizeof(saved_state2);
        break;

    case APP_CMD_PAUSE:
		LOGI("[APP_CMD]_PAUSE");
		break;

    case APP_CMD_STOP:
		LOGI("[APP_CMD]_STOP");
		break;

    case APP_CMD_DESTROY:
		LOGI("[APP_CMD]_DESTROY");
		break;
    }
}

/**
 * This is the main entry point of a native application that is using
 * android_native_app_glue.  It runs in its own thread, with its own
 * event loop for receiving input events and doing other things.
 */
void android_main(struct android_app* state) {
    struct engine engine;

    // Make sure glue isn't stripped.
    app_dummy();

    memset(&engine, 0, sizeof(engine));
    state->userData = &engine;
    state->onAppCmd = engine_handle_cmd;
    state->onInputEvent = engine_handle_input;
    engine.app = state;

    // Prepare to monitor accelerometer
    engine.sensorManager = ASensorManager_getInstance();
    engine.sensorEventQueue = ASensorManager_createEventQueue(
            engine.sensorManager, state->looper, LOOPER_ID_USER, NULL,
            NULL);

    if (state->savedState != NULL) {
    	// We are starting with a previous saved state; restore from it.
    	LOGI(">> SAVE STATE RESTORE");
        engine.state = *(saved_state2*) state->savedState;
    }

    g_app = state;

    // loop waiting for stuff to do.

    while (1) {

    	// Read all pending events.
    	int ident;
        int events;
        struct android_poll_source* source;

        // If not animating, we will block forever waiting for events.
        // If animating, we loop until all events are read, then continue
        // to draw the next frame of animation.
        while ((ident = ALooper_pollAll(engine.animating ? 0 : -1, NULL, &events, (void**) &source)) >= 0) {

        	// Process this event.
        	if (source != NULL) {
                source->process(state, source);
            }

        	// Check if we are exiting.
        	if (state->destroyRequested != 0) {
                engine_term_display(&engine);
                return;
            }
        }

        if (engine.animating) {
        	// Drawing is throttled to the screen update rate, so there
			// is no need to do timing here.
            engine_draw_frame(&engine);
        }

    }
}

/*
 * Java to Native C
 */
jint pushEvent
	(JNIEnv *env, jclass cls, jint message, jint wParam, jint lParam, jobject obj)
{
	switch(message) {

		case PET_VIEW_EYE_MOVE_RIGHT:
			OGL.m_VC->setEyeTurnAngle(CHANGE_VIEW, 0.5f);
			break;

		case PET_VIEW_EYE_MOVE_LEFT:
			OGL.m_VC->setEyeTurnAngle(CHANGE_VIEW, -0.5f);
			break;

		case PET_VIEW_EYE_MOVE_UP:
			OGL.m_VC->setEyeTiltAngle(CHANGE_VIEW, 0.5f);
			break;

		case PET_VIEW_EYE_MOVE_DOWN:
			OGL.m_VC->setEyeTiltAngle(CHANGE_VIEW, -0.5f);
			break;

		case PET_VIEW_SET_EYE_TOP:
			OGL.m_VC->setEyeTurnAngle(TARGET_VIEW, 0);
			OGL.m_VC->setEyeTiltAngle(TARGET_VIEW, 90);
			OGL.m_VC->setEyeDistance(TARGET_VIEW, 1500.0f);
			OGL.m_VC->setViewCenter(TARGET_VIEW, 0.0f, 0.0f);

			break;

		case PET_VIEW_CENTER_MOVE_RIGHT:
			OGL.m_VC->changeViewCenter(90.0f, 4.0f);
			break;

		case PET_VIEW_CENTER_MOVE_LEFT:
			OGL.m_VC->changeViewCenter(-90.0f, 4.0f);
			break;

		case PET_VIEW_CENTER_MOVE_FRONT_SIDE:
			OGL.m_VC->changeViewCenter(0.0f, 4.0f);
			break;

		case PET_VIEW_CENTER_MOVE_BACK_SIDE:
			OGL.m_VC->changeViewCenter(180.0f, 4.0f);
			break;

		case PET_VIEW_SET_CENTER:
			OGL.m_VC->setViewCenter(TARGET_VIEW, 0.0f, 0.0f);
			break;

		case PET_VIEW_ZOOM_IN:
			OGL.m_VC->setEyeDistance(CHANGE_VIEW, -0.5 * OGL.m_VC->getEyeDistance3D(CURRENT_VIEW)/INIT_ZOOM_VAL*50.0f);
			break;

		case PET_VIEW_ZOOM_OUT:
			OGL.m_VC->setEyeDistance(CHANGE_VIEW, +0.5 * OGL.m_VC->getEyeDistance3D(CURRENT_VIEW)/INIT_ZOOM_VAL*50.0f);
			break;

		default:
			break;
	}

	return 0;
}

jint setAssetManager(JNIEnv* env, jclass clazz, jobject assetManager)
{
	LOGI("[SET ASSET MANAGER]");

	g_assetMgr = AAssetManager_fromJava(env, assetManager);
	assert(NULL != g_assetMgr);

	if(NULL == g_assetMgr) {
		LOGI("[SET ASSET MANAGER FAIL]");
		return -1;
	}

	return 0;
}

jint setSdcardPath(JNIEnv* env, jclass clazz, jstring sdcardPath)
{
	LOGI("[SET SDCARD PATH]");

	const char* str = env->GetStringUTFChars(sdcardPath, 0);
	memset(g_sdcardPath, 0, 512);
	strncpy(g_sdcardPath, str, 511);
	LOGI("==============>> set SDCARD path : %s" ,g_sdcardPath);
	env->ReleaseStringUTFChars(sdcardPath, str);

	return 0;
}

/*
 * JNI Function ?� ?�곳???�록?�여 ?�용.
 */
static JNINativeMethod wrMethods[] = {
	/* name, signature, funcPtr */
	{"pushEvent"									, "(IIILjava/lang/Object;)I"		, (void*)pushEvent		},
	{"setAssetManager"								, "(Ljava/lang/Object;)I"		, (void*)setAssetManager		},
	{"setSdcardPath"								, "(Ljava/lang/String;)I"			, (void*)setSdcardPath		},
};

int jniRegisterNativeMethods(JNIEnv* env, const char* className, const JNINativeMethod* gMethods, int numMethods)
{
	jclass clazz;

	LOGI("[JNI]Registering %s natives\n", className);
	clazz = env->FindClass(className);
	if(clazz == NULL) {
		LOGI("[JNI]Native registration unable to find class '%s'\n", className);
		return -1;
	}
	if(env->RegisterNatives(clazz, gMethods, numMethods) < 0) {
		LOGI("[JNI]RegisterNatives failed for '%s'\n", className);
		return -1;
	}

	return 0;
}

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
	JNIEnv* env = NULL;
	jint result = -1;

	if (vm->GetEnv((void**) &env, JNI_VERSION_1_6) != JNI_OK) {
		return result;
	}

	jniRegisterNativeMethods(env, "com/amcop/app/mainActivity", wrMethods, NELEM(wrMethods));

	return JNI_VERSION_1_6;
}

/*
 * Android UI
 */
void showAndroidView()
{
	// Get the android application's activity.
	ANativeActivity* activity = g_app->activity;
	JavaVM* jvm = g_app->activity->vm;

	JNIEnv* env = NULL;
	jvm->GetEnv((void **)&env, JNI_VERSION_1_6);
	jint res = jvm->AttachCurrentThread(&env, NULL);

	if (res == JNI_ERR)
	{
	  // Failed to retrieve JVM environment
		return;
	}

	jclass clazz = env->GetObjectClass(activity->clazz);
	jmethodID methodID = env->GetMethodID(clazz, "showAndroidView", "()V");

	env->CallVoidMethod(activity->clazz, methodID);
	jvm->DetachCurrentThread();
}

void showAndroidPopup(int val, unsigned int objID, GLfloat mx, GLfloat my) {
	// Get the android application's activity.
	ANativeActivity* activity = g_app->activity;
	JavaVM* jvm = g_app->activity->vm;

	JNIEnv* env = NULL;
	jvm->GetEnv((void **)&env, JNI_VERSION_1_6);
	jint res = jvm->AttachCurrentThread(&env, NULL);

	if (res == JNI_ERR)
	{
	  // Failed to retrieve JVM environment
		return;
	}

	jclass clazz = env->GetObjectClass(activity->clazz);
	jmethodID methodID = env->GetMethodID(clazz, "showAndroidPopup", "(IIFF)V");

	env->CallVoidMethod(activity->clazz, methodID, val, objID, mx, my);
	jvm->DetachCurrentThread();
}

int getScreenWidth() {
	// Get the android application's activity.
	ANativeActivity* activity = g_app->activity;
	JavaVM* jvm = g_app->activity->vm;

	JNIEnv* env = NULL;
	jvm->GetEnv((void **)&env, JNI_VERSION_1_6);
	jint res = jvm->AttachCurrentThread(&env, NULL);

	if (res == JNI_ERR)
	{
	  // Failed to retrieve JVM environment
		return -1;
	}

	jclass clazz = env->GetObjectClass(activity->clazz);
	jmethodID methodID = env->GetMethodID(clazz, "getScreenWidth", "()I");

	int retVal = env->CallIntMethod(activity->clazz, methodID);
	jvm->DetachCurrentThread();

	return retVal;
}

int getScreenHeight() {
	// Get the android application's activity.
	ANativeActivity* activity = g_app->activity;
	JavaVM* jvm = g_app->activity->vm;

	JNIEnv* env = NULL;
	jvm->GetEnv((void **)&env, JNI_VERSION_1_6);
	jint res = jvm->AttachCurrentThread(&env, NULL);

	if (res == JNI_ERR)
	{
	  // Failed to retrieve JVM environment
		return -1;
	}

	jclass clazz = env->GetObjectClass(activity->clazz);
	jmethodID methodID = env->GetMethodID(clazz, "getScreenHeight", "()I");

	int retVal = env->CallIntMethod(activity->clazz, methodID);
	jvm->DetachCurrentThread();

	return retVal;
}

/*
 * Global Functions
 */
int g_LoadBitmapFile(unsigned char* &bitmapImage, char *filename, BITMAPINFOHEADER *bitmapInfoHeader)
{
//	LOGI("[LOAD BITMAP FILE : %s]", filename);
	AAsset *asset = AAssetManager_open(g_assetMgr, filename, AASSET_MODE_UNKNOWN);
	if(NULL == asset) {
		LOGI("[LOAD BITMAP FILE FALSE]");
		return -1 ;
	}

	// load bitmap file
	BITMAPFILEHEADER bitmapFileHeader;
//	unsigned char *bitmapImage;  //store image data
	int imageIdx=0;  //image index counter
	unsigned char tempRGB;  //our swap variable

	//read the bitmap file header
	AAsset_read(asset, &bitmapFileHeader, sizeof(BITMAPFILEHEADER));

	//verify that this is a bmp file by check bitmap id
	if (bitmapFileHeader.bfType !=0x4D42)
	{
		AAsset_close(asset);
		return -2;
	}

	//read the bitmap info header
	AAsset_read(asset, bitmapInfoHeader, sizeof(BITMAPINFOHEADER));

	//move file point to the begging of bitmap data
	AAsset_seek(asset, bitmapFileHeader.bfOffBits, SEEK_SET);

	//allocate enough memory for the bitmap image data
	bitmapImage = (unsigned char*)malloc(bitmapInfoHeader->biSizeImage);

	//verify memory allocation
	if (!bitmapImage)
	{
		free(bitmapImage);
		AAsset_close(asset);
		return -3;
	}

	//read in the bitmap image data
	AAsset_read(asset, bitmapImage, bitmapInfoHeader->biSizeImage);

	//make sure bitmap image data was read
	if (bitmapImage == NULL)
	{
		AAsset_close(asset);
		return -4;
	}

	//swap the r and b values to get RGB (bitmap is BGR)
	for (imageIdx = 0;imageIdx < bitmapInfoHeader->biSizeImage;imageIdx+=3)
	{
		tempRGB = bitmapImage[imageIdx];
		bitmapImage[imageIdx] = bitmapImage[imageIdx + 2];
		bitmapImage[imageIdx + 2] = tempRGB;
	}

	//close file and return bitmap iamge data
	AAsset_close(asset);
	return 0;
}

