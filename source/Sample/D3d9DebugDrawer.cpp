#include "D3d9DebugDrawer.h"
#include "D3d9Stuff.h"

// Convert to 32bit pattern : (RGBA = 8888)
DWORD getAsRGBA(const btVector3& valuef)
{
    return D3DCOLOR_RGBA(
        static_cast<WORD>(valuef.getX()*255), 
        static_cast<WORD>(valuef.getY()*255), 
        static_cast<WORD>(valuef.getZ()*255),
        255);
}
       
D3d9DebugDrawer::D3d9DebugDrawer()
    :m_debugMode(0)
{
    mVertexBuffer2.index = 0;
    mVertexBuffer2.size = SIZE_VERTEXBUFFER;
    mVertexBuffer2.resize = false;
    mVertexBuffer2.vertexBuffer = new D3D9VERTEX2[ SIZE_VERTEXBUFFER ];

    mVertexBuffer3.index = 0;
    mVertexBuffer3.size = SIZE_VERTEXBUFFER;
    mVertexBuffer3.resize = false;
    mVertexBuffer3.vertexBuffer = new D3D9VERTEX3[ SIZE_VERTEXBUFFER ];
}

D3d9DebugDrawer::~D3d9DebugDrawer()
{
    mVertexBuffer2.index = mVertexBuffer2.size = 0;
    mVertexBuffer3.index = mVertexBuffer3.size = 0;
    delete [] mVertexBuffer2.vertexBuffer;
    delete [] mVertexBuffer3.vertexBuffer;
}

void    D3d9DebugDrawer::clearVertexBuffer()
{    
    if (mVertexBuffer2.resize)
    {
        mVertexBuffer2.size *= 2;

        delete [] mVertexBuffer2.vertexBuffer;
        mVertexBuffer2.vertexBuffer = new D3D9VERTEX2[ mVertexBuffer2.size ];
    } // End if
 
    mVertexBuffer2.index = 0;
    mVertexBuffer2.resize = false;
    memset(mVertexBuffer2.vertexBuffer, 0, sizeof(D3D9VERTEX2) * mVertexBuffer2.size);
    
    if (mVertexBuffer3.resize)
    {
        mVertexBuffer3.size *= 2;

        delete [] mVertexBuffer3.vertexBuffer;
        mVertexBuffer3.vertexBuffer = new D3D9VERTEX3[ mVertexBuffer3.size ];
    } // End if
 
    mVertexBuffer3.index = 0;
    mVertexBuffer3.resize = false;
    memset(mVertexBuffer3.vertexBuffer, 0, sizeof(D3D9VERTEX2) * mVertexBuffer3.size);
}

void	D3d9DebugDrawer::drawLine(const btVector3& from,const btVector3& to,const btVector3& fromColor, const btVector3& toColor)
{
    UINT i = mVertexBuffer2.index;
    if (i < (mVertexBuffer2.size - 2))
    {
        mVertexBuffer2.vertexBuffer[ i ].x = from.getX();
        mVertexBuffer2.vertexBuffer[ i ].y = from.getY();
        mVertexBuffer2.vertexBuffer[ i ].z = from.getZ();
        mVertexBuffer2.vertexBuffer[ i ].color = getAsRGBA(fromColor);

        mVertexBuffer2.vertexBuffer[ i+1 ].x = to.getX();
        mVertexBuffer2.vertexBuffer[ i+1 ].y = to.getY();
        mVertexBuffer2.vertexBuffer[ i+1 ].z = to.getZ();
        mVertexBuffer2.vertexBuffer[ i+1 ].color = getAsRGBA(toColor);

        mVertexBuffer2.index += 2;
    }
    else
        mVertexBuffer2.resize = true;   
}

void	D3d9DebugDrawer::drawLine(const btVector3& from,const btVector3& to,const btVector3& color)
{
	drawLine(from, to, color, color);
}

