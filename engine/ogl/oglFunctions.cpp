#include <stdlib.h>

#ifdef IS_ANDROID
#include <EGL/egl.h>
#include <GLES/gl.h>
#include <stdio.h>
#include <jni.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#else
#include "../../vsProject/StdAfx.h"
#endif


#include <math.h>
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
	m_pTargetVP = (ViewPoint *)malloc(sizeof(ViewPoint));
	memset(m_pTargetVP, 0x00, sizeof(ViewPoint));
	m_pTargetVP->changeEyeTurnAngleVal = -M_PI/2;	// North up (Y axis up)
	m_pTargetVP->changeEyeTiltAngleVal = M_PI/2;		// 위에서 내려다보기
	m_pTargetVP->eyez = m_pTargetVP->zoomingVal = INIT_ZOOM_VAL;
	m_pTargetVP->uniEyez = GLOBE_SIZE + INIT_ZOOM_VAL;
	m_pTargetVP->upz = 1.0f;

	m_pVP = (ViewPoint *)malloc(sizeof(ViewPoint));
	memset(m_pVP, 0x00, sizeof(ViewPoint));
	m_pVP->changeEyeTurnAngleVal = -M_PI/2;	// North up (Y axis up)
	m_pVP->changeEyeTiltAngleVal = M_PI/2;		// 위에서 내려다보기
	m_pVP->eyez = m_pVP->zoomingVal = INIT_ZOOM_VAL;
	m_pVP->uniEyez = GLOBE_SIZE + INIT_ZOOM_VAL;
	m_pVP->upz = 1.0f;

}

