// =========================================================
//
// data.h
// 3D data
// Created by Wooram Hong 2013/02/06
// This file is part of the AMCOP project.
//
// =========================================================

#ifndef __DATA_H__
#define __DATA_H__

#include "types.h"

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

#endif
