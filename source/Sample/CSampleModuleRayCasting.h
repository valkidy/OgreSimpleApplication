#pragma once

#include "COgreHeader.h"

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
    CBulletPhysicManager* m_pBulletPhysicManager;
};
