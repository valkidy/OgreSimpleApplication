#include "Win32Platform.h"
#include "CSampleModule.h"

void 
CSampleModule::createScene()
{
    LOG("CSampleModule::createScene");

    //default param
    m_bMoveForward = false;
    m_bMoveBackward = false;
    m_bMoveRight = false;
    m_bMoveLeft = false;
    m_bMoveUp = false;
    m_bMoveDown = false;

    m_bMouseLeft = false;

    //default scene manager
    m_pSceneMgr = Ogre::Root::getSingletonPtr()->createSceneManager(Ogre::ST_GENERIC,"DefaultSceneManager");
    assert(m_pSceneMgr);

    //default camera
    m_pSceneMgr->setAmbientLight(Ogre::ColourValue(0.3f, 0.3f, 0.3f));
    m_pSceneMgr->setSkyBox(true, "DefaultSkyBox");

	m_pCamera = m_pSceneMgr->createCamera("DefaultCamera");
	m_pCamera->setFarClipDistance(10000.0f);
	m_pCamera->setNearClipDistance(0.025f);
    m_pCamera->setFOVy(Ogre::Radian(Ogre::Degree(90.0f))); 
	m_pCamera->setAspectRatio(4.0f/3.0f);		
	m_pCamera->setPosition(Ogre::Vector3(0, 5.0f, -30.0f));
	m_pCamera->lookAt(Ogre::Vector3(0,0,0));
    
    //default viewport
    Ogre::RenderWindow* _pRenderWindow = (Ogre::RenderWindow*)Ogre::Root::getSingletonPtr()->getRenderTarget("Ogre Render Window");
    assert(_pRenderWindow);    
    Ogre::Viewport* _pViewport = _pRenderWindow->addViewport(m_pCamera);
    assert(_pViewport);
    _pViewport->setBackgroundColour(Ogre::ColourValue(0.25f,0.25f,0.25f,1.0f));
	
    //build plane
    Ogre::Plane* _pPlane = new Ogre::MovablePlane("Plane");
	_pPlane->d = 0;
	_pPlane->normal = Ogre::Vector3::UNIT_Y;
	
	Ogre::MeshManager::getSingleton().createPlane("PlaneMesh", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, *_pPlane,
		256, 256, 1, 1, true, 1, 1, 1, Ogre::Vector3::UNIT_Z);

    Ogre::Entity* _pPlaneEntity = m_pSceneMgr->createEntity("PlaneEntity", "PlaneMesh");
    assert(_pPlaneEntity);
    _pPlaneEntity->setMaterialName("DefaultPlane");
	Ogre::SceneNode* _pPlaneNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode("PlaneNode");
	_pPlaneNode->attachObject(_pPlaneEntity);
	_pPlaneNode->setPosition(Ogre::Vector3::ZERO);


    //entity
    Ogre::Entity* _pEntity = m_pSceneMgr->createEntity("SinbadEntity", "robot.mesh");
    assert(_pEntity);
    _pEntity->setMaterialName("Examples/Robot");

    Ogre::SceneNode* _pNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode("SinbadNode");
    assert(_pNode);
    _pNode->attachObject(_pEntity);
    _pNode->setScale(0.1f, 0.1f, 0.1f);
    _pNode->setPosition(0, 0, 0);
    _pNode->setVisible(true);

    m_pSceneMgr->addRenderQueueListener(this);
    
    Ogre::RenderTarget *pTarget = m_pCamera->getViewport()->getTarget();
    assert(pTarget);
    pTarget->addListener(this);    
}

void 
CSampleModule::destroyScene()
{
    LOG("CSampleModule::destroyScene");
}

void 
CSampleModule::updateScene(float fDeltaTime)
{
    if (m_pCamera)
    {
        Ogre::Vector3 accel = Ogre::Vector3::ZERO;

        if (m_bMoveForward) accel += m_pCamera->getDirection();
        if (m_bMoveBackward) accel -= m_pCamera->getDirection();
        if (m_bMoveRight) accel += m_pCamera->getRight();
        if (m_bMoveLeft) accel -= m_pCamera->getRight();
        if (m_bMoveUp) accel += m_pCamera->getUp();
        if (m_bMoveDown) accel -= m_pCamera->getUp();

        m_pCamera->move(accel * 10.0f * fDeltaTime);
    } // End if
}

bool 
CSampleModule::keyPressed(const OIS::KeyCode& iKeyCode)
{           
    switch(iKeyCode)
    {
    case OIS::KC_W:
        m_bMoveForward = true;
        break;
    case OIS::KC_S:
        m_bMoveBackward = true;
        break;
    case OIS::KC_A:
        m_bMoveLeft = true;
        break;
    case OIS::KC_D:
        m_bMoveRight = true;
        break;
    }
    
    return true;
}

bool 
CSampleModule::keyReleased(const OIS::KeyCode& iKeyCode)
{
    switch(iKeyCode)
    {
    case OIS::KC_W:
        m_bMoveForward = false;
        break;
    case OIS::KC_S:
        m_bMoveBackward = false;
        break;
    case OIS::KC_A:
        m_bMoveLeft = false;
        break;
    case OIS::KC_D:
        m_bMoveRight = false;
        break;
    case OIS::KC_ESCAPE:
        COgreApplication::sharedApplication()->End();
        break;
    case OIS::KC_Q:
        {
            static bool enable = false;

            Ogre::RenderWindow* _pRenderWindow = (Ogre::RenderWindow*)Ogre::Root::getSingletonPtr()->getRenderTarget("Ogre Render Window");
            Ogre::Viewport* _viewport = _pRenderWindow->getViewport(0);
            Ogre::CompositorManager& compMgr = Ogre::CompositorManager::getSingleton();
            compMgr.addCompositor(_viewport, "B&W");
            compMgr.setCompositorEnabled(_viewport, "B&W", !enable);

            enable = !enable;
        }
        break;
    case OIS::KC_P:
        {
            static bool enable = false;

            Ogre::CompositorManager& compMgr = Ogre::CompositorManager::getSingleton();
            compMgr.addCompositor(m_pCamera->getViewport(), "B&W2");
            compMgr.setCompositorEnabled(m_pCamera->getViewport(), "B&W2", !enable);

            enable = !enable;
        }
        break;
    }

    return true;
}

bool 
CSampleModule::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
    switch(id)
    {
    case OIS::MB_Left:
        m_bMouseLeft = true;
        break;
    case OIS::MB_Right:
        break;
    case OIS::MB_Middle:
        break;
    }

    return true;
}

bool 
CSampleModule::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
    switch(id)
    {
    case OIS::MB_Left:
        m_bMouseLeft = false;
        break;
    case OIS::MB_Right:
        break;
    case OIS::MB_Middle:
        break;
    }

    return true;
}

bool 
CSampleModule::mouseMoved(const OIS::MouseEvent &e)
{
    OIS::MouseEvent orientedEvt((OIS::Object*)e.device, e.state);
	if (orientedEvt.state.Z.rel != 0)
	{
		LOG("mouse wheel Z.rel = %d" ,orientedEvt.state.Z.rel);
	}

    if (m_pCamera && m_bMouseLeft)
    { 
        m_pCamera->yaw(Ogre::Degree(-(float)(e.state.X.rel) * 0.25f));
	    m_pCamera->pitch(Ogre::Degree(-(float)(e.state.Y.rel) * 0.25f));
    }

    return true;
}
