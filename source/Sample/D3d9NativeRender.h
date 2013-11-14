#pragma once

#include "CNativeRenderQueueListener.h"
#include "D3d9Stuff.h"

/*
openGL native render 
 */
class D3d9DebugDrawer;
class D3d9NativeRender : public INativeRender
{
public:    
    virtual void initRender();

    virtual void preRender();
    virtual void postRender();
        
    void bindDebugDrawer(D3d9DebugDrawer* pD3dDebugDrawer) {m_pD3dDebugDrawer = pD3dDebugDrawer;}

protected:
    LPDIRECT3DDEVICE9       m_pD3dDevice;
	LPDIRECT3DVERTEXBUFFER9 m_pVB;

    D3d9DebugDrawer* m_pD3dDebugDrawer;
};
