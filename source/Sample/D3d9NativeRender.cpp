#include "D3d9NativeRender.h"
#include "D3d9DebugDrawer.h"

// custom FVF, which describes our custom vertex structure
#define D3DFVF_VERTEX2 (D3DFVF_XYZ|D3DFVF_DIFFUSE)
#define D3DFVF_VERTEX3 (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE)

void 
D3d9NativeRender::initRender()
{
    Ogre::RenderWindow* window = static_cast<Ogre::RenderWindow*>(Ogre::Root::getSingletonPtr()->getRenderTarget("Ogre Render Window"));
	window->getCustomAttribute( "D3DDEVICE", &m_pD3dDevice );    
}

void 
D3d9NativeRender::preRender()
{
    m_pD3dDebugDrawer->clearVertexBuffer();

    m_pD3dDevice->SetRenderState(D3DRS_LIGHTING, false);
    m_pD3dDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);
    m_pD3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);	
}

void 
D3d9NativeRender::postRender()
{
    D3D9VERTEXBUFFER2& VertexBuffer2 = m_pD3dDebugDrawer->getVertexBuffer2();
    UINT primitiveCount  = VertexBuffer2.index / 2;    
    UINT bufferSize = VertexBuffer2.index * sizeof(D3D9VERTEX2);

	// Create the vertex buffer.
    if( FAILED( m_pD3dDevice->CreateVertexBuffer( bufferSize, 
        0, D3DFVF_VERTEX2, D3DPOOL_DEFAULT, &m_pVB, NULL ) ) )
	{
		return;//E_FAIL;
	}

	// Fill the vertex buffer.
	VOID* pVertices;
	if( FAILED( m_pVB->Lock( 0, bufferSize, (void**)&pVertices, 0 ) ) )
		return;// E_FAIL;
    memcpy( pVertices, VertexBuffer2.vertexBuffer, bufferSize );
	m_pVB->Unlock();

	// Render the vertex buffer contents
	m_pD3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof(D3D9VERTEX2) );
	m_pD3dDevice->SetFVF( D3DFVF_VERTEX2 );
	m_pD3dDevice->DrawPrimitive( D3DPT_LINELIST, 0, primitiveCount );

	m_pVB->Release();
}