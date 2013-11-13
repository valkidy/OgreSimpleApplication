#include "D3d9DebugDrawer.h"
#include "D3d9Stuff.h"
#include <stdio.h>

#include "COgreHeader.h"

// A structure for our custom vertex type
struct D3D9VERTEX
{
    Ogre::Vector3 position;
	//btVector3 position; // The untransformed, 3D position for the vertex			
	DWORD color;        // The vertex color
} mVertexBufferForLine[2];

// Convert to 32bit pattern : (RGBA = 8888)
DWORD getAsRGBA(const btVector3& valuef)
{
    float x = valuef.getX();
    float y = valuef.getY();
    float z = valuef.getZ();

    return D3DCOLOR_RGBA(
        static_cast<WORD>(x*255), 
        static_cast<WORD>(y*255), 
        static_cast<WORD>(z*255),
        255);

    /*
    DWORD value32 = 0;
    value32 =  (static_cast<WORD>(valuef.getX() * 255) << 24); // Red
    value32 += (static_cast<WORD>(valuef.getY() * 255) << 16); // Green
    value32 += (static_cast<WORD>(valuef.getZ() * 255) << 8 ); // Blue
    value32 += (255); // Alpha = 255
    
    return value32;
    */
}

        
D3d9DebugDrawer::D3d9DebugDrawer()
    :m_debugMode(0)
{

}

D3d9DebugDrawer::~D3d9DebugDrawer()
{
}

void	D3d9DebugDrawer::drawLine(const btVector3& from,const btVector3& to,const btVector3& fromColor, const btVector3& toColor)
{
    LPDIRECT3DDEVICE9       pd3dDevice = NULL; // Our rendering device
	LPDIRECT3DVERTEXBUFFER9 pVB        = NULL; // Buffer to hold vertices

    Ogre::RenderWindow* mWindow = static_cast<Ogre::RenderWindow*>(Ogre::Root::getSingletonPtr()->getRenderTarget("Ogre Render Window"));
	mWindow->getCustomAttribute( "D3DDEVICE", &pd3dDevice );

    pd3dDevice->SetRenderState(D3DRS_LIGHTING, false);
    pd3dDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);
    pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);

	// custom FVF, which describes our custom vertex structure
	#define D3DFVF_VERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE)

    mVertexBufferForLine[0].position = Ogre::Vector3(from.getX(), from.getY(), from.getZ());    
    mVertexBufferForLine[0].color = D3DCOLOR_RGBA(
        static_cast<WORD>(fromColor.getX() * 255.0f),
        static_cast<WORD>(fromColor.getY() * 255.0f),
        static_cast<WORD>(fromColor.getZ() * 255.0f),
        255);
    
    mVertexBufferForLine[1].position = Ogre::Vector3(to.getX(), to.getY(), to.getZ());        
    mVertexBufferForLine[1].color = D3DCOLOR_RGBA(
        static_cast<WORD>(toColor.getX() * 255.0f),
        static_cast<WORD>(toColor.getY() * 255.0f),
        static_cast<WORD>(toColor.getZ() * 255.0f),
        255);   	
	
	UINT primitiveCount = sizeof(mVertexBufferForLine)  / sizeof(D3D9VERTEX) / 2;

	// Create the vertex buffer.
	if( FAILED( pd3dDevice->CreateVertexBuffer( sizeof(mVertexBufferForLine),
		0, D3DFVF_VERTEX,
		D3DPOOL_DEFAULT, &pVB, NULL ) ) )
	{
		return;//E_FAIL;
	}

	// Fill the vertex buffer.
	VOID* pVertices;
	if( FAILED( pVB->Lock( 0, sizeof(mVertexBufferForLine), (void**)&pVertices, 0 ) ) )
		return;// E_FAIL;
	memcpy( pVertices, mVertexBufferForLine, sizeof(mVertexBufferForLine) );
	pVB->Unlock();

	// Render the vertex buffer contents
	pd3dDevice->SetStreamSource( 0, pVB, 0, sizeof(D3D9VERTEX) );
	pd3dDevice->SetFVF( D3DFVF_VERTEX );
	pd3dDevice->DrawPrimitive( D3DPT_LINELIST, 0, primitiveCount );

	pVB->Release();
    
    /*
    glBegin(GL_LINES);
		glColor3f(fromColor.getX(), fromColor.getY(), fromColor.getZ());
		glVertex3d(from.getX(), from.getY(), from.getZ());
		glColor3f(toColor.getX(), toColor.getY(), toColor.getZ());
		glVertex3d(to.getX(), to.getY(), to.getZ());
	glEnd();
    */
}

void	D3d9DebugDrawer::drawLine(const btVector3& from,const btVector3& to,const btVector3& color)
{
	drawLine(from,to,color,color);
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





