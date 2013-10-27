#pragma once

#include "COgreModuleHeader.h"

class CSampleModule :
    public COgreModule,
    public Ogre::RenderQueueListener,
    public Ogre::RenderTargetListener
{
public:
    CSampleModule() {}
    virtual ~CSampleModule() {}

public:
    virtual void createScene();
    virtual void updateScene(float fDeltaTime);
    virtual void destroyScene();

    virtual bool keyPressed(const OIS::KeyCode& iKeyCode);
	virtual bool keyReleased(const OIS::KeyCode& iKeyCode);
	virtual bool mouseMoved(const OIS::MouseEvent &evt);
	virtual bool mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id);
	virtual bool mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id);

    //Implement RenderTargetListener
    void preRenderTargetUpdate(const Ogre::RenderTargetEvent& evt) { LOG("preRenderTargetUpdate");}
    void postRenderTargetUpdate(const Ogre::RenderTargetEvent& evt) { LOG("postRenderTargetUpdate");}
	
	//Implement RenderQueueListener
    void renderQueueStarted(Ogre::uint8 queueGroupId, const Ogre::String&, bool &skipThisInvocation) {LOG("renderQueueStarted");}

protected:
    Ogre::SceneManager* m_pSceneMgr;
    Ogre::Camera* m_pCamera;
    
    bool m_bMoveForward;
    bool m_bMoveBackward;
    bool m_bMoveRight;
    bool m_bMoveLeft;
    bool m_bMoveUp;
    bool m_bMoveDown;

    bool m_bMouseLeft;
};