oglFunctions::~oglFunctions()
{
	free(m_pTargetVP);
	free(m_pVP);

	if(!m_objectInfo.empty()) {
		m_objectInfo.clear();
	}
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

void oglFunctions::gluLookAt(GLfloat eyex, GLfloat eyey, GLfloat eyez,
          GLfloat centerx, GLfloat centery, GLfloat centerz,
          GLfloat upx, GLfloat upy, GLfloat upz)
{
/*
 *  gluLookAt.h
 *
 *  This is a modified version of the function of the same name from 
 *  the Mesa3D project ( http://mesa3d.org/ ), which is  licensed
 *  under the MIT license, which allows use, modification, and 
 *  redistribution
 *
 *  In order to work under OpenGL ES, all instances of GLdouble
 *  had to be changed to GLfloat, and all "d" function calls had
 *  to be changed to the "f" versions.
 *
 *  Original developer's comments have been left in place.
 *
 *  Out of respect for the original authors, this is licensed under
 *  the Mesa (MIT) license. Original license follows:
 *  
 *  -----------------------------------------------------------------------
 *
 *  Copyright (C) 1999-2007  Brian Paul   All Rights Reserved.
 *  
 *  Permission is hereby granted, free of charge, to any person obtaining a
 *  copy of this software and associated documentation files (the "Software"),
 *  to deal in the Software without restriction, including without limitation
 *  the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *  and/or sell copies of the Software, and to permit persons to whom the
 *  Software is furnished to do so, subject to the following conditions:
 *  
 *  The above copyright notice and this permission notice shall be included
 *  in all copies or substantial portions of the Software.
 
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 *  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 *  BRIAN PAUL BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 *  AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 *  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */
    GLfloat m[16];
    GLfloat x[3], y[3], z[3];
    GLfloat mag;

    /* Make rotation matrix */

    /* Z vector */
    z[0] = eyex - centerx;
    z[1] = eyey - centery;
    z[2] = eyez - centerz;
    mag = sqrt(z[0] * z[0] + z[1] * z[1] + z[2] * z[2]);
    if (mag) {          /* mpichler, 19950515 */
        z[0] /= mag;
        z[1] /= mag;
        z[2] /= mag;
    }

    /* Y vector */
    y[0] = upx;
    y[1] = upy;
    y[2] = upz;

    /* X vector = Y cross Z */
    x[0] = y[1] * z[2] - y[2] * z[1];
    x[1] = -y[0] * z[2] + y[2] * z[0];
    x[2] = y[0] * z[1] - y[1] * z[0];

    /* Recompute Y = Z cross X */
    y[0] = z[1] * x[2] - z[2] * x[1];
    y[1] = -z[0] * x[2] + z[2] * x[0];
    y[2] = z[0] * x[1] - z[1] * x[0];

    /* mpichler, 19950515 */
    /* cross product gives area of parallelogram, which is < 1.0 for
     * non-perpendicular unit-length vectors; so normalize x, y here
     */

    mag = sqrt(x[0] * x[0] + x[1] * x[1] + x[2] * x[2]);
    if (mag) {
        x[0] /= mag;
        x[1] /= mag;
        x[2] /= mag;
    }

    mag = sqrt(y[0] * y[0] + y[1] * y[1] + y[2] * y[2]);
    if (mag) {
        y[0] /= mag;
        y[1] /= mag;
        y[2] /= mag;
    }

#define M(row,col)  m[col*4+row]
    M(0, 0) = x[0];
    M(0, 1) = x[1];
    M(0, 2) = x[2];
    M(0, 3) = 0.0;
    M(1, 0) = y[0];
    M(1, 1) = y[1];
    M(1, 2) = y[2];
    M(1, 3) = 0.0;
    M(2, 0) = z[0];
    M(2, 1) = z[1];
    M(2, 2) = z[2];
    M(2, 3) = 0.0;
    M(3, 0) = 0.0;
    M(3, 1) = 0.0;
    M(3, 2) = 0.0;
    M(3, 3) = 1.0;
#undef M
    glMultMatrixf(m);

    /* Translate Eye to Origin */
    glTranslatef(-eyex, -eyey, -eyez);

}

float oglFunctions::getEyeDistance3D() {
	return m_pTargetVP->zoomingVal;
}

float oglFunctions::getEyeDistance2D() {
	float distance = pow((m_pTargetVP->eyex - m_pTargetVP->centerx), 2)
			+ pow((m_pTargetVP->eyey - m_pTargetVP->centery), 2);

	distance = sqrt(distance);
	return distance;
}

float oglFunctions::getTanDegree(float x, float y) {
	float tanVal = y / x;
	float moveDegree = 0;

	// tan 연산 시 각도가 90도 단위면 문제가 된다.
	if(x == 0)
		moveDegree = y > 0 ? 0 : 180;
	else if(y == 0)
		moveDegree = x > 0 ? -90 : 90;
	else {
		moveDegree = (atan(tanVal) * 180.0f / M_PI) ;
		if(x > 0) {
			if(y < 0) 			// 1사분면
				moveDegree -= 90;
			else if(y > 0)	// 4 사분면
				moveDegree -= 90;
		} else if(x < 0) {
			if(y < 0) 			// 2 사분면
				moveDegree += 90;
			else if(y > 0) 	// 3 사분면
				moveDegree += 90;
		}
	}
	return moveDegree;
}

void oglFunctions::changeEyeTurnAngle(float degree) {
	float eyeDistance = getEyeDistance2D();

	m_pTargetVP->changeEyeTurnAngleVal += (degree * (M_PI / 180.0f));

	m_pTargetVP->eyex = eyeDistance * cos(m_pTargetVP->changeEyeTurnAngleVal) + m_pTargetVP->centerx;
	m_pTargetVP->eyey = eyeDistance * sin(m_pTargetVP->changeEyeTurnAngleVal) + m_pTargetVP->centery;
}

void oglFunctions::changeEyeTiltAngle(float degree) {
	float eyeDistance3D = getEyeDistance3D();
	float eyeDistance2D = 0;

	m_pTargetVP->changeEyeTiltAngleVal += (degree * (M_PI / 180.0f));
	if(m_pTargetVP->changeEyeTiltAngleVal < (M_PI / 180.0f * 10.0f))
		m_pTargetVP->changeEyeTiltAngleVal = (M_PI / 180.0f * 10.0f);
	else if(m_pTargetVP->changeEyeTiltAngleVal > (M_PI / 180.0f * 89.0f))
		m_pTargetVP->changeEyeTiltAngleVal = (M_PI / 180.0f * 89.0f);

	m_pTargetVP->eyez = eyeDistance3D * sin(m_pTargetVP->changeEyeTiltAngleVal) + m_pTargetVP->centerz;
	eyeDistance2D = eyeDistance3D * cos(m_pTargetVP->changeEyeTiltAngleVal) + m_pTargetVP->centerz;

	m_pTargetVP->eyex = eyeDistance2D * cos(m_pTargetVP->changeEyeTurnAngleVal) + m_pTargetVP->centerx;
	m_pTargetVP->eyey = eyeDistance2D * sin(m_pTargetVP->changeEyeTurnAngleVal) + m_pTargetVP->centery;
}

void oglFunctions::changeViewCenter(float degree, float val) {

	float moveEyeAngleVal = (m_pTargetVP->changeEyeTurnAngleVal + M_PI) + (-degree * (M_PI / 180.0f));

	// 지구본에서는 움직임을 빠르게
	if(getEyeDistance3D() > LIMIT_GLOBE_LINE) {
		setEyeTurnAngle(0);
		setEyeTiltAngle(90);
		val = val * getEyeDistance3D() / LIMIT_GLOBE_LINE * 1.5f;
	}

	float tmpCy = m_pTargetVP->centery + val * sin(moveEyeAngleVal);

	// 극지방으로는 지도 이동에 제한을 둔다.
	if(!(tmpCy > MAP_BOX_UNIT-(15*MAP_EXPANSION) || tmpCy < -(MAP_BOX_UNIT-(15*MAP_EXPANSION)))) {
		m_pTargetVP->eyex += val * cos(moveEyeAngleVal);
		m_pTargetVP->eyey += val * sin(moveEyeAngleVal);

		m_pTargetVP->centerx = m_pTargetVP->centerx + val * cos(moveEyeAngleVal);
		m_pTargetVP->centery = tmpCy;
	}

	// Repeat map moving. 경도로는 지도가 반복하여 나타난다.
	// 2D Map 과 Globe 모두 적용되므로 순서 바꾸지 말 것.
	if(m_pTargetVP->centerx > (MAP_BOX_UNIT*2)) {
		GLfloat xGap = m_pTargetVP->eyex - m_pTargetVP->centerx;

		m_pVP->centerx = -(MAP_BOX_UNIT*4) + m_pVP->centerx;
		m_pVP->eyex = m_pVP->centerx + xGap;

		m_pTargetVP->centerx = -(MAP_BOX_UNIT*4) + m_pTargetVP->centerx;
		m_pTargetVP->eyex = m_pTargetVP->centerx + xGap;

	} else if(m_pTargetVP->centerx < -(MAP_BOX_UNIT*2)) {
		GLfloat xGap = m_pTargetVP->eyex - m_pTargetVP->centerx;

		m_pVP->centerx = (MAP_BOX_UNIT*4) + m_pVP->centerx;
		m_pVP->eyex = m_pVP->centerx + xGap;

		m_pTargetVP->centerx = (MAP_BOX_UNIT*4) + m_pTargetVP->centerx;
		m_pTargetVP->eyex = m_pTargetVP->centerx + xGap;
	}

}

void oglFunctions::changeEyeDistance(float val) {
	float eyeDistance2D = 0;

	m_pTargetVP->zoomingVal += val;
	if(m_pTargetVP->zoomingVal <= LIMIT_ZOOM_MIN)				// 확대 제한
		m_pTargetVP->zoomingVal = LIMIT_ZOOM_MIN;
	else if(m_pTargetVP->zoomingVal >= LIMIT_ZOOM_MAX)		// 축소 제한
		m_pTargetVP->zoomingVal = LIMIT_ZOOM_MAX;

	if(m_pTargetVP->zoomingVal < LIMIT_GLOBE_LINE) {
		// 2D map
		m_pTargetVP->eyez = m_pTargetVP->zoomingVal * sin(m_pTargetVP->changeEyeTiltAngleVal) + m_pTargetVP->centerz;
		eyeDistance2D = m_pTargetVP->zoomingVal * cos(m_pTargetVP->changeEyeTiltAngleVal) + m_pTargetVP->centerz;

		m_pTargetVP->eyex = eyeDistance2D * cos(m_pTargetVP->changeEyeTurnAngleVal) + m_pTargetVP->centerx;
		m_pTargetVP->eyey = eyeDistance2D * sin(m_pTargetVP->changeEyeTurnAngleVal) + m_pTargetVP->centery;
	} else {
		// Globe
		float ratateVal = (m_pTargetVP->centerx/MAP_EXPANSION) * (M_PI / 180.0f);
		float tiltVal = (m_pTargetVP->centery/MAP_EXPANSION) * (M_PI / 180.0f);

		if(tiltVal < (M_PI / 180.0f * -89.0f))
			tiltVal = (M_PI / 180.0f * -89.0f);
		else if(tiltVal > (M_PI / 180.0f * 89.0f))
			tiltVal = (M_PI / 180.0f * 89.0f);

		m_pTargetVP->uniEyez = m_pTargetVP->zoomingVal * sin(tiltVal);
		eyeDistance2D = m_pTargetVP->zoomingVal * cos(tiltVal);

		m_pTargetVP->uniEyex = eyeDistance2D * cos(ratateVal);
		m_pTargetVP->uniEyey = eyeDistance2D * sin(ratateVal);
	}
}

void oglFunctions::setEyeTurnAngle(int degree) {
	float eyeDistance = getEyeDistance2D();

	m_pTargetVP->changeEyeTurnAngleVal = ((float)(degree-90.0f) * (M_PI / 180.0f));	// 90 을 뺀 이유는 북쪽을 0도로 생각하기 위함임.

	m_pTargetVP->eyex = eyeDistance * cos(m_pTargetVP->changeEyeTurnAngleVal) + m_pTargetVP->centerx;
	m_pTargetVP->eyey = eyeDistance * sin(m_pTargetVP->changeEyeTurnAngleVal) + m_pTargetVP->centery;
}

void oglFunctions::setEyeTiltAngle(int degree) {
	float eyeDistance3D = getEyeDistance3D();
	float eyeDistance2D = 0;

	m_pTargetVP->changeEyeTiltAngleVal = (degree * (M_PI / 180.0f));
	if(m_pTargetVP->changeEyeTiltAngleVal < (M_PI / 180.0f * 10.0f))
		m_pTargetVP->changeEyeTiltAngleVal = (M_PI / 180.0f * 10.0f);
	else if(m_pTargetVP->changeEyeTiltAngleVal > (M_PI / 180.0f * 89.0f))
		m_pTargetVP->changeEyeTiltAngleVal = (M_PI / 180.0f * 89.0f);

	m_pTargetVP->eyez = eyeDistance3D * sin(m_pTargetVP->changeEyeTiltAngleVal) + m_pTargetVP->centerz;
	eyeDistance2D = eyeDistance3D * cos(m_pTargetVP->changeEyeTiltAngleVal) + m_pTargetVP->centerz;

	m_pTargetVP->eyex = eyeDistance2D * cos(m_pTargetVP->changeEyeTurnAngleVal) + m_pTargetVP->centerx;
	m_pTargetVP->eyey = eyeDistance2D * sin(m_pTargetVP->changeEyeTurnAngleVal) + m_pTargetVP->centery;
}

void oglFunctions::setViewCenter(float setX, float setY) {
	m_pTargetVP->eyex = m_pTargetVP->eyex - m_pTargetVP->centerx + setX;
	m_pTargetVP->eyey = m_pTargetVP->eyey - m_pTargetVP->centery + setY;

	m_pTargetVP->centerx = setX;
	m_pTargetVP->centery = setY;
}

void oglFunctions::setEyeDistance(float val) {
	float eyeDistance2D = 0;

	m_pTargetVP->zoomingVal = val;
	if(m_pTargetVP->zoomingVal <= LIMIT_ZOOM_MIN)				// 확대 제한
		m_pTargetVP->zoomingVal = LIMIT_ZOOM_MIN;
	else if(m_pTargetVP->zoomingVal >= LIMIT_ZOOM_MAX)		// 축소 제한
		m_pTargetVP->zoomingVal = LIMIT_ZOOM_MAX;

	if(m_pTargetVP->zoomingVal < LIMIT_GLOBE_LINE) {
		// 2D map
		m_pTargetVP->eyez = m_pTargetVP->zoomingVal * sin(m_pTargetVP->changeEyeTiltAngleVal) + m_pTargetVP->centerz;
		eyeDistance2D = m_pTargetVP->zoomingVal * cos(m_pTargetVP->changeEyeTiltAngleVal) + m_pTargetVP->centerz;

		m_pTargetVP->eyex = eyeDistance2D * cos(m_pTargetVP->changeEyeTurnAngleVal) + m_pTargetVP->centerx;
		m_pTargetVP->eyey = eyeDistance2D * sin(m_pTargetVP->changeEyeTurnAngleVal) + m_pTargetVP->centery;

	} else {
		// Globe
		float ratateVal = (m_pTargetVP->centerx/MAP_EXPANSION) * (M_PI / 180.0f);
		float tiltVal = (m_pTargetVP->centery/MAP_EXPANSION) * (M_PI / 180.0f);

		if(tiltVal < (M_PI / 180.0f * -89.0f))
			tiltVal = (M_PI / 180.0f * -89.0f);
		else if(tiltVal > (M_PI / 180.0f * 89.0f))
			tiltVal = (M_PI / 180.0f * 89.0f);

		m_pTargetVP->uniEyez = m_pTargetVP->zoomingVal * sin(tiltVal);
		eyeDistance2D = m_pTargetVP->zoomingVal * cos(tiltVal);

		m_pTargetVP->uniEyex = eyeDistance2D * cos(ratateVal);
		m_pTargetVP->uniEyey = eyeDistance2D * sin(ratateVal);
	}
}

void oglFunctions::initWorld(int32_t width, int32_t height) {
	/************************************************************************/
	/* 표시를 초기화한다.                                                   */
	/************************************************************************/
    glEnable(GL_NORMALIZE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);
    glCullFace(GL_BACK);
    glShadeModel(GL_SMOOTH);

	m_screenWidth = width;
	m_screenHeight = height;

    glClearColor(0.0f, 0.0f, 0.0f, 1.f);

    float ratio = (float) m_screenWidth / (float) m_screenHeight;
    glViewport(0, 0, (int) m_screenWidth, (int) m_screenHeight);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, ratio, 10.0f, LIMIT_ZOOM_MAX * 1.5F);

    changeEyeTurnAngle(0);
    changeEyeTiltAngle(0);
}

