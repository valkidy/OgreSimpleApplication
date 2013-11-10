#pragma once

#include "COgreHeader.h"

/*=============================================================================
| Basic parameters panel widget.
=============================================================================*/
class CSimpleModuleTemplate :
    public COgreModule
{
public:
    CSimpleModuleTemplate() : 
        m_pSceneMgr(NULL),
        m_pCamera(NULL) {}
    virtual ~CSimpleModuleTemplate() {}

public:
    virtual void createScene();
    virtual void updateScene(float fDeltaTime);
    virtual void destroyScene();
    
    virtual bool keyPressed(const OIS::KeyCode& iKeyCode);
	virtual bool keyReleased(const OIS::KeyCode& iKeyCode);
	virtual bool mouseMoved(const OIS::MouseEvent &evt);
	virtual bool mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id);
	virtual bool mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id);

protected:
    Ogre::SceneManager* m_pSceneMgr;
    Ogre::Camera* m_pCamera;
    
    bool m_bMoveForward;
    bool m_bMoveBackward;
    bool m_bMoveRight;
    bool m_bMoveLeft;
    bool m_bMoveUp;
    bool m_bMoveDown;

    bool m_bMouseButtonL;
};
