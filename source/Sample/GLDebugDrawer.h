#ifndef GL_DEBUG_DRAWER_H
#define GL_DEBUG_DRAWER_H

#include "CNativeRenderQueueListener.h"

// for openGL
#pragma comment(lib , "opengl32.lib")
#pragma comment(lib , "glut32.lib")
#pragma comment(lib , "glew32s.lib")

class GLDebugDrawer : public INativeRender
{
	int m_debugMode;

public:
    /* ------------------------------------------------------------------
     | interface of render state
       ------------------------------------------------------------------ */
    virtual void initRender() {}
    virtual void preRender();
    virtual void postRender();

public:
    
	GLDebugDrawer();
	virtual ~GLDebugDrawer(); 

    /* ------------------------------------------------------------------
     | interface of btDebugDrawer
       ------------------------------------------------------------------ */
	virtual void	drawLine(const btVector3& from,const btVector3& to,const btVector3& fromColor, const btVector3& toColor);

	virtual void	drawLine(const btVector3& from,const btVector3& to,const btVector3& color);

	virtual void	drawSphere (const btVector3& p, btScalar radius, const btVector3& color);

	virtual void	drawTriangle(const btVector3& a,const btVector3& b,const btVector3& c,const btVector3& color,btScalar alpha);
	
	virtual void	drawContactPoint(const btVector3& PointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color);

	virtual void	reportErrorWarning(const char* warningString);

	virtual void	draw3dText(const btVector3& location,const char* textString);

	virtual void	setDebugMode(int debugMode);

	virtual int		getDebugMode() const { return m_debugMode;}

};

#endif//GL_DEBUG_DRAWER_H