void oglFunctions::calcSmoothView() {
	
	float eyeDistance2D = 0;

	float ratateVal = (m_pVP->centerx/MAP_EXPANSION) * (M_PI / 180.0f);
	float tiltVal = (m_pVP->centery/MAP_EXPANSION) * (M_PI / 180.0f);

	if(tiltVal < (M_PI / 180.0f * -89.0f))
		tiltVal = (M_PI / 180.0f * -89.0f);
	else if(tiltVal > (M_PI / 180.0f * 89.0f))
		tiltVal = (M_PI / 180.0f * 89.0f);

	GLfloat gapEyeX = m_pTargetVP->eyex - m_pVP->eyex;
	GLfloat gapEyeY = m_pTargetVP->eyey - m_pVP->eyey;
	GLfloat gapEyeZ = m_pTargetVP->eyez - m_pVP->eyez;

	GLfloat gapUniEyeX = m_pTargetVP->uniEyex - m_pVP->uniEyex;
	GLfloat gapUniEyeY = m_pTargetVP->uniEyey - m_pVP->uniEyey;
	GLfloat gapUniEyeZ = m_pTargetVP->uniEyez - m_pVP->uniEyez;

	GLfloat gapCenterX = m_pTargetVP->centerx - m_pVP->centerx;
	GLfloat gapCenterY = m_pTargetVP->centery - m_pVP->centery;
	GLfloat gapCenterZ = m_pTargetVP->centerz - m_pVP->centerz;

	// Eye Position
	if(gapEyeX < 0.3f && gapEyeX > -0.3f)
		m_pVP->eyex = m_pTargetVP->eyex;
	else
		m_pVP->eyex = m_pVP->eyex + (gapEyeX / MAP_SMOOTHMOVE_GAP);

	if(gapEyeY < 0.3f && gapEyeY > -0.3f )
		m_pVP->eyey = m_pTargetVP->eyey;
	else
		m_pVP->eyey = m_pVP->eyey + (gapEyeY / MAP_SMOOTHMOVE_GAP);

	if(gapEyeZ < 0.3f && gapEyeZ > -0.3f)
		m_pVP->eyez = m_pTargetVP->eyez;
	else
		m_pVP->eyez = m_pVP->eyez + (gapEyeZ / MAP_SMOOTHMOVE_GAP);

	// Uni Eye Position
	m_pVP->uniEyez = getEyeDistance3D() * sin(tiltVal);
	eyeDistance2D = getEyeDistance3D() * cos(tiltVal);

	m_pVP->uniEyex = eyeDistance2D * cos(ratateVal);
	m_pVP->uniEyey = eyeDistance2D * sin(ratateVal);

	// Center Position
	if(gapCenterX < 0.3f && gapCenterX > -0.3f)
		m_pVP->centerx = m_pTargetVP->centerx;
	else
		m_pVP->centerx = m_pVP->centerx + (gapCenterX / MAP_SMOOTHMOVE_GAP);

	if(gapCenterY < 0.3f && gapCenterY > -0.3f)
		m_pVP->centery = m_pTargetVP->centery;
	else
		m_pVP->centery = m_pVP->centery + (gapCenterY / MAP_SMOOTHMOVE_GAP);

	if(gapCenterZ < 0.3f && gapCenterZ > -0.3f)
		m_pVP->centerz = m_pTargetVP->centerz;
	else
		m_pVP->centerz = m_pVP->centerz + (gapCenterZ / MAP_SMOOTHMOVE_GAP);

}

