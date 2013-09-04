#ifndef __OGL_H__
#define __OGL_H__

#include "../include/types.h"

typedef struct _ViewPoint{
	GLfloat eyex;
	GLfloat eyey;
	GLfloat eyez;
	GLfloat uniEyex;
	GLfloat uniEyey;
	GLfloat uniEyez;
	GLfloat centerx;
	GLfloat centery;
	GLfloat centerz;
	GLfloat upx;
	GLfloat upy;
	GLfloat upz;
	float changeEyeTurnAngleVal;
	float changeEyeTiltAngleVal;
	float zoomingVal;
} ViewPoint;

class oglFunctions
{
private:
	ViewPoint *m_pVP;
	ViewPoint *m_pTargetVP;

	unsigned int m_nTextureObject[1];

	int32_t m_screenWidth, m_screenHeight;
	unsigned int m_drawPickingCount;

	std::vector<OBJINFO>	m_objectInfo;
	float m_targetX, m_targetY;

	// glu
	void gluPerspective(double fovy, double aspect, double zNear, double zFar);
	void gluLookAt(GLfloat eyex, GLfloat eyey, GLfloat eyez, GLfloat centerx, GLfloat centery, GLfloat centerz, GLfloat upx, GLfloat upy, GLfloat upz);

public:
	oglFunctions();
	virtual ~oglFunctions();

	// get
	float getEyeDistance3D();
	float getEyeDistance2D();
	float getTanDegree(float x, float y);

	unsigned int getObjInfo(int sx, int sy, unsigned int objID, GLfloat &mx, GLfloat &my);

	// set
	void changeEyeTurnAngle(float degree);
	void changeEyeTiltAngle(float degree);
	void changeViewCenter(float degree, float val);
	void changeEyeDistance(float val);

	void setEyeTurnAngle(int degree);
	void setEyeTiltAngle(int degree);
	void setViewCenter(float setX, float setY);
	void setEyeDistance(float val);

	// drawing
	void initWorld(int32_t width, int32_t height);
	void calcSmoothView();
	void draw3DWorld();
	void draw2DInfo();
	
	// After click the screen, draw world. (To distinguish between the object ID.)
	void drawPicking3DWorld();
	void drawPicking2DInfo();

};

#endif

