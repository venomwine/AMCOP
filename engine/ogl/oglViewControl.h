// =========================================================
//
// oglViewControl.h
// view control engine (head).
// Created by Wooram Hong 2013/05/13
// This file is part of the AMCOP project.
//
// =========================================================

#ifndef __OGL_VIEWCONTROL_H__
#define __OGL_VIEWCONTROL_H__

#include "../include/types.h"

typedef struct _ViewPoint{
	float eyex;
	float eyey;
	float eyez;
	float uniEyex;
	float uniEyey;
	float uniEyez;
	float centerx;
	float centery;
	float centerz;
	float upx;
	float upy;
	float upz;
	float changeEyeTurnAngleDegreeVal;
	float changeEyeTiltAngleDegreeVal;
	float zoomingVal;
} ViewPoint;

class oglViewContor
{
private:
	ViewPoint *m_pVP;
	ViewPoint *m_pTargetVP;

public:
	oglViewContor();
	virtual ~oglViewContor();

	// calc
	void calcSmoothView();
	float calcCutVal(float cutVal, float inVal);

	// get
	float getEyeDistance3D(int eyeType);
	float getEyeDistance2D(int eyeType);
	float getTanDegree(float x, float y);

	ViewPoint *getVP(int eyeType);

	// change
	void changeViewCenter(float degree, float val);

	// set
	void setViewCenter(int eyeType, float setX, float setY);
	void setEyeTurnAngle(int eyeType, float degree);
	void setEyeTiltAngle(int eyeType, float degree);
	void setEyeDistance(int eyeType, float val);
	
	void setUniverseEye();

};

#endif

