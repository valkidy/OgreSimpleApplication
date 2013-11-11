#include "CGLNativeRenderQueueListener.h"

static const std::string g_strManualObjectName = "GLNativeDebugDrawer";
static const std::string g_strOpenGLRenderSystem = "OpenGL Rendering Subsystem";


#define ENABLE_NATIVE_RENDER_VERIFY 0

#if (ENABLE_NATIVE_RENDER_VERIFY)
void doNativeRenderQuads()
{
	glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);

	glScalef(10.0f, 10.0f, 10.0f);

	// sample code from here: http://www.oreillynet.com/network/2000/06/23/magazine/cube.c
			
	// quads.
	glBegin(GL_QUADS); 

    // bottom face
	glNormal3f( 0.0f, -1.0f, 0.0f); // Needed for lighting
	glColor4f(0.9f, 0.2f, 0.2f, 0.75f); // Basic polygon color

	glTexCoord2f(0.800f, 0.800f); glVertex3f(-1.0f, -1.0f, -1.0f); 
	glTexCoord2f(0.200f, 0.800f); glVertex3f( 1.0f, -1.0f, -1.0f);
	glTexCoord2f(0.200f, 0.200f); glVertex3f( 1.0f, -1.0f,  1.0f);
	glTexCoord2f(0.800f, 0.200f); glVertex3f(-1.0f, -1.0f,  1.0f);


    // top face
	glNormal3f( 0.0f, 1.0f, 0.0f);  glColor4f(0.5f, 0.5f, 0.5f, 0.5f);

	glTexCoord2f(0.005f, 1.995f); glVertex3f(-1.0f,  1.3f, -1.0f);
	glTexCoord2f(0.005f, 0.005f); glVertex3f(-1.0f,  1.3f,  1.0f);
	glTexCoord2f(1.995f, 0.005f); glVertex3f( 1.0f,  1.3f,  1.0f);
	glTexCoord2f(1.995f, 1.995f); glVertex3f( 1.0f,  1.3f, -1.0f);


	// back face
	glNormal3f( 0.0f, 0.0f,-1.0f);  glColor4f(0.2f, 0.9f, 0.2f, 0.5f); 

	glTexCoord2f(0.995f, 0.005f); glVertex3f(-1.0f, -1.0f, -1.3f);
	glTexCoord2f(2.995f, 2.995f); glVertex3f(-1.0f,  1.0f, -1.3f);
	glTexCoord2f(0.005f, 0.995f); glVertex3f( 1.0f,  1.0f, -1.3f);
	glTexCoord2f(0.005f, 0.005f); glVertex3f( 1.0f, -1.0f, -1.3f);


    // right face
	glNormal3f( 1.0f, 0.0f, 0.0f);  glColor4f(0.2f, 0.2f, 0.9f, 0.25f);

	glTexCoord2f(0.995f, 0.005f); glVertex3f( 1.0f, -1.0f, -1.0f); 
	glTexCoord2f(0.995f, 0.995f); glVertex3f( 1.0f,  1.0f, -1.0f);
	glTexCoord2f(0.005f, 0.995f); glVertex3f( 1.0f,  1.0f,  1.0f);
	glTexCoord2f(0.005f, 0.005f); glVertex3f( 1.0f, -1.0f,  1.0f);


	// front face
	glNormal3f( 0.0f, 0.0f, 1.0f); 

	glColor4f( 0.9f, 0.2f, 0.2f, 0.5f);
	glTexCoord2f( 0.005f, 0.005f); glVertex3f(-1.0f, -1.0f,  1.3f);
	glColor4f( 0.2f, 0.9f, 0.2f, 0.5f);
	glTexCoord2f( 0.995f, 0.005f); glVertex3f( 1.0f, -1.0f,  1.3f);
	glColor4f( 0.2f, 0.2f, 0.9f, 0.5f);
	glTexCoord2f( 0.995f, 0.995f); glVertex3f( 1.0f,  1.0f,  1.3f); 
	glColor4f( 0.1f, 0.1f, 0.1f, 0.5f);
	glTexCoord2f( 0.005f, 0.995f); glVertex3f(-1.0f,  1.0f,  1.3f);

	// left face
	glNormal3f(-1.0f, 0.0f, 0.0f);  

	glColor4f(0.9f, 0.9f, 0.2f, 0.0f);
	glTexCoord2f(0.005f, 0.005f); glVertex3f(-1.3f, -1.0f, -1.0f); 
	glColor4f(0.9f, 0.9f, 0.2f, 0.66f);
	glTexCoord2f(0.995f, 0.005f); glVertex3f(-1.3f, -1.0f,  1.0f);
	glColor4f(0.9f, 0.9f, 0.2f, 1.0f);
	glTexCoord2f(0.995f, 0.995f); glVertex3f(-1.3f,  1.0f,  1.0f);
	glColor4f(0.9f, 0.9f, 0.2f, 0.33f);
	glTexCoord2f(0.005f, 0.995f); glVertex3f(-1.3f,  1.0f, -1.0f);

	// All polygons have been drawn.
	glEnd();	
}
#endif

