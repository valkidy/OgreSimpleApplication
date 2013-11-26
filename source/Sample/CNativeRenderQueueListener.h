#pragma once

#pragma warning(disable: 4010)
#pragma warning(disable: 4251)
#pragma warning(disable: 4996)
#pragma warning(disable: 4193)
#pragma warning(disable: 4275)

#include <Ogre.h>


#include "LinearMath/btIDebugDraw.h"
#include "BulletDynamics/Dynamics/btDynamicsWorld.h"

/*
    simple virtual native render
    - interface of render state 
    - interface of bullet debug drawer
 */
class INativeRender : public btIDebugDraw
{
public:
    virtual void initRender() = 0;
    virtual void preRender() = 0;
    virtual void postRender() = 0;

    virtual void doNativeRender() {}
}; // End of INativeRenderListener

/*
    using RenderQueueListener and unique pass call native openGL api to draw
    Ref:http://www.ogre3d.org/forums/viewtopic.php?p=296902
 */
class CNativeRenderSystemRenderQueueListener : public Ogre::RenderQueueListener
{
public:
    CNativeRenderSystemRenderQueueListener(Ogre::MovableObject* object, const Ogre::Camera* camera, 
        Ogre::SceneManager* sceneMgr, btDynamicsWorld* dynamicsWorld, INativeRender* nativeDebugRender);	  
      ~CNativeRenderSystemRenderQueueListener() {releaseNativeRender();}
	virtual void renderQueueStarted(Ogre::uint8 queueGroupId, const Ogre::String& invocation, bool& skipThisInvocation) {}
	virtual void renderQueueEnded(Ogre::uint8 queueGroupId, const Ogre::String& invocation, bool& repeatThisInvocation);	

protected:
    void initNativeRender();
    void releaseNativeRender();
    
protected:
    Ogre::MovableObject* mObject;		
	const Ogre::Camera* mCamera;		
	Ogre::SceneManager* mSceneMgr;
    btDynamicsWorld* mDynamicsWorld;

    INativeRender* mNativeRender;
    
    Ogre::HighLevelGpuProgramPtr mVertProg;
    Ogre::HighLevelGpuProgramPtr mFragProg;
    Ogre::Pass* mPass;
};

/*
    debug drawer binding bullet world and ogre native render api command as simple immediate mode.
*/
class CDebugDrawer
{    
public:
    CDebugDrawer(const Ogre::Camera* camera, Ogre::SceneManager* sceneMgr, btDynamicsWorld* dynamicsWorld);
    ~CDebugDrawer();
	
    void setEnable(bool enable=true);

protected:                
    Ogre::RenderQueueListener* mRenderQueueListener;
    Ogre::SceneManager* mSceneMgr;
        
    INativeRender* mDebugDraw;
};
