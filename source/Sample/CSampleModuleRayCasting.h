#pragma once

#include "COgreHeader.h"

class COgreCharacterController;
class CBulletPhysicManager;
class CSampleModuleRayCasting :
    public CSimpleModuleTemplate
{
public:
    CSampleModuleRayCasting();
    virtual ~CSampleModuleRayCasting() {}

public:
    virtual void createScene();
    virtual void updateScene(float fDeltaTime);
    virtual void destroyScene();

protected:
    virtual bool keyPressed(const OIS::KeyCode& iKeyCode);
    virtual bool keyReleased(const OIS::KeyCode& iKeyCode);
    virtual bool mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id);
    virtual bool mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id);
    virtual bool mouseMoved(const OIS::MouseEvent &evt);
    
    CBulletPhysicManager* m_pBulletPhysicManager;

    COgreCharacterController* m_Char;
};