/* -------------------------------------------------------------------
 | implement of CGLNativeRenderSystemRenderQueueListener
 --------------------------------------------------------------------- */
void 
CGLNativeRenderSystemRenderQueueListener::buildPassShader()
{
    Ogre::RenderSystem* renderSystem = mSceneMgr->getDestinationRenderSystem();

    const Ogre::String& RenderSystemName = renderSystem->getName();    
    bool capability = renderSystem->getCapabilities()->hasCapability(Ogre::RSC_FIXED_FUNCTION);
    if ((!capability) && RenderSystemName == g_strOpenGLRenderSystem)
    {
        Ogre::String strVP = 
        "#version 100\n  \
        precision mediump int;\n \
        precision mediump float;\n  \
        attribute vec4 position;\n  \
        attribute vec4 uv0;\n  \
        attribute vec4 colour;\n  \
        varying vec4 outUV0;\n \
        varying vec4 vexColor;\n  \
        void main()\n  \
        { \n \
            gl_Position = position;\n \
            outUV0 = uv0;\n \
            vexColor = colour;\n  \
        }\n ";
        
        Ogre::String strFP = 
        "#version 100\n  \
        precision mediump int;\n  \
        precision mediump float;\n  \
        uniform sampler2D sampler;\n  \
        varying vec4 outUV0;\n  \
        varying vec4 vexColor;\n  \
        void main()\n  \
        { \n \
            vec4 texColor = texture2D(sampler, outUV0.xy);\n  \
            gl_FragColor = (texColor*vexColor);\n \
        }\n ";
        
        m_vertProg = Ogre::HighLevelGpuProgramManager::getSingleton().createProgram("SimpleTextureNoLightVP", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, "glsles", Ogre::GPT_VERTEX_PROGRAM);
        m_vertProg->setSource(strVP);
        m_vertProg->load();
        
        m_fragProg = Ogre::HighLevelGpuProgramManager::getSingleton().createProgram("SimpleTextureNoLightFP", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, "glsles", Ogre::GPT_FRAGMENT_PROGRAM);
        m_fragProg->setSource(strFP);
        m_fragProg->load(); 
    } // End if

    
	if (!mPass)
	{
		Ogre::MaterialPtr clearMat = Ogre::MaterialManager::getSingleton().getByName("BaseWhite");
		mPass = clearMat->getTechnique(0)->getPass(0);
	} // End if        
}

