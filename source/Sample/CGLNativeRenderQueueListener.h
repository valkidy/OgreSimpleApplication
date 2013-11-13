#pragma once

#pragma warning(disable: 4010)
#pragma warning(disable: 4251)
#pragma warning(disable: 4996)
#pragma warning(disable: 4193)
#pragma warning(disable: 4275)

#include <Ogre.h>

#include "GlutStuff.h"
#include "GLDebugDrawer.h"
#include "D3d9DebugDrawer.h"
#include "BulletDynamics/Dynamics/btDynamicsWorld.h"

#pragma comment(lib , "opengl32.lib")
#pragma comment(lib , "glut32.lib")
#pragma comment(lib , "glew32s.lib")

/*
simple virtual native render listener
 */
class INativeRenderListener 
{
public:
    virtual void doNativeRender() = 0;
}; // End of INativeRenderListener

/*
using RenderQueueListener and unique pass call native openGL api to draw
Ref:http://www.ogre3d.org/forums/viewtopic.php?p=296902
 */
class CGLNativeRenderSystemRenderQueueListener : public Ogre::RenderQueueListener
{
protected:
    Ogre::MovableObject* mObject;		
	const Ogre::Camera* mCamera;		
	Ogre::SceneManager* mSceneMgr;
    INativeRenderListener* mNativeRenderListener;
public:
    CGLNativeRenderSystemRenderQueueListener(Ogre::MovableObject* object, 
        const Ogre::Camera* camera, Ogre::SceneManager* sceneMgr, INativeRenderListener* listener) :
	  mObject(object),
	  mCamera(camera),
      mSceneMgr(sceneMgr),
      mNativeRenderListener(listener),
      mPass(NULL){ buildPassShader();}
    ~CGLNativeRenderSystemRenderQueueListener() {}
	virtual void renderQueueStarted(Ogre::uint8 queueGroupId, const Ogre::String& invocation, bool& skipThisInvocation) {}
	virtual void renderQueueEnded(Ogre::uint8 queueGroupId, const Ogre::String& invocation, bool& repeatThisInvocation);	

protected:
    void buildPassShader();

    Ogre::HighLevelGpuProgramPtr m_vertProg;
    Ogre::HighLevelGpuProgramPtr m_fragProg;
    Ogre::Pass* mPass;
};

/*
 debug drawer binding bullet object and ogre native openGL render api command as simple immediate mode.
*/
class CGLNativeDebugDrawer : public INativeRenderListener
{    
public:
    CGLNativeDebugDrawer(const Ogre::Camera* camera, Ogre::SceneManager* sceneMgr, btDynamicsWorld* dynamicsWorld);
    ~CGLNativeDebugDrawer();
	
protected:
    virtual void doNativeRender();

    Ogre::RenderQueueListener* mRenderQueueListener;
    Ogre::SceneManager* mSceneMgr;
    btDynamicsWorld* mDynamicsWorld;
    
    D3d9DebugDrawer mDebugDraw;
    //GLDebugDrawer mDebugDraw;    
};
