#include "CNativeRenderQueueListener.h"
#include "GLDebugDrawer.h"
#include "D3d9DebugDrawer.h"
#include "D3d9NativeRender.h"
#include "GLNativeRender.h"

#include <stdio.h>

static const std::string g_manualObjectName = "NativeDebugDrawer";
static const std::string g_openGLRenderSystem = "OpenGL Rendering Subsystem";
static const std::string g_d3d9RenderSystem = "Direct3D9 Rendering Subsystem";

using namespace Ogre;

/* -------------------------------------------------------------------
 | implement of CGLNativeRenderSystemRenderQueueListener
 --------------------------------------------------------------------- */
CNativeRenderSystemRenderQueueListener::CNativeRenderSystemRenderQueueListener(
    Ogre::MovableObject* object, const Ogre::Camera* camera, Ogre::SceneManager* sceneMgr, 
    btDynamicsWorld* dynamicsWorld, INativeRender* nativeDebugRender) :
    mObject(object),
    mCamera(camera),
    mSceneMgr(sceneMgr),
    mDynamicsWorld(dynamicsWorld),
    mNativeRender(nativeDebugRender),
    mPass(NULL) 
{
    initNativeRender();
}

void 
CNativeRenderSystemRenderQueueListener::initNativeRender()
{
    if (mNativeRender)
        mNativeRender->initRender();

/*
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
*/
    
	if (!mPass)
	{
		Ogre::MaterialPtr clearMat = Ogre::MaterialManager::getSingleton().getByName("BaseWhite");
		mPass = clearMat->getTechnique(0)->getPass(0);
	} // End if        
}

void
CNativeRenderSystemRenderQueueListener::releaseNativeRender()
{ 
    mNativeRender = NULL;
}

void 
CNativeRenderSystemRenderQueueListener::renderQueueEnded(Ogre::uint8 queueGroupId, const Ogre::String& invocation, bool& repeatThisInvocation)
{ 
	// Set wanted render queue here - make sure there are - make sure that something is on
	// this queue - else you will never pass this if.
    if (queueGroupId != Ogre::RENDER_QUEUE_MAIN) 
        return;

    Ogre::RenderSystem* renderSystem = mObject->_getManager()->getDestinationRenderSystem();
	Ogre::Node* parentNode = mObject->getParentNode();
	renderSystem->_setWorldMatrix(parentNode->_getFullTransform());
	renderSystem->_setViewMatrix(mCamera->getViewMatrix());
	renderSystem->_setProjectionMatrix(mCamera->getProjectionMatrixRS());

    // clear shader
    if (renderSystem->isGpuProgramBound(Ogre::GPT_VERTEX_PROGRAM))
        renderSystem->unbindGpuProgram(Ogre::GPT_VERTEX_PROGRAM);
    if (renderSystem->isGpuProgramBound(Ogre::GPT_FRAGMENT_PROGRAM))
        renderSystem->unbindGpuProgram(Ogre::GPT_FRAGMENT_PROGRAM);
    // assgin shader
    if (!mVertProg.isNull())
        renderSystem->bindGpuProgram(mVertProg->_getBindingDelegate());
    if (!mFragProg.isNull())
        renderSystem->bindGpuProgram(mFragProg->_getBindingDelegate());
    
	//Set a clear pass to give the renderer a clear renderstate
	mSceneMgr->_setPass(mPass, true, false);

    // preRender
    mNativeRender->preRender();

    // doNativeRender
    mDynamicsWorld->debugDrawWorld();
    
    // postRender
    mNativeRender->postRender();
}

/* -------------------------------------------------------------------
 | implement of CGLNativeDebugDrawer
 --------------------------------------------------------------------- */
CDebugDrawer::CDebugDrawer(const Ogre::Camera* camera, Ogre::SceneManager* sceneMgr, btDynamicsWorld* dynamicsWorld) :
    mSceneMgr(sceneMgr)
{
    assert(camera && sceneMgr && dynamicsWorld);

    Ogre::String renderSystemName = sceneMgr->getDestinationRenderSystem()->getName();    
    if (g_d3d9RenderSystem == renderSystemName) 
    {
        mDebugDraw = new D3d9DebugDrawer();
    }
    else if (g_openGLRenderSystem == renderSystemName) 
    {
        mDebugDraw = new GLDebugDrawer();
    }
    else   
    {
        // not upport
        printf("Native debug drawer not support current render system, CNativeDebugDrawer::CNativeDebugDrawer \n");        
    } // End if

    if (mDebugDraw)
    {
        // Use this Manual Object as a reference point for the native rendering
        Ogre::ManualObject *manObj; 
        manObj = sceneMgr->createManualObject(g_manualObjectName);
    	
        // Attach to child of root node, better for culling (otherwise bounds are the combination of the 2)
        sceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(manObj);

        // Create render listener
	    mRenderQueueListener = new CNativeRenderSystemRenderQueueListener(manObj, camera, sceneMgr, dynamicsWorld, mDebugDraw);
	    sceneMgr->addRenderQueueListener(mRenderQueueListener);	

        // Don't receive shadows
        manObj->setCastShadows(false);

        // Set debugDrawer
        mDebugDraw->setDebugMode(btIDebugDraw::DBG_EnableCCD | btIDebugDraw::DBG_DrawWireframe);        
        dynamicsWorld->setDebugDrawer(static_cast<btIDebugDraw*>(mDebugDraw));
    } // End if
}

CDebugDrawer::~CDebugDrawer()
{
    // Detach Manual Object
    if (mSceneMgr->hasManualObject(g_manualObjectName))
    {
        Ogre::ManualObject *manObj = mSceneMgr->getManualObject(g_manualObjectName); 
        manObj->detachFromParent();

        mSceneMgr->destroyManualObject(g_manualObjectName);
    } // End if

    // Remove render queue listener
    if (mRenderQueueListener)
    {
        mSceneMgr->removeRenderQueueListener(mRenderQueueListener);

        delete mRenderQueueListener;
        mRenderQueueListener = NULL;
    }

    // Release debugDraw 
    if (mDebugDraw)
    {
        delete mDebugDraw;
        mDebugDraw = NULL;
    }
}


void 
CDebugDrawer::setEnable(bool enable)
{
    if (!mRenderQueueListener)
        return;
    
    if (enable)
        mSceneMgr->addRenderQueueListener(mRenderQueueListener);
    else
        mSceneMgr->removeRenderQueueListener(mRenderQueueListener);        
}