#include "CSampleModuleRayCasting.h"
#include "CSamplePhysicModule.h"
#include "CCharacterController.h"
#include "COgreCharacterController.h"

CSampleModuleRayCasting::CSampleModuleRayCasting() :
    m_pBulletPhysicManager(NULL),
    m_Char(NULL)
{
}

void 
CSampleModuleRayCasting::createScene()
{
    CSimpleModuleTemplate::createScene();

    LOG("CSampleModuleRayCasting::createScene");  

    // set shadow properties
    /*
    m_pSceneMgr->setShadowColour(Ogre::ColourValue(0.5, 0.5, 0.5));
	m_pSceneMgr->setShadowTextureSize(1024);
	m_pSceneMgr->setShadowTextureCount(1);
    m_pSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_MODULATIVE);
    m_pSceneMgr->setShadowUseInfiniteFarPlane(false);
    */

    // use a small amount of ambient lighting
	m_pSceneMgr->setAmbientLight(Ogre::ColourValue(1.0, 1.0, 1.0));

    Ogre::Light* directionalLight = m_pSceneMgr->createLight("directionalLight");
    directionalLight->setType(Ogre::Light::LT_DIRECTIONAL);
    directionalLight->setDiffuseColour(Ogre::ColourValue(.25, .25, 0));
    directionalLight->setSpecularColour(Ogre::ColourValue(.25, .25, 0));
    directionalLight->setDirection(Ogre::Vector3( 0, -1, 1 )); 

	// add a bright light above the scene
    Ogre::Light* pointLight = m_pSceneMgr->createLight("pointLight");
    pointLight->setType(Ogre::Light::LT_POINT);
    pointLight->setPosition(Ogre::Vector3(0, 150, 250)); 
    pointLight->setDiffuseColour(1.0, 1.0, 0.0);
    pointLight->setSpecularColour(1.0, 1.0, 0.0);

    m_pBulletPhysicManager = new CBulletPhysicManager();

    // m_Char = new COgreCharacterController(m_pCamera);
    // m_Char->createCharacterController(m_pBulletPhysicManager->getWorld());
}

void 
CSampleModuleRayCasting::destroyScene()
{
    CSimpleModuleTemplate::destroyScene();

    LOG("CSampleModuleRayCasting::destroyScene");

    if (m_Char)
    {
        delete m_Char;
        m_Char = NULL;
    }

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

    if (m_Char)
        m_Char->addTime(fDeltaTime);

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
CSampleModuleRayCasting::keyPressed(const OIS::KeyCode& iKeyCode)
{
    CSimpleModuleTemplate::keyPressed(iKeyCode);

    if (m_Char)
        m_Char->injectKeyDown(iKeyCode);
    
    return true;
}

bool 
CSampleModuleRayCasting::keyReleased(const OIS::KeyCode& iKeyCode)
{
    CSimpleModuleTemplate::keyReleased(iKeyCode);

    if (m_Char)
        m_Char->injectKeyUp(iKeyCode);

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
    case OIS::KC_SPACE:
        {
            //if (m_pBulletPhysicManager)
            //{
            //    m_pBulletPhysicManager->getCharacter()->jump();                
            //}
        }
        break;
    case OIS::KC_B:
        {
            if (m_pBulletPhysicManager)
            {
                static bool enable = false;

                m_pBulletPhysicManager->enableDebug(enable);

                enable = !enable;
            }
        }
        break;
    }

    //if (m_pBulletPhysicManager)
    //{
    //    m_pBulletPhysicManager->getCharacter()->setLinearVelocity(dir);
    //}
        
    return true;
}
    