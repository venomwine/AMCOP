// =========================================================
//
// type.h
// defines & constants module
// Created by Wooram Hong 2013/02/06
// This file is part of the AMCOP project.
//
// =========================================================

#ifndef __TYPES_H__
#define __TYPES_H__

#if IS_ANDROID
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <math.h>
#include <EGL/egl.h>
#include <GLES/gl.h>
#include <time.h>
#include <android/sensor.h>
#include <android/log.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android_native_app_glue.h>
#include <assert.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "native-activity", __VA_ARGS__))

typedef unsigned long       DWORD;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;

typedef char CHAR;
typedef short SHORT;
typedef long LONG;
typedef unsigned long           ULONG;

#pragma pack(push)
#pragma pack(1)

typedef struct tagBITMAPINFOHEADER
{
	DWORD biSize;			//specifies the number of bytes required by the struct
	LONG biWidth;			//specifies width in pixels
	LONG biHeight;			//species height in pixels
	WORD biPlanes;			//specifies the number of color planes, must be 1
	WORD biBitCount;		//specifies the number of bit per pixel
	DWORD biCompression;	//spcifies the type of compression
	DWORD biSizeImage;		//size of image in bytes
	LONG biXPelsPerMeter;	//number of pixels per meter in x axis
	LONG biYPelsPerMeter;	//number of pixels per meter in y axis
	DWORD biClrUsed;		//number of colors used by th ebitmap
	DWORD biClrImportant;	//number of colors that are important
}BITMAPINFOHEADER;

typedef struct tagBITMAPFILEHEADER
{
	WORD bfType;  //specifies the file type
	DWORD bfSize;  //specifies the size in bytes of the bitmap file
	WORD bfReserved1;  //reserved; must be 0
	WORD bfReserved2;  //reserved; must be 0
	DWORD bfOffBits;  //species the offset in bytes from the bitmapfileheader to the bitmap bits
}BITMAPFILEHEADER;

#pragma pack(pop)

int g_fdOpen(const char* filename);
int g_fdRead(int fd, void *buff, int size);
int g_fdSeek(int fd, int offset, int type);
int g_fdClose(int fd);

AAsset *g_aOpen(const char* filename);
int g_aRead(AAsset* asset, void *buff, int size, int num);
void g_aSeek(AAsset* asset, int offset, int type);
off_t g_aTell(AAsset* asset);
void g_aClose(AAsset* asset);

int g_LoadBitmapFile(unsigned char* &bitmapImage, char *filename, BITMAPINFOHEADER *bitmapInfoHeader);
size_t g_LoadFTFile( char *filename, unsigned char* &ftBuffer);

#else
#define LOGI(...) printf(__VA_ARGS__);
#define LOGW(...) printf(__VA_ARGS__);
#endif

#include <vector>

#define POINT_MAX (5)
#define TRUE (1)
#define FALSE (0)

#ifndef NELEM
#define NELEM(x) ((int) (sizeof(x) / sizeof((x)[0])))
#endif

#ifndef int32_t
#define int32_t int
#endif

#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif

typedef struct _TouchPoint{
	int32_t x;
	int32_t y;
} TouchPoint;

typedef struct _CheckTouch{
	TouchPoint point;
#if IS_ANDROID
	timespec time;
#else
	DWORD	time;
#endif
} TouchCheck;

typedef struct _TouchGesture {
	int32_t flag;
	int32_t action;
	int32_t count;
	int32_t click;
	int32_t popup;
} TouchGesture;

struct OBJINFO {
	unsigned int index;
	unsigned int objID;
	GLfloat x;
	GLfloat y;
	GLfloat z;
};

// push event type
enum {
	PET_VIEW_EYE_MOVE_RIGHT = 0,
	PET_VIEW_EYE_MOVE_LEFT,
	PET_VIEW_EYE_MOVE_UP,
	PET_VIEW_EYE_MOVE_DOWN,
	PET_VIEW_CENTER_MOVE_RIGHT,
	PET_VIEW_CENTER_MOVE_LEFT,
	PET_VIEW_CENTER_MOVE_FRONT_SIDE,
	PET_VIEW_CENTER_MOVE_BACK_SIDE,
	PET_VIEW_ZOOM_IN,
	PET_VIEW_ZOOM_OUT,
	PET_VIEW_SET_EYE_LR,
	PET_VIEW_SET_EYE_UD,
	PET_VIEW_SET_EYE_TOP,
	PET_VIEW_SET_CENTER,
	PET_VIEW_SET_ZOOM,
} ;

// touch control values
enum {
	TOUCH_INIT = 0,
	TOUCH_POPUP = 1,
	TOUCH_FLAG = 3,
	TOUCH_ACTION_COUNT = 5,
	TOUCH_TICK_COUNT = 30,
};

// map control values
enum {
	INIT_ZOOM_VAL		= 1500,
	MAP_ZOOM_UNIT		= 200,
	LIMIT_ZOOM_MIN		= 100,
	LIMIT_ZOOM_MAX		= 6000000,
	LIMIT_TILT_MIN		= 10,
	LIMIT_TILT_MAX		= 89,
	MAP_EXPANSION		= 100,
	MAP_BOX_UNIT		= 90 * MAP_EXPANSION,
	MAP_GRIDGAP			= 90,
	MAP_GRIDCOUNT		= 30,
	MAP_SMOOTHMOVE_GAP	= 15,
	MAP_INERTIA_UNIT   	= 2000,
	MAP_INERTIA_VALUE 	= 2,
	TIME_DIV_VALUE 		= 1000000000,
};

// view type
enum {
	CURRENT_VIEW	=	0,
	TARGET_VIEW		=	1,
	CHANGE_VIEW		=	2,
	SET_VIEW		=	3,
};


#endif
