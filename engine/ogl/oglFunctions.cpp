// =========================================================
//
// oglFunctions.cpp
// view control & draw objects engine (body).
// Created by Wooram Hong 2013/02/06
// This file is part of the AMCOP project.
//
// =========================================================

#include <stdlib.h>

#ifdef IS_ANDROID
#include <EGL/egl.h>
#include <GLES/gl.h>
#include <stdio.h>
#include <jni.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
extern char g_sdcardPath[512];
#else
#include "../../vsProject/StdAfx.h"
#endif


#include <math.h>
#include "gluLookAt.h"
#include "oglFunctions.h"

#define LENGTH (100)
static const GLfloat triangleBuffer[] = {
/*        X                Y          Z */
	LENGTH/2, 	LENGTH/2,
	-LENGTH/2, 	LENGTH/2,
	0, 					0,
	
};

static float colorBuffer[] = {
/*   R    G    B    A  */
	1.0, 0.0, 0.0, 1.0,
	0.0, 1.0, 0.0, 1.0,
	0.0, 0.0, 1.0, 1.0,
	0.5, 0.5, 0.0, 1.0,
};

static const GLfloat cubeVertices[] = {
	-LENGTH / 2, -LENGTH / 2,  LENGTH,
	LENGTH / 2, -LENGTH / 2,  LENGTH,
	-LENGTH / 2,  LENGTH / 2,  LENGTH,
	LENGTH / 2,  LENGTH / 2,  LENGTH,
	-LENGTH / 2, -LENGTH / 2, 0,
	LENGTH / 2, -LENGTH / 2, 0,
	-LENGTH / 2,  LENGTH / 2, 0,
	LENGTH / 2,  LENGTH / 2, 0,
};

static const GLushort cubeIndices[] = {
	0, 1, 2, 3, 7, 1, 5, 4, 7, 6, 2, 4, 0, 1
};

static const GLubyte cubeColors[] = {
	255, 255,   0, 255,
	0,   255, 255, 255,
	0,     0, 255, 255,
	255,   0, 255, 255,
	255, 255,   0, 255,
	0,   255, 255, 255,
	0,     0, 255, 255,
	255,   0, 255, 255
};

static const GLfloat gridLine[] = {
	MAP_BOX_UNIT*2, 0, 0,
	-MAP_BOX_UNIT*2, 0, 0,
	0, MAP_BOX_UNIT, 0,
	0, -MAP_BOX_UNIT, 0,
	0, 0, 0,
	0, 0, 7000000,
};

static const GLfloat selectPosition[] = {
	-25, 0,
	25, 0,
	0, -25,
	0, 25,
};

oglFunctions::oglFunctions()
{
	m_VC = new oglViewContor();

}

oglFunctions::~oglFunctions()
{
	if(!m_objectInfo.empty()) {
		m_objectInfo.clear();
	}

	delete m_VC;
}

void oglFunctions::gluPerspective(double fovy, double aspect, double zNear, double zFar) {
    GLfloat xmin, xmax, ymin, ymax;
    ymax = zNear * tan(fovy * M_PI / 360.0f);
    ymin = -ymax;
    xmin = ymin * aspect;
    xmax = ymax * aspect;
#ifdef IS_ANDROID
    glFrustumf(xmin, xmax, ymin, ymax, zNear, zFar);
#else
	glFrustum(xmin, xmax, ymin, ymax, zNear, zFar);
#endif
}

int oglFunctions::initWorld(int32_t width, int32_t height) {
	/************************************************************************/
	/* 표시를 초기화한다.                                                   */
	/************************************************************************/

	// OPENGL INIT //////////////////////////////////////////////////
    glEnable(GL_NORMALIZE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glShadeModel(GL_SMOOTH);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// OPENGL END ///////////////////////////////////////////////////

	// View Init Start //////////////////////////////////////////////
	m_screenWidth = width;
	m_screenHeight = height;

	glClearColor(0.0f, 0.0f, 0.0f, 1.f);

	float ratio = (float) m_screenWidth / (float) m_screenHeight;
	glViewport(0, 0, (int) m_screenWidth, (int) m_screenHeight);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, ratio, 10.0f, LIMIT_ZOOM_MAX * 1.5F);
	// View Init End ////////////////////////////////////////////////

	return 0;
}

