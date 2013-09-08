// =========================================================
//
// oglViewControl.cpp
// view control engine (body).
// Created by Wooram Hong 2013/05/13
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
#else
#include "../../vsProject/StdAfx.h"
#endif

#include "oglViewControl.h"
#include <math.h>

oglViewContor::oglViewContor()
{
	// set view
	m_pTargetVP = (ViewPoint *)malloc(sizeof(ViewPoint));
	memset(m_pTargetVP, 0x00, sizeof(ViewPoint));
	m_pTargetVP->changeEyeTurnAngleDegreeVal = -90.0f;	// North up (Y axis up)
	m_pTargetVP->changeEyeTiltAngleDegreeVal = 89.0f;		// ������ �����ٺ���
	m_pTargetVP->eyez = m_pTargetVP->zoomingVal = INIT_ZOOM_VAL;
	m_pTargetVP->uniEyez = GLOBE_SIZE + INIT_ZOOM_VAL;

	m_pVP = (ViewPoint *)malloc(sizeof(ViewPoint));
	memset(m_pVP, 0x00, sizeof(ViewPoint));
	m_pVP->changeEyeTurnAngleDegreeVal = -90.0f;	// North up (Y axis up)
	m_pVP->changeEyeTiltAngleDegreeVal = 89.0f;		// ������ �����ٺ���
	m_pVP->eyez = m_pVP->zoomingVal = INIT_ZOOM_VAL;
	m_pVP->uniEyez = GLOBE_SIZE + INIT_ZOOM_VAL;

	// set start position
	float setX = 0.0f;
	float setY = 0.0f;

	m_pTargetVP->eyex = 0.0f;
	m_pTargetVP->eyey = -1.0f;
	m_pTargetVP->centerx = setX;
	m_pTargetVP->centery = setY;

	m_pVP->eyex =  0.0f;
	m_pVP->eyey = -1.0f;
	m_pVP->centerx = setX;
	m_pVP->centery = setY;

	setEyeDistance(SET_VIEW, INIT_ZOOM_VAL);

}

oglViewContor::~oglViewContor()
{
	free(m_pVP);
}

ViewPoint *oglViewContor::getVP(int eyeType)
{
	switch(eyeType)
	{
	case CURRENT_VIEW : 
		return m_pVP;
	case TARGET_VIEW:
		return m_pTargetVP;
	default:
		return m_pVP;
	}
}

float oglViewContor::getEyeDistance3D(int eyeType) {
	switch(eyeType)
	{
	case CURRENT_VIEW:
		return m_pVP->zoomingVal;
	case TARGET_VIEW:
		return m_pTargetVP->zoomingVal;
	default:
		return m_pVP->zoomingVal;
	}
}

float oglViewContor::getEyeDistance2D(int eyeType) {
	float distance = 0;

	switch(eyeType)
	{
	case CURRENT_VIEW :
		distance = pow((m_pVP->eyex), 2) + pow((m_pVP->eyey), 2);
		break;
	case TARGET_VIEW : 
		distance = pow((m_pTargetVP->eyex), 2) + pow((m_pTargetVP->eyey), 2);
		break;
	default:
		distance = pow((m_pVP->eyex), 2) + pow((m_pVP->eyey), 2);
		break;
	}

	distance = sqrt(distance);
	return distance;
}

float oglViewContor::getTanDegree(float x, float y) {
	float tanVal = y / x;
	float moveDegree = 0;

	// tan ���� �� ������ 90�� ������ ������ �ȴ�.
	if(x == 0)
		moveDegree = y > 0 ? 0.0f : 180.0f;
	else if(y == 0)
		moveDegree = x > 0 ? -90.0f : 90.0f;
	else {
		moveDegree = (atan(tanVal) * 180.0f / M_PI) ;
		if(x > 0) {
			if(y < 0) 			// 1��и�
				moveDegree -= 90.0f;
			else if(y > 0)	// 4 ��и�
				moveDegree -= 90.0f;
		} else if(x < 0) {
			if(y < 0) 			// 2 ��и�
				moveDegree += 90.0f;
			else if(y > 0) 	// 3 ��и�
				moveDegree += 90.0f;
		}
	}
	return moveDegree;
}