void oglFunctions::draw3DWorld() {
	// 프레임을 표시할 때마다 매번 초기화 한다.

	glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);

	gluLookAt(m_pVP->eyex, m_pVP->eyey, m_pVP->eyez, m_pVP->centerx, m_pVP->centery, m_pVP->centerz, m_pVP->upx, m_pVP->upy, m_pVP->upz );

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
	
	if(getEyeDistance3D() > LIMIT_GLOBE_LINE)
		return;

	float view2Dz = (m_pTargetVP->changeEyeTurnAngleVal / M_PI * 180.0f) + 90.0f;
	float view2Dx = -(m_pTargetVP->changeEyeTiltAngleVal / M_PI * 180.0f) + 90.0f;

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
	glTranslatef(m_targetX, m_targetY, 0);
	float crossScaleVal = getEyeDistance3D() / INIT_ZOOM_VAL;
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

	if(getEyeDistance3D() > LIMIT_GLOBE_LINE)
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
	float gridGap = getEyeDistance3D() / MAP_GRIDGAP;
	for(int i = -MAP_GRIDCOUNT; i <= MAP_GRIDCOUNT; i++) {
		for(int j = -MAP_GRIDCOUNT; j <= MAP_GRIDCOUNT; j++) {
			GLfloat moveGapX = 2 * gridGap * j;
			GLfloat moveGapY = 2 * gridGap * i;

			GLfloat gridVertices[] = {
				m_pTargetVP->centerx + moveGapX - gridGap, m_pTargetVP->centery + moveGapY + gridGap,
				m_pTargetVP->centerx + moveGapX - gridGap, m_pTargetVP->centery + moveGapY - gridGap,
				m_pTargetVP->centerx + moveGapX + gridGap, m_pTargetVP->centery + moveGapY + gridGap,

				m_pTargetVP->centerx + moveGapX + gridGap, m_pTargetVP->centery + moveGapY - gridGap,
				m_pTargetVP->centerx + moveGapX + gridGap, m_pTargetVP->centery + moveGapY + gridGap,
				m_pTargetVP->centerx + moveGapX - gridGap, m_pTargetVP->centery + moveGapY - gridGap,
			};
			glVertexPointer(2, GL_FLOAT, 0, gridVertices);
			glColor4f((m_drawPickingCount/(256*256))/255.0f, ((m_drawPickingCount%(256*256))/256)/255.0f,(m_drawPickingCount%256)/255.0f, 1.0f);
			glDrawArrays(GL_TRIANGLES, 0, 6);

			insertInfo.index = m_drawPickingCount;
			insertInfo.objID = 0;
			insertInfo.x = m_pTargetVP->centerx + moveGapX;
			insertInfo.y = m_pTargetVP->centery + moveGapY;

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

	m_drawPickingCount++;

	glPushMatrix();
	glTranslatef(170, -170, 0);
	glColor4f((m_drawPickingCount/(256*256))/255.0f, ((m_drawPickingCount%(256*256))/256)/255.0f,(m_drawPickingCount%256)/255.0f, 1.0f);
	glDrawElements(GL_TRIANGLE_STRIP, 14, GL_UNSIGNED_SHORT, cubeIndices);
	glPopMatrix();

	m_drawPickingCount++;

	glPushMatrix();
	glTranslatef(0, -170, 0);
	glColor4f((m_drawPickingCount/(256*256))/255.0f, ((m_drawPickingCount%(256*256))/256)/255.0f,(m_drawPickingCount%256)/255.0f, 1.0f);
	glDrawElements(GL_TRIANGLE_STRIP, 14, GL_UNSIGNED_SHORT, cubeIndices);
	glPopMatrix();
 
	m_drawPickingCount++;

	glPushMatrix();
	glTranslatef(0, 0, 0);
	glColor4f((m_drawPickingCount/(256*256))/255.0f, ((m_drawPickingCount%(256*256))/256)/255.0f,(m_drawPickingCount%256)/255.0f, 1.0f);
	glDrawElements(GL_TRIANGLE_STRIP, 14, GL_UNSIGNED_SHORT, cubeIndices);
	glPopMatrix();

	m_drawPickingCount++;

	glPushMatrix();
	glTranslatef(0, 170, 0);
	glColor4f((m_drawPickingCount/(256*256))/255.0f, ((m_drawPickingCount%(256*256))/256)/255.0f,(m_drawPickingCount%256)/255.0f, 1.0f);
	glDrawElements(GL_TRIANGLE_STRIP, 14, GL_UNSIGNED_SHORT, cubeIndices);
	glPopMatrix();

	m_drawPickingCount++;

	glPushMatrix();
	glTranslatef(0, 340, 0);
	glColor4f((m_drawPickingCount/(256*256))/255.0f, ((m_drawPickingCount%(256*256))/256)/255.0f,(m_drawPickingCount%256)/255.0f, 1.0f);
	glDrawElements(GL_TRIANGLE_STRIP, 14, GL_UNSIGNED_SHORT, cubeIndices);
	glPopMatrix();

	m_drawPickingCount++;

	glPushMatrix();
	glTranslatef(300, 170, 0);
	glColor4f((m_drawPickingCount/(256*256))/255.0f, ((m_drawPickingCount%(256*256))/256)/255.0f,(m_drawPickingCount%256)/255.0f, 1.0f);
	glDrawElements(GL_TRIANGLE_STRIP, 14, GL_UNSIGNED_SHORT, cubeIndices);
	glPopMatrix();

	m_drawPickingCount++;

	glPushMatrix();
	glTranslatef(300, 340, 0);
 	glColor4f((m_drawPickingCount/(256*256))/255.0f, ((m_drawPickingCount%(256*256))/256)/255.0f,(m_drawPickingCount%256)/255.0f, 1.0f);
	glDrawElements(GL_TRIANGLE_STRIP, 14, GL_UNSIGNED_SHORT, cubeIndices);
	glPopMatrix();

	m_drawPickingCount++;

	glPushMatrix();
	glTranslatef(12688539, 3747775, 0);
	glColor4f((m_drawPickingCount/(256*256))/255.0f, ((m_drawPickingCount%(256*256))/256)/255.0f,(m_drawPickingCount%256)/255.0f, 1.0f);
	glDrawElements(GL_TRIANGLE_STRIP, 14, GL_UNSIGNED_SHORT, cubeIndices);
	glPopMatrix();

	m_drawPickingCount++;

}

void oglFunctions::drawPicking2DInfo() { 
	/************************************************************************/
	/* Object 의 선택을 파악하기 위한 함수									*/
	/* POI 등의 선택을 파악하기 위한 함수									*/
	/* 2D 로 보이기 위한 각도 계산											*/
	/************************************************************************/

	if(getEyeDistance3D() > LIMIT_GLOBE_LINE)
		return;

	float view2Dz = (m_pTargetVP->changeEyeTurnAngleVal / M_PI * 180.0f) + 90.0f;
	float view2Dx = -(m_pTargetVP->changeEyeTiltAngleVal / M_PI * 180.0f) + 90.0f;

	glDisable(GL_DEPTH_TEST);

	glVertexPointer(2, GL_FLOAT, 0, triangleBuffer);

	glPushMatrix();
	glColor4f((m_drawPickingCount/(256*256))/255.0f, ((m_drawPickingCount%(256*256))/256)/255.0f,(m_drawPickingCount%256)/255.0f, 1.0f);
	glTranslatef(1000, 0, 0);
	glRotatef(view2Dz, 0, 0, 1);
	glRotatef(view2Dx, 1, 0, 0);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);
	glPopMatrix();

	m_drawPickingCount++;

	glPushMatrix();
	glColor4f((m_drawPickingCount/(256*256))/255.0f, ((m_drawPickingCount%(256*256))/256)/255.0f,(m_drawPickingCount%256)/255.0f, 1.0f);
	glTranslatef(0, 0, 400);
	glRotatef(view2Dz, 0, 0, 1);
	glRotatef(view2Dx, 1, 0, 0);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);
	glPopMatrix();

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
				break;
			}
		}
	}
	
	return retVal;
}