void oglFunctions::draw3DWorld() {
	// 프레임을 표시할 때마다 매번 초기화 한다.

	glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);

	gluLookAt(m_VC->getVP(CURRENT_VIEW)->centerx + m_VC->getVP(CURRENT_VIEW)->eyex, m_VC->getVP(CURRENT_VIEW)->centery + m_VC->getVP(CURRENT_VIEW)->eyey, m_VC->getVP(CURRENT_VIEW)->eyez, m_VC->getVP(CURRENT_VIEW)->centerx, m_VC->getVP(CURRENT_VIEW)->centery, m_VC->getVP(CURRENT_VIEW)->centerz, 0.0f, 0.0f, 1.0f );

	//나중에 지울 부분. 개발 편의를 위해 둔 것임.
    glPushMatrix();
    glLineWidth(3.0f);
    glVertexPointer(3, GL_FLOAT, 0, gridLine);
    glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
    glDrawArrays(GL_LINES, 0, 2);
    glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
	glDrawArrays(GL_LINES, 2, 2);
	glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
	glDrawArrays(GL_LINES, 4, 2);
    glPopMatrix();

	glEnable(GL_DEPTH_TEST);


	// 그리기
    glEnableClientState(GL_COLOR_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, cubeVertices);
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, cubeColors);

    glPushMatrix();
	glTranslatef(340, -170, 0);
	glDrawElements(GL_TRIANGLE_STRIP, 14, GL_UNSIGNED_SHORT, cubeIndices);
	glPopMatrix();

    glPushMatrix();
	glTranslatef(170, -170, 0);
	glDrawElements(GL_TRIANGLE_STRIP, 14, GL_UNSIGNED_SHORT, cubeIndices);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, -170, 0);
	glDrawElements(GL_TRIANGLE_STRIP, 14, GL_UNSIGNED_SHORT, cubeIndices);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, 0, 0);
	glDrawElements(GL_TRIANGLE_STRIP, 14, GL_UNSIGNED_SHORT, cubeIndices);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, 170, 0);
	glDrawElements(GL_TRIANGLE_STRIP, 14, GL_UNSIGNED_SHORT, cubeIndices);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, 340, 0);
	glDrawElements(GL_TRIANGLE_STRIP, 14, GL_UNSIGNED_SHORT, cubeIndices);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(300, 170, 0);
	glDrawElements(GL_TRIANGLE_STRIP, 14, GL_UNSIGNED_SHORT, cubeIndices);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(300, 340, 0);
	glDrawElements(GL_TRIANGLE_STRIP, 14, GL_UNSIGNED_SHORT, cubeIndices);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(12688539, 3747775, 0);
	glDrawElements(GL_TRIANGLE_STRIP, 14, GL_UNSIGNED_SHORT, cubeIndices);
	glPopMatrix();

	glDisableClientState(GL_COLOR_ARRAY);

}

void oglFunctions::draw2DInfo() {
	/************************************************************************/
	/* 2D 로 그리고 싶은거 여기 모여라~~                                    */
	/* 2D 로 보이기 위한 각도 계산											*/
	/************************************************************************/
	
	if(m_VC->getEyeDistance3D(CURRENT_VIEW) > LIMIT_GLOBE_LINE)
		return;

	float view2Dz = (m_VC->getVP(CURRENT_VIEW)->changeEyeTurnAngleDegreeVal) + 90.0f;
	float view2Dx = -(m_VC->getVP(CURRENT_VIEW)->changeEyeTiltAngleDegreeVal) + 90.0f;

	glDisable(GL_DEPTH_TEST);
	glDisableClientState(GL_COLOR_ARRAY);

	glVertexPointer(2, GL_FLOAT, 0, triangleBuffer);

	glPushMatrix();
	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
	glTranslatef(1000, 0, 0);
 	glRotatef(view2Dz, 0, 0, 1);
 	glRotatef(view2Dx, 1, 0, 0);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);
	glPopMatrix();

	glPushMatrix();
	glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
	glTranslatef(0, 0, 400);
	glRotatef(view2Dz, 0, 0, 1);
	glRotatef(view2Dx, 1, 0, 0);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);
	glPopMatrix();

	// 사용자가 지도 클릭한 위치 표시
	glPushMatrix();
	glLineWidth(5.0f);
	glVertexPointer(2, GL_FLOAT, 0, selectPosition);
	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
	glTranslatef(m_targetX, m_targetY, m_targetZ);
	float crossScaleVal = m_VC->getEyeDistance3D(CURRENT_VIEW) / INIT_ZOOM_VAL;
	glScalef(crossScaleVal, crossScaleVal, crossScaleVal);
	glRotatef(view2Dz, 0, 0, 1);
	glRotatef(view2Dx, 1, 0, 0);
	glDrawArrays(GL_LINES, 0, 4);
	glPopMatrix();

}

