#ifndef __MAIN_H__
#define __MAIN_H__

#include "../engine/include/types.h"
#include "../engine/ogl/oglFunctions.h"

struct android_app *g_app;

typedef struct _saved_state
{
	ViewPoint vP;
} saved_state2;

TouchCheck touchCheck[2];
TouchPoint oldPoint[POINT_MAX];
TouchGesture TG;

oglFunctions OGL;

AAssetManager *g_assetMgr;
JNIEnv* g_env;

/**
 * Shared state for our app.
 */
struct engine
{
    struct android_app* app;

    ASensorManager* sensorManager;
    const ASensor* accelerometerSensor;
    const ASensor* gyroscopeSensor;
    ASensorEventQueue* sensorEventQueue;

    int animating;
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
    int32_t width;
    int32_t height;    saved_state2 state;
};

int getScreenWidth();
int getScreenHeight();

void showAndroidView();
void showAndroidPopup(int val, unsigned int objID, GLfloat mx, GLfloat my);

#endif
