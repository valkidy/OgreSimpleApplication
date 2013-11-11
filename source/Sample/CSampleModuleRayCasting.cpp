#include "CSampleModuleRayCasting.h"
#include "CSamplePhysicModule.h"

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