void oglFunctions::drawPicking3DWorld() { 
	/************************************************************************/
	/* 지도의 좌표나 Object 의 선택을 파악하기 위한 함수                    */
	/*
	 Object 마다 아래와 같이 색을 지정한다.
	 glColor4f((m_drawPickingCount/(256*256))/255.0f, ((m_drawPickingCount%(256*256))/256)/255.0f,(m_drawPickingCount%256)/255.0f, 1.0f);
	 색 지정 후 m_drawPickingCount 값을 1씩 증가시켜 Object 를 구분한다.
	 */
	/************************************************************************/
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if(m_VC->getEyeDistance3D(CURRENT_VIEW) > LIMIT_GLOBE_LINE)
		return;

	m_drawPickingCount = 1; // 0 은 기본색이다. 따라서 1로 시작

	glEnable(GL_DEPTH_TEST);
	glEnableClientState(GL_VERTEX_ARRAY);

	// draw world map
	glPushMatrix();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	
	// 2D map
	// 클릭지점 좌표를 파악하기 위한 그리드
	OBJINFO insertInfo;
	m_objectInfo.clear();
	float gridGap = m_VC->getEyeDistance3D(CURRENT_VIEW) / MAP_GRIDGAP;
	for(int i = -MAP_GRIDCOUNT; i <= MAP_GRIDCOUNT; i++) {
		for(int j = -MAP_GRIDCOUNT; j <= MAP_GRIDCOUNT; j++) {
			GLfloat moveGapX = 2 * gridGap * j;
			GLfloat moveGapY = 2 * gridGap * i;

			GLfloat gridVertices[] = {
				m_VC->getVP(CURRENT_VIEW)->centerx + moveGapX - gridGap, m_VC->getVP(CURRENT_VIEW)->centery + moveGapY + gridGap, 0,
				m_VC->getVP(CURRENT_VIEW)->centerx + moveGapX - gridGap, m_VC->getVP(CURRENT_VIEW)->centery + moveGapY - gridGap, 0,
				m_VC->getVP(CURRENT_VIEW)->centerx + moveGapX + gridGap, m_VC->getVP(CURRENT_VIEW)->centery + moveGapY + gridGap, 0,

				m_VC->getVP(CURRENT_VIEW)->centerx + moveGapX + gridGap, m_VC->getVP(CURRENT_VIEW)->centery + moveGapY - gridGap, 0,
				m_VC->getVP(CURRENT_VIEW)->centerx + moveGapX + gridGap, m_VC->getVP(CURRENT_VIEW)->centery + moveGapY + gridGap, 0,
				m_VC->getVP(CURRENT_VIEW)->centerx + moveGapX - gridGap, m_VC->getVP(CURRENT_VIEW)->centery + moveGapY - gridGap, 0,
			};
			glVertexPointer(3, GL_FLOAT, 0, gridVertices);
			glColor4f((m_drawPickingCount/(256*256))/255.0f, ((m_drawPickingCount%(256*256))/256)/255.0f,(m_drawPickingCount%256)/255.0f, 1.0f);
			glDrawArrays(GL_TRIANGLES, 0, 6);

			insertInfo.index = m_drawPickingCount;
			insertInfo.objID = 0;
			insertInfo.x = m_VC->getVP(CURRENT_VIEW)->centerx + moveGapX;
			insertInfo.y = m_VC->getVP(CURRENT_VIEW)->centery + moveGapY;
			insertInfo.z = 0;

			m_objectInfo.push_back(insertInfo);
			m_drawPickingCount++;
		}
	}
	glPopMatrix();

	glVertexPointer(3, GL_FLOAT, 0, cubeVertices);

	// 그리기
	glPushMatrix();
	glTranslatef(340, -170, 0);
 	glColor4f((m_drawPickingCount/(256*256))/255.0f, ((m_drawPickingCount%(256*256))/256)/255.0f,(m_drawPickingCount%256)/255.0f, 1.0f);
	glDrawElements(GL_TRIANGLE_STRIP, 14, GL_UNSIGNED_SHORT, cubeIndices);
	glPopMatrix();

	insertInfo.index = m_drawPickingCount;
	insertInfo.objID = 0;
	insertInfo.x = 340;
	insertInfo.y = -170;
	insertInfo.z = LENGTH;

	m_objectInfo.push_back(insertInfo);
	m_drawPickingCount++;

	glPushMatrix();
	glTranslatef(170, -170, 0);
	glColor4f((m_drawPickingCount/(256*256))/255.0f, ((m_drawPickingCount%(256*256))/256)/255.0f,(m_drawPickingCount%256)/255.0f, 1.0f);
	glDrawElements(GL_TRIANGLE_STRIP, 14, GL_UNSIGNED_SHORT, cubeIndices);
	glPopMatrix();

	insertInfo.index = m_drawPickingCount;
	insertInfo.objID = 0;
	insertInfo.x = 170;
	insertInfo.y = -170;
	insertInfo.z = LENGTH;

	m_objectInfo.push_back(insertInfo);
	m_drawPickingCount++;

	glPushMatrix();
	glTranslatef(0, -170, 0);
	glColor4f((m_drawPickingCount/(256*256))/255.0f, ((m_drawPickingCount%(256*256))/256)/255.0f,(m_drawPickingCount%256)/255.0f, 1.0f);
	glDrawElements(GL_TRIANGLE_STRIP, 14, GL_UNSIGNED_SHORT, cubeIndices);
	glPopMatrix();
 
	insertInfo.index = m_drawPickingCount;
	insertInfo.objID = 0;
	insertInfo.x = 0;
	insertInfo.y = -170;
	insertInfo.z = LENGTH;

	m_objectInfo.push_back(insertInfo);
	m_drawPickingCount++;

	glPushMatrix();
	glTranslatef(0, 0, 0);
	glColor4f((m_drawPickingCount/(256*256))/255.0f, ((m_drawPickingCount%(256*256))/256)/255.0f,(m_drawPickingCount%256)/255.0f, 1.0f);
	glDrawElements(GL_TRIANGLE_STRIP, 14, GL_UNSIGNED_SHORT, cubeIndices);
	glPopMatrix();

	insertInfo.index = m_drawPickingCount;
	insertInfo.objID = 0;
	insertInfo.x = 0;
	insertInfo.y = 0;
	insertInfo.z = LENGTH;

	m_objectInfo.push_back(insertInfo);
	m_drawPickingCount++;

	glPushMatrix();
	glTranslatef(0, 170, 0);
	glColor4f((m_drawPickingCount/(256*256))/255.0f, ((m_drawPickingCount%(256*256))/256)/255.0f,(m_drawPickingCount%256)/255.0f, 1.0f);
	glDrawElements(GL_TRIANGLE_STRIP, 14, GL_UNSIGNED_SHORT, cubeIndices);
	glPopMatrix();

	insertInfo.index = m_drawPickingCount;
	insertInfo.objID = 0;
	insertInfo.x = 0;
	insertInfo.y = 170;
	insertInfo.z = LENGTH;

	m_objectInfo.push_back(insertInfo);
	m_drawPickingCount++;

	glPushMatrix();
	glTranslatef(0, 340, 0);
	glColor4f((m_drawPickingCount/(256*256))/255.0f, ((m_drawPickingCount%(256*256))/256)/255.0f,(m_drawPickingCount%256)/255.0f, 1.0f);
	glDrawElements(GL_TRIANGLE_STRIP, 14, GL_UNSIGNED_SHORT, cubeIndices);
	glPopMatrix();

	insertInfo.index = m_drawPickingCount;
	insertInfo.objID = 0;
	insertInfo.x = 0;
	insertInfo.y = 340;
	insertInfo.z = LENGTH;

	m_objectInfo.push_back(insertInfo);
	m_drawPickingCount++;

	glPushMatrix();
	glTranslatef(300, 170, 0);
	glColor4f((m_drawPickingCount/(256*256))/255.0f, ((m_drawPickingCount%(256*256))/256)/255.0f,(m_drawPickingCount%256)/255.0f, 1.0f);
	glDrawElements(GL_TRIANGLE_STRIP, 14, GL_UNSIGNED_SHORT, cubeIndices);
	glPopMatrix();

	insertInfo.index = m_drawPickingCount;
	insertInfo.objID = 0;
	insertInfo.x = 300;
	insertInfo.y = 170;
	insertInfo.z = LENGTH;

	m_objectInfo.push_back(insertInfo);
	m_drawPickingCount++;

	glPushMatrix();
	glTranslatef(300, 340, 0);
 	glColor4f((m_drawPickingCount/(256*256))/255.0f, ((m_drawPickingCount%(256*256))/256)/255.0f,(m_drawPickingCount%256)/255.0f, 1.0f);
	glDrawElements(GL_TRIANGLE_STRIP, 14, GL_UNSIGNED_SHORT, cubeIndices);
	glPopMatrix();

	insertInfo.index = m_drawPickingCount;
	insertInfo.objID = 0;
	insertInfo.x = 300;
	insertInfo.y = 340;
	insertInfo.z = LENGTH;

	m_objectInfo.push_back(insertInfo);
	m_drawPickingCount++;

	glPushMatrix();
	glTranslatef(12688539, 3747775, 0);
	glColor4f((m_drawPickingCount/(256*256))/255.0f, ((m_drawPickingCount%(256*256))/256)/255.0f,(m_drawPickingCount%256)/255.0f, 1.0f);
	glDrawElements(GL_TRIANGLE_STRIP, 14, GL_UNSIGNED_SHORT, cubeIndices);
	glPopMatrix();

	insertInfo.index = m_drawPickingCount;
	insertInfo.objID = 0;
	insertInfo.x = 12688539;
	insertInfo.y = 3747775;
	insertInfo.z = LENGTH;

	m_objectInfo.push_back(insertInfo);
	m_drawPickingCount++;

}

void oglFunctions::drawPicking2DInfo() { 
	/************************************************************************/
	/* Object 의 선택을 파악하기 위한 함수									*/
	/* POI 등의 선택을 파악하기 위한 함수									*/
	/* 2D 로 보이기 위한 각도 계산											*/
	/************************************************************************/

	if(m_VC->getEyeDistance3D(CURRENT_VIEW) > LIMIT_GLOBE_LINE)
		return;

	OBJINFO insertInfo;

	float view2Dz = (m_VC->getVP(CURRENT_VIEW)->changeEyeTurnAngleDegreeVal) + 90.0f;
	float view2Dx = -(m_VC->getVP(CURRENT_VIEW)->changeEyeTiltAngleDegreeVal) + 90.0f;

	glDisable(GL_DEPTH_TEST);

	glVertexPointer(2, GL_FLOAT, 0, triangleBuffer);

	glPushMatrix();
	glColor4f((m_drawPickingCount/(256*256))/255.0f, ((m_drawPickingCount%(256*256))/256)/255.0f,(m_drawPickingCount%256)/255.0f, 1.0f);
	glTranslatef(1000, 0, 0);
	glRotatef(view2Dz, 0, 0, 1);
	glRotatef(view2Dx, 1, 0, 0);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);
	glPopMatrix();

	insertInfo.index = m_drawPickingCount;
	insertInfo.objID = 0;
	insertInfo.x = 1000;
	insertInfo.y = 0;
	insertInfo.z = 0;

	m_objectInfo.push_back(insertInfo);
	m_drawPickingCount++;

	glPushMatrix();
	glColor4f((m_drawPickingCount/(256*256))/255.0f, ((m_drawPickingCount%(256*256))/256)/255.0f,(m_drawPickingCount%256)/255.0f, 1.0f);
	glTranslatef(0, 0, 400);
	glRotatef(view2Dz, 0, 0, 1);
	glRotatef(view2Dx, 1, 0, 0);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);
	glPopMatrix();

	insertInfo.index = m_drawPickingCount;
	insertInfo.objID = 0;
	insertInfo.x = 0;
	insertInfo.y = 0;
	insertInfo.z = 400;

	m_objectInfo.push_back(insertInfo);
	m_drawPickingCount++;
}

