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
	PET_VIEW_EYE_MOVE_RIGHT = 0, 			// View center 중심으로 eye 를 오른쪽으로 회전
	PET_VIEW_EYE_MOVE_LEFT,					// View center 중심으로 eye 를 왼쪽으로 회전
	PET_VIEW_EYE_MOVE_UP,					// View center 중심으로 eye 를 위로 회전
	PET_VIEW_EYE_MOVE_DOWN,				// View center 중심으로 eye 를 아래로 회전
	PET_VIEW_CENTER_MOVE_RIGHT,			// View center 중심을 center to eye 연장선의 90도 방향으로 (x, y axis 값만) 이동, eye 도 같이 이동.
	PET_VIEW_CENTER_MOVE_LEFT,			// View center 중심을 center to eye 연장선의 -90도 방향으로 (x, y axis 값만) 이동, eye 도 같이 이동.
	PET_VIEW_CENTER_MOVE_FRONT_SIDE,	// View center 중심을 center to eye 연장선의 0도 방향으로 (x, y axis 값만) 이동, eye 도 같이 이동.
	PET_VIEW_CENTER_MOVE_BACK_SIDE,	// View center 중심을 center to eye 연장선의 180도 방향으로 (x, y axis 값만) 이동, eye 도 같이 이동.
	PET_VIEW_ZOOM_IN,							// View center to eye 연장선 위에서 이동. center 와 eye 의 거리를 줄인다.
	PET_VIEW_ZOOM_OUT,						// View center to eye 연장선 위에서 이동. center 와 eye 의 거리를 늘인다.
	PET_VIEW_SET_EYE_LR,						// View center 중심으로 eye 위치의 각도 지정.
	PET_VIEW_SET_EYE_UD,						// View center 중심으로 eye 높이 각도 지정.
	PET_VIEW_SET_EYE_TOP,						// View center 중심으로 eye 가 center 위에서 본다.
	PET_VIEW_SET_CENTER,						// View center 지정.
	PET_VIEW_SET_ZOOM,						// View zoom 지정.
} ;

// touch control values
enum {
	TOUCH_INIT = 0,
	TOUCH_POPUP = 1,				// 팝업 확인용
	TOUCH_FLAG = 3,					// 터치 튐 방지
	TOUCH_ACTION_COUNT = 5,	// 다른 액션 방지
	TOUCH_TICK_COUNT = 30,		// 스크린 클릭 확인용
};

// map control values
enum {
	INIT_ZOOM_VAL		= 1500,
	MAP_ZOOM_UNIT		= 200,
	LIMIT_ZOOM_MIN		= 100,
	LIMIT_GLOBE_LINE	= 2800000,
	LIMIT_ZOOM_MAX		= 6000000,
	LIMIT_TILT_MIN		= 10,
	LIMIT_TILT_MAX		= 89,
	GLOBE_SIZE			= 1900000,
	MAP_EXPANSION		= 100000,
	MAP_BOX_UNIT		= 90 * MAP_EXPANSION,
	MAP_GRIDGAP			= 90,
	MAP_GRIDCOUNT		= 30,
	MAP_SMOOTHMOVE_GAP	= 15,
	MAP_INERTIA_UNIT   	= 2000,
	MAP_INERTIA_VALUE 	= 2,
	TIME_DIV_VALUE 		= 1000000000,
	MAP_TILT_SPEED		= 10,
};

// view type
enum {
	CURRENT_VIEW	=	0,
	TARGET_VIEW		=	1,
	CHANGE_VIEW		=	2,
	SET_VIEW		=	3,
};


#endif