void D3d9DebugDrawer::drawSphere (const btVector3& p, btScalar radius, const btVector3& color)
{
    /*
	glColor4f (color.getX(), color.getY(), color.getZ(), btScalar(1.0f));
	glPushMatrix ();
	glTranslatef (p.getX(), p.getY(), p.getZ());

	int lats = 5;
	int longs = 5;

	int i, j;
	for(i = 0; i <= lats; i++) {
		btScalar lat0 = SIMD_PI * (-btScalar(0.5) + (btScalar) (i - 1) / lats);
		btScalar z0  = radius*sin(lat0);
		btScalar zr0 =  radius*cos(lat0);

		btScalar lat1 = SIMD_PI * (-btScalar(0.5) + (btScalar) i / lats);
		btScalar z1 = radius*sin(lat1);
		btScalar zr1 = radius*cos(lat1);

		glBegin(GL_QUAD_STRIP);
		for(j = 0; j <= longs; j++) {
			btScalar lng = 2 * SIMD_PI * (btScalar) (j - 1) / longs;
			btScalar x = cos(lng);
			btScalar y = sin(lng);

			glNormal3f(x * zr0, y * zr0, z0);
			glVertex3f(x * zr0, y * zr0, z0);
			glNormal3f(x * zr1, y * zr1, z1);
			glVertex3f(x * zr1, y * zr1, z1);
		}
		glEnd();
	}

	glPopMatrix();
    */
}

void	D3d9DebugDrawer::drawTriangle(const btVector3& a,const btVector3& b,const btVector3& c,const btVector3& color,btScalar alpha)
{
    /*
	{
		const btVector3	n=btCross(b-a,c-a).normalized();
		glBegin(GL_TRIANGLES);		
		glColor4f(color.getX(), color.getY(), color.getZ(),alpha);
		glNormal3d(n.getX(),n.getY(),n.getZ());
		glVertex3d(a.getX(),a.getY(),a.getZ());
		glVertex3d(b.getX(),b.getY(),b.getZ());
		glVertex3d(c.getX(),c.getY(),c.getZ());
		glEnd();
	}
    */
}

void	D3d9DebugDrawer::setDebugMode(int debugMode)
{
	m_debugMode = debugMode;

}

void	D3d9DebugDrawer::draw3dText(const btVector3& location,const char* textString)
{
    printf("Function not support, D3d9DebugDrawer::draw3dText\n");	
}

void	D3d9DebugDrawer::reportErrorWarning(const char* warningString)
{
	printf("%s\n",warningString);
}

void	D3d9DebugDrawer::drawContactPoint(const btVector3& pointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color)
{
	/*
	{
		btVector3 to=pointOnB+normalOnB*1;//distance;
		const btVector3&from = pointOnB;
		glColor4f(color.getX(), color.getY(), color.getZ(),1.f);	
		glBegin(GL_LINES);
		glVertex3d(from.getX(), from.getY(), from.getZ());
		glVertex3d(to.getX(), to.getY(), to.getZ());
		glEnd();
	
	}
    */
}

/* ------------------------------------------------------------------
| implement of d3d render state
  ------------------------------------------------------------------ */	
// custom FVF, which describes our custom vertex structure
#define D3DFVF_VERTEX2 (D3DFVF_XYZ|D3DFVF_DIFFUSE)
#define D3DFVF_VERTEX3 (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE)

void D3d9DebugDrawer::initRender()
{
    Ogre::RenderWindow* window = static_cast<Ogre::RenderWindow*>(Ogre::Root::getSingletonPtr()->getRenderTarget("Ogre Render Window"));
	window->getCustomAttribute( "D3DDEVICE", &m_pD3dDevice );

    m_pVB = NULL;
}

void D3d9DebugDrawer::preRender()
{
    clearVertexBuffer();

    m_pD3dDevice->SetRenderState(D3DRS_LIGHTING, false);
    m_pD3dDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);
    m_pD3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);	
}

void D3d9DebugDrawer::postRender()
{
    D3D9VERTEXBUFFER2& VertexBuffer2 = getVertexBuffer2();
    UINT primitiveCount  = VertexBuffer2.index / 2;    
    static UINT bufferSize = VertexBuffer2.index * sizeof(D3D9VERTEX2);
        
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