unsigned int oglFunctions::getObjInfo(int sx, int sy, unsigned int objID, GLfloat &mx, GLfloat &my) {
	
	GLubyte pixel[4];
	glReadPixels(sx, m_screenHeight-sy,1,1,GL_RGBA,GL_UNSIGNED_BYTE,(void *)pixel);

	int R = (int)pixel[0];
	int G = (int)pixel[1];
	int B = (int)pixel[2];
	int A = (int)pixel[3];

#if IS_ANDROID
	LOGI("[%4d, %4d] %03d %03d %03d %03d ::\n", sx, sy, R, G, B, A);
#else
	printf("[%4d, %4d] %03d %03d %03d %03d ::\n", sx, sy, R, G, B, A);
#endif
	
	unsigned int retVal = (R*256*256) + (G*256) + B;

	if(retVal == 0)
		return retVal;

	if(!m_objectInfo.empty()) {
		std::vector<OBJINFO>::iterator gridIdx;
		for(gridIdx = m_objectInfo.begin(); gridIdx != m_objectInfo.end(); ++gridIdx) {
			if(gridIdx->index == retVal) {
#if IS_ANDROID
				LOGI(">>> [%d][%d], %f, %f\n", gridIdx->index, gridIdx->objID, gridIdx->x, gridIdx->y);
#else
				printf(">>> [%d][%d], %f, %f\n", gridIdx->index, gridIdx->objID, gridIdx->x, gridIdx->y);
#endif
				objID = gridIdx->objID;
				m_targetX = mx = gridIdx->x;
				m_targetY = my = gridIdx->y;
				m_targetZ = gridIdx->z;

				// 클릭하면 지도 이동
				m_VC->setViewCenter(TARGET_VIEW, m_targetX, m_targetY);

				break;
			}
		}
	}
	
	return retVal;
}