void oglViewContor::changeViewCenter(float degree, float val) {

 	float moveEyeAngleVal = m_pTargetVP->changeEyeTurnAngleDegreeVal + 180.0f - degree;

	// ������������ �������� ������
	if(getEyeDistance3D(TARGET_VIEW) > LIMIT_GLOBE_LINE) {
		setEyeTurnAngle(SET_VIEW, 0.0f);
		setEyeTiltAngle(SET_VIEW, 90.0f);
		val = val * getEyeDistance3D(CURRENT_VIEW) / LIMIT_GLOBE_LINE * 1.5f;
	}

	float tmpCy = m_pTargetVP->centery + val * sin((double)moveEyeAngleVal * M_PI / 180.0f);

	// ���������δ� ���� �̵��� ������ �д�.
	if(!(tmpCy > MAP_BOX_UNIT-(15*MAP_EXPANSION) || tmpCy < -(MAP_BOX_UNIT-(15*MAP_EXPANSION)))) {
		m_pTargetVP->centerx = m_pTargetVP->centerx + val * cos((double)moveEyeAngleVal * M_PI / 180.0f);
		m_pTargetVP->centery = tmpCy;
	}

	// Repeat map moving. �浵�δ� ������ �ݺ��Ͽ� ��Ÿ����.
	// 2D Map �� Globe ��� ����ǹǷ� ���� �ٲ��� �� ��.
	if(m_pTargetVP->centerx > (MAP_BOX_UNIT*2)) {
		double xGap = m_pTargetVP->eyex - m_pTargetVP->centerx;

		m_pTargetVP->centerx = -(MAP_BOX_UNIT*4) + m_pTargetVP->centerx;
		m_pTargetVP->eyex = xGap;

		m_pTargetVP->centerx = -(MAP_BOX_UNIT*4) + m_pTargetVP->centerx;
		m_pTargetVP->eyex = m_pTargetVP->centerx + xGap;

	} else if(m_pTargetVP->centerx < -(MAP_BOX_UNIT*2)) {
		double xGap = m_pTargetVP->eyex - m_pTargetVP->centerx;

		m_pTargetVP->centerx = (MAP_BOX_UNIT*4) + m_pTargetVP->centerx;
		m_pTargetVP->eyex = xGap;

		m_pTargetVP->centerx = (MAP_BOX_UNIT*4) + m_pTargetVP->centerx;
		m_pTargetVP->eyex = m_pTargetVP->centerx + xGap;
	}

	setUniverseEye();

}

void oglViewContor::setEyeTurnAngle(int eyeType, float degree) {
	float eyeDistance = getEyeDistance2D(TARGET_VIEW);

	switch(eyeType)
	{
	case CHANGE_VIEW:
		m_pTargetVP->changeEyeTurnAngleDegreeVal += degree;
		break;
	case SET_VIEW:
		m_pTargetVP->changeEyeTurnAngleDegreeVal = degree - 90.0f;	// 90 �� �� ������ ������ 0���� �����ϱ� ������.
		break;
	default:
		m_pTargetVP->changeEyeTurnAngleDegreeVal = degree - 90.0f;	// 90 �� �� ������ ������ 0���� �����ϱ� ������.
		break;
	}
	
	m_pTargetVP->eyex = eyeDistance * cos((double)m_pTargetVP->changeEyeTurnAngleDegreeVal * M_PI / 180.0f);
	m_pTargetVP->eyey = eyeDistance * sin((double)m_pTargetVP->changeEyeTurnAngleDegreeVal * M_PI / 180.0f);
}

void oglViewContor::setEyeTiltAngle(int eyeType, float degree) {
	float eyeDistance3D = getEyeDistance3D(TARGET_VIEW);
	float eyeDistance2D = 0;

	switch(eyeType)
	{
	case CHANGE_VIEW:
		m_pTargetVP->changeEyeTiltAngleDegreeVal += degree;
		break;
	case SET_VIEW:
		m_pTargetVP->changeEyeTiltAngleDegreeVal = degree;
		break;
	default:
		m_pTargetVP->changeEyeTiltAngleDegreeVal = degree;
		break;
	}
	
	if(m_pTargetVP->changeEyeTiltAngleDegreeVal < LIMIT_TILT_MIN)
		m_pTargetVP->changeEyeTiltAngleDegreeVal = LIMIT_TILT_MIN;
	else if(m_pTargetVP->changeEyeTiltAngleDegreeVal > LIMIT_TILT_MAX)
		m_pTargetVP->changeEyeTiltAngleDegreeVal = LIMIT_TILT_MAX;

	m_pTargetVP->eyez = eyeDistance3D * sin((double)m_pTargetVP->changeEyeTiltAngleDegreeVal * M_PI / 180.0f);
	eyeDistance2D = eyeDistance3D * cos((double)m_pTargetVP->changeEyeTiltAngleDegreeVal * M_PI / 180.0f);

	m_pTargetVP->eyex = eyeDistance2D * cos((double)m_pTargetVP->changeEyeTurnAngleDegreeVal * M_PI / 180.0f);
	m_pTargetVP->eyey = eyeDistance2D * sin((double)m_pTargetVP->changeEyeTurnAngleDegreeVal * M_PI / 180.0f);
}

