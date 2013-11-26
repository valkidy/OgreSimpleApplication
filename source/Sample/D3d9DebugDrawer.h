#ifndef D3D9_DEBUG_DRAWER_H
#define D3D9_DEBUG_DRAWER_H

#include "CNativeRenderQueueListener.h"
#include "D3d9Stuff.h"
#include <windows.h>

// A structure for our custom vertex type
struct D3D9VERTEX2 // for line
{
    btScalar x, y, z;    // The untransformed, 3D position for the vertex
	DWORD color;         // The vertex color
};

struct D3D9VERTEX3 // for triangle
{
    btScalar x, y, z;    // The untransformed, 3D position for the vertex
    btScalar nx, ny, nz; // The untransformed, 3D normal for the vertex
	DWORD color;         // The vertex color
};

// A structure for our custom vertex buffer
struct D3D9VERTEXBUFFER2 // for line
{
    UINT index;             
    UINT size;
    BOOL resize; 
	D3D9VERTEX2* vertexBuffer;
};

struct D3D9VERTEXBUFFER3 // for triangle
{
    UINT index;
    UINT size;
    BOOL resize;
	D3D9VERTEX3* vertexBuffer;
};

#define SIZE_VERTEXBUFFER 4096
//
class D3d9DebugDrawer : public INativeRender
{
	int m_debugMode;

protected:
    LPDIRECT3DDEVICE9       m_pD3dDevice;
	LPDIRECT3DVERTEXBUFFER9 m_pVB;

public:
    /* ------------------------------------------------------------------
     | interface of render state
       ------------------------------------------------------------------ */
    virtual void initRender();
    virtual void preRender();
    virtual void postRender();
    
public:
    
    D3d9DebugDrawer();
	virtual ~D3d9DebugDrawer(); 

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

    void clearVertexBuffer();

    D3D9VERTEXBUFFER2& getVertexBuffer2() {return mVertexBuffer2;}
    D3D9VERTEXBUFFER3& getVertexBuffer3() {return mVertexBuffer3;}

protected:
    D3D9VERTEXBUFFER2 mVertexBuffer2;
    D3D9VERTEXBUFFER3 mVertexBuffer3;
};

#endif//D3D9_DEBUG_DRAWER_H
