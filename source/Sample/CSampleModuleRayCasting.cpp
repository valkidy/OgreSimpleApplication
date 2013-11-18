#include "CSampleModuleRayCasting.h"
#include "CSamplePhysicModule.h"
#include "CCharacterController.h"

CSampleModuleRayCasting::CSampleModuleRayCasting() :
    m_pBulletPhysicManager(NULL)
{
}

void 
CSampleModuleRayCasting::createScene()
{
    CSimpleModuleTemplate::createScene();

    LOG("CSampleModuleRayCasting::createScene");  

    m_pBulletPhysicManager = new CBulletPhysicManager();
}

void 
CSampleModuleRayCasting::destroyScene()
{
    CSimpleModuleTemplate::destroyScene();

    LOG("CSampleModuleRayCasting::destroyScene");

    if (m_pBulletPhysicManager)
    {
        delete m_pBulletPhysicManager;
        m_pBulletPhysicManager = NULL;
    }
}

void 
CSampleModuleRayCasting::updateScene(float fDeltaTime)
{
    if (m_pBulletPhysicManager)
        m_pBulletPhysicManager->simulate(fDeltaTime);

    CSimpleModuleTemplate::updateScene(fDeltaTime);        
}

bool 
CSampleModuleRayCasting::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
    CSimpleModuleTemplate::mousePressed(evt, id);

    const OIS::MouseState& ms = evt.state;

    if (m_pBulletPhysicManager)
        m_pBulletPhysicManager->rayCasting(ms.X.abs, ms.Y.abs);

    return true;
}

bool 
CSampleModuleRayCasting::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
    CSimpleModuleTemplate::mouseReleased(evt, id);

    if (m_pBulletPhysicManager)
        m_pBulletPhysicManager->removePickingConstraint();

    return true;
}

bool
CSampleModuleRayCasting::mouseMoved(const OIS::MouseEvent &evt)
{
    CSimpleModuleTemplate::mouseMoved(evt);

    const OIS::MouseState& ms = evt.state;

    if (m_pBulletPhysicManager)
        m_pBulletPhysicManager->dragPickingConstraint(ms.X.abs, ms.Y.abs);

    return true;
}

bool 
CSampleModuleRayCasting::keyReleased(const OIS::KeyCode& iKeyCode)
{
    CSimpleModuleTemplate::keyReleased(iKeyCode);

    btVector3 dir;
    dir.setZero();

    switch (iKeyCode)
    {
    case OIS::KC_LEFT:
        dir.setX(-1.0f);
        break;
    case OIS::KC_RIGHT:
        dir.setX(1.0f);
        break;
    }

    if (m_pBulletPhysicManager)
    {
        // m_pBulletPhysicManager->getCharacter()->setLinearVelocity(dir);
    }
        
    return true;
}
    