void 
CGLNativeRenderSystemRenderQueueListener::renderQueueEnded(Ogre::uint8 queueGroupId, const Ogre::String& invocation, bool& repeatThisInvocation)
{
	// Set wanted render queue here - make sure there are - make sure that something is on
	// this queue - else you will never pass this if.
    if (queueGroupId != Ogre::RENDER_QUEUE_MAIN) 
        return;

	// save matrices
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glMatrixMode(GL_TEXTURE);
	glPushMatrix();
	glLoadIdentity(); //Texture addressing should start out as direct.

	Ogre::RenderSystem* renderSystem = mObject->_getManager()->getDestinationRenderSystem();
	Ogre::Node* parentNode = mObject->getParentNode();
	renderSystem->_setWorldMatrix(parentNode->_getFullTransform());
	renderSystem->_setViewMatrix(mCamera->getViewMatrix());
	renderSystem->_setProjectionMatrix(mCamera->getProjectionMatrixRS());

    if(renderSystem->isGpuProgramBound(Ogre::GPT_VERTEX_PROGRAM))
        renderSystem->unbindGpuProgram(Ogre::GPT_VERTEX_PROGRAM);
    if(renderSystem->isGpuProgramBound(Ogre::GPT_FRAGMENT_PROGRAM))
        renderSystem->unbindGpuProgram(Ogre::GPT_FRAGMENT_PROGRAM);
    if (!m_vertProg.isNull())
        renderSystem->bindGpuProgram(m_vertProg->_getBindingDelegate());
    if (!m_fragProg.isNull())
        renderSystem->bindGpuProgram(m_fragProg->_getBindingDelegate());
    
	//Set a clear pass to give the renderer a clear renderstate
	mSceneMgr->_setPass(mPass, true, false);

	GLboolean depthTestEnabled=glIsEnabled(GL_DEPTH_TEST);
	glDisable(GL_DEPTH_TEST);
	GLboolean stencilTestEnabled = glIsEnabled(GL_STENCIL_TEST);
	glDisable(GL_STENCIL_TEST);

	// save attribs
	glPushAttrib(GL_ALL_ATTRIB_BITS);

    // call native rendering function

#if (ENABLE_NATIVE_RENDER_VERIFY)
    doNativeRenderQuads();
#else
    if (mNativeRenderListener)
    {
        mNativeRenderListener->doNativeRender();
    }				
#endif

	// restore original state
	glPopAttrib();

	if (depthTestEnabled)
	{
		glEnable(GL_DEPTH_TEST);
	}
	if (stencilTestEnabled)
	{
		glEnable(GL_STENCIL_TEST);
	}

	// restore matrices
	glMatrixMode(GL_TEXTURE);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

/* -------------------------------------------------------------------
 | implement of CGLNativeDebugDrawer
 --------------------------------------------------------------------- */
CGLNativeDebugDrawer::CGLNativeDebugDrawer(const Ogre::Camera* camera, Ogre::SceneManager* sceneMgr, btDynamicsWorld* dynamicsWorld) :
    mSceneMgr(sceneMgr),
    mDynamicsWorld(dynamicsWorld)
{
    assert(camera && sceneMgr && dynamicsWorld);

    Ogre::String RenderSystemName = sceneMgr->getDestinationRenderSystem()->getName();
    if (g_strOpenGLRenderSystem != RenderSystemName)
    {
        OGRE_EXCEPT(Ogre::Exception::ERR_NOT_IMPLEMENTED, 
				"Native debug drawer only support OpenGL render system.",
				"CGLNativeDebugDrawer::CGLNativeDebugDrawer");
        return;
    } // End if

    //use this Manual Object as a reference point for the native rendering
    Ogre::ManualObject *manObj; 
    manObj = sceneMgr->createManualObject(g_strManualObjectName);
	
    // Attach to child of root node, better for culling (otherwise bounds are the combination of the 2)
    sceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(manObj);
    
	mRenderQueueListener = new CGLNativeRenderSystemRenderQueueListener(manObj, camera, sceneMgr, this);
	sceneMgr->addRenderQueueListener(mRenderQueueListener);	    
    
    // debug flag
    mDebugDraw.setDebugMode(btIDebugDraw::DBG_EnableCCD | btIDebugDraw::DBG_DrawWireframe);
    dynamicsWorld->setDebugDrawer(&mDebugDraw);
}

CGLNativeDebugDrawer::~CGLNativeDebugDrawer()
{
    // Detach Manual Object
    if (mSceneMgr->hasManualObject(g_strManualObjectName))
    {
        Ogre::ManualObject *manObj = mSceneMgr->getManualObject(g_strManualObjectName); 
        manObj->detachFromParent();

        mSceneMgr->destroyManualObject(g_strManualObjectName);
    } // End if

    // remove render queue listener
    if (mRenderQueueListener)
    {
        mSceneMgr->removeRenderQueueListener(mRenderQueueListener);

        delete mRenderQueueListener;
        mRenderQueueListener = NULL;
    }
}

void 
CGLNativeDebugDrawer::doNativeRender()
{
     if (mDynamicsWorld)
     {         
         glDisable(GL_TEXTURE_2D);
	     glDisable(GL_LIGHTING);

         mDynamicsWorld->debugDrawWorld();
     } // End if
} 