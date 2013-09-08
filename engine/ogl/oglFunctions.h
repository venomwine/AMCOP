// =========================================================
//
// oglFunctions.h
// view control & draw objects engine (head).
// Created by Wooram Hong 2013/02/06
// This file is part of the AMCOP project.
//
// =========================================================

#ifndef __OGL_H__
#define __OGL_H__

#include "../include/types.h"
#include "oglViewControl.h"

class oglFunctions
{
private:
	unsigned int m_nTextureObject[1];

	int32_t m_screenWidth, m_screenHeight;
	unsigned int m_drawPickingCount;

	std::vector<OBJINFO>	m_objectInfo;
	float m_targetX, m_targetY, m_targetZ;

	// glu
	void	gluPerspective(double fovy, double aspect, double zNear, double zFar);

public:
	oglFunctions();
	virtual ~oglFunctions();

	oglViewContor	*m_VC;

	// drawing
	int initWorld(int32_t width, int32_t height);

	void draw3DWorld();
	void draw2DInfo();

	// After click the screen, draw world. (To distinguish between the object ID.)
	void drawPicking3DWorld();
	void drawPicking2DInfo();

	// get
	int		getZoomLevel();
	unsigned int	getObjInfo(int sx, int sy, unsigned int objID, GLfloat &mx, GLfloat &my);

};

#endif