void oglViewContor::setViewCenter(int eyeType, float setX, float setY) {

	switch(eyeType)
	{
	case TARGET_VIEW:
		m_pTargetVP->centerx = setX;
		m_pTargetVP->centery = setY;
		break;
	case CURRENT_VIEW:
		m_pVP->centerx = setX;
		m_pVP->centery = setY;
		break;
	default:
		m_pVP->centerx = setX;
		m_pVP->centery = setY;
		break;
	}
}

void oglViewContor::setEyeDistance(int eyeType, float val) {
	float eyeDistance2D = 0;

	switch(eyeType)
	{
	case CHANGE_VIEW:
		m_pTargetVP->zoomingVal += val;
		break;
	case SET_VIEW:
		m_pTargetVP->zoomingVal = val;
		break;
	default:
		m_pTargetVP->zoomingVal = val;
		break;
	}
	
	if(m_pTargetVP->zoomingVal <= LIMIT_ZOOM_MIN)				// Ȯ�� ����
		m_pTargetVP->zoomingVal = LIMIT_ZOOM_MIN;
	else if(m_pTargetVP->zoomingVal >= LIMIT_ZOOM_MAX)		// ��� ����
		m_pTargetVP->zoomingVal = LIMIT_ZOOM_MAX;

	if(m_pTargetVP->zoomingVal < LIMIT_GLOBE_LINE) {
		// 2D map
		m_pTargetVP->eyez = m_pTargetVP->zoomingVal * sin((double)m_pTargetVP->changeEyeTiltAngleDegreeVal * M_PI / 180.0f);
		eyeDistance2D = m_pTargetVP->zoomingVal * cos((double)m_pTargetVP->changeEyeTiltAngleDegreeVal * M_PI / 180.0f);

		m_pTargetVP->eyex = eyeDistance2D * cos((double)m_pTargetVP->changeEyeTurnAngleDegreeVal * M_PI / 180.0f);
		m_pTargetVP->eyey = eyeDistance2D * sin((double)m_pTargetVP->changeEyeTurnAngleDegreeVal * M_PI / 180.0f);
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

void oglViewContor::setUniverseEye() {
	float eyeDistance2D = 0;

	double ratateVal = (m_pTargetVP->centerx/MAP_EXPANSION) * (M_PI / 180.0f);
	double tiltVal = (m_pTargetVP->centery/MAP_EXPANSION) * (M_PI / 180.0f);

	if(tiltVal < (M_PI / 180.0f * -89.0f))
		tiltVal = (M_PI / 180.0f * -89.0f);
	else if(tiltVal > (M_PI / 180.0f * 89.0f))
		tiltVal = (M_PI / 180.0f * 89.0f);

	m_pTargetVP->uniEyez = getEyeDistance3D(TARGET_VIEW) * sin(tiltVal);
	eyeDistance2D = getEyeDistance3D(TARGET_VIEW) * cos(tiltVal);

	m_pTargetVP->uniEyex = eyeDistance2D * cos(ratateVal);
	m_pTargetVP->uniEyey = eyeDistance2D * sin(ratateVal);
}

void oglViewContor::calcSmoothView() {

	// Globe ��
 	float rotateVal = (m_pVP->centerx/MAP_EXPANSION) * (M_PI / 180.0f);
 	float tiltVal = (m_pVP->centery/MAP_EXPANSION) * (M_PI / 180.0f);
 	if(tiltVal < (M_PI / 180.0f * -89.0f))
 		tiltVal = (M_PI / 180.0f * -89.0f);
 	else if(tiltVal > (M_PI / 180.0f * 89.0f))
 		tiltVal = (M_PI / 180.0f * 89.0f);
 
 	// Uni Eye Position
 	float eyeUniDistance2D = getEyeDistance3D(TARGET_VIEW) * cos(tiltVal);
 	m_pVP->uniEyex = eyeUniDistance2D * cos(rotateVal);
	m_pVP->uniEyey = eyeUniDistance2D * sin(rotateVal);
	m_pVP->uniEyez = getEyeDistance3D(TARGET_VIEW) * sin(tiltVal);

 	// calc gap
 	float gapEyeX = m_pTargetVP->eyex - m_pVP->eyex;
 	float gapEyeY = m_pTargetVP->eyey - m_pVP->eyey;
 	float gapEyeZ = m_pTargetVP->eyez - m_pVP->eyez;

	float gapCenterX = m_pTargetVP->centerx - m_pVP->centerx;
	float gapCenterY = m_pTargetVP->centery - m_pVP->centery;
	float gapCenterZ = m_pTargetVP->centerz - m_pVP->centerz;

// 	if(gapCenterX != 0)
// 		gapEyeX = gapCenterX;
// 
// 	if(gapCenterY != 0)
// 		gapEyeY = gapCenterY;
// 
// 	if(gapCenterZ != 0)
// 		gapEyeZ = gapCenterZ;

	int zoomLevel = (int)(sqrt((m_pVP->zoomingVal / 200.0f)) + 1) * (float)MAP_SMOOTHMOVE_GAP / 2;

	// Eye Position
// 	if(gapEyeX <= zoomLevel && gapEyeX >= -zoomLevel)
// 		m_pVP->eyex = m_pTargetVP->eyex;
// 	else
		m_pVP->eyex = m_pVP->eyex + (gapEyeX / (float)MAP_SMOOTHMOVE_GAP);

// 	if(gapEyeY <= zoomLevel && gapEyeY >= -zoomLevel)
// 		m_pVP->eyey = m_pTargetVP->eyey;
// 	else
		m_pVP->eyey = m_pVP->eyey + (gapEyeY / (float)MAP_SMOOTHMOVE_GAP);

// 	if(gapEyeZ <= zoomLevel && gapEyeZ >= -zoomLevel)
// 		m_pVP->eyez = m_pTargetVP->eyez;
// 	else
		m_pVP->eyez = m_pVP->eyez + (gapEyeZ / (float)MAP_SMOOTHMOVE_GAP);

	// Center Position
// 	if(gapCenterX <= zoomLevel && gapCenterX >= -zoomLevel)
// 		m_pVP->centerx = m_pTargetVP->centerx;
// 	else
		m_pVP->centerx = m_pVP->centerx + (gapCenterX / (float)MAP_SMOOTHMOVE_GAP);

// 	if(gapCenterY <= zoomLevel && gapCenterY >= -zoomLevel)
// 		m_pVP->centery = m_pTargetVP->centery;
// 	else
		m_pVP->centery = m_pVP->centery + (gapCenterY / (float)MAP_SMOOTHMOVE_GAP);

// 	if(gapCenterZ <= zoomLevel && gapCenterZ >= -zoomLevel)
// 		m_pVP->centerz = m_pTargetVP->centerz;
// 	else
		m_pVP->centerz = m_pVP->centerz + (gapCenterZ / (float)MAP_SMOOTHMOVE_GAP);

	// Set Angle	// POI ���� ��꿡 �ʿ�
	m_pVP->changeEyeTurnAngleDegreeVal = getTanDegree(m_pVP->eyex, m_pVP->eyey) + 90;
	m_pVP->changeEyeTiltAngleDegreeVal = m_pTargetVP->changeEyeTiltAngleDegreeVal;//(int)asin((m_pVP->eyez - m_pVP->centerz) / M_PI * 180.0f / getEyeDistance3D(TARGET_VIEW));

	// set zoom
	float currEyeDistance3D = pow(m_pVP->eyex, 2)
		+ pow(m_pVP->eyey, 2)
		+ pow(m_pVP->eyez, 2);
	currEyeDistance3D = sqrt(currEyeDistance3D);

	m_pVP->zoomingVal = currEyeDistance3D;

}

float oglViewContor::calcCutVal(float cutVal, float inVal)
{
	float tmp;

	if(inVal < cutVal && inVal > -cutVal)
		return inVal;

	if(inVal >= cutVal)
		inVal -= cutVal;
	else if(inVal <= -cutVal)
		inVal += cutVal;

	tmp = calcCutVal(cutVal, inVal);

	return tmp ;
}