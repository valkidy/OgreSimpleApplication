#include "Win32Application.h"

#include <windows.h>
#define FRAMERATE (1.0f/60.0f)

COgreApplication * COgreApplication::sm_pSharedApplication = 0;

COgreApplication::COgreApplication() :
    m_pRoot(NULL),
    m_pRenderWindow(NULL),    
    m_pInputMgr(NULL),
    m_pKeyboard(NULL),
    m_pMouse(NULL),
    m_bRun(true),
    m_dElapsedTime(0.0)
{
    assert(!sm_pSharedApplication);
    sm_pSharedApplication = this;
}

COgreApplication::~COgreApplication()
{
    assert(this == sm_pSharedApplication);
    sm_pSharedApplication = NULL;
}

COgreApplication* 
COgreApplication::sharedApplication()
{
    assert(sm_pSharedApplication);
    return sm_pSharedApplication;
}

bool 
COgreApplication::init()
{
    if (m_pRoot)
        return false;

    //root
    m_pRoot = new Ogre::Root("plugins_d.cfg");

    //render window
    m_pRoot->setRenderSystem( m_pRoot->getRenderSystemByName("Direct3D9 Rendering Subsystem") );    
    m_pRoot->initialise(false);

    HWND hWnd = CreateWin32Window(1024, 768, GetModuleHandle( NULL ));

    Ogre::NameValuePairList params;
    params["vsync"] = Ogre::StringConverter::toString(false);
    params["displayFrequency"] = Ogre::StringConverter::toString(FRAMERATE);
    params["left"] = Ogre::StringConverter::toString(0);
    params["top"] = Ogre::StringConverter::toString(0);
    params["externalWindowHandle"] = Ogre::StringConverter::toString( (int)hWnd );
	params["border"] = std::string("none");

    m_pRenderWindow = m_pRoot->createRenderWindow("Ogre Render Window", 1024, 768, false, &params);
    assert( m_pRenderWindow );

    //viewport
    //Ogre::Viewport* v = m_pRenderWindow->addViewport(0);
    //v->setBackgroundColour(Ogre::ColourValue(0.5f,0.5f,0.5f,1.0f));
    //v->setCamera(0);

    //input system
    size_t handle = 0;
    OIS::ParamList paramList;
    m_pRenderWindow->getCustomAttribute("WINDOW", &handle);
    paramList.insert(OIS::ParamList::value_type("WINDOW", Ogre::StringConverter::toString(handle)));
    paramList.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND" )));
    paramList.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));
    paramList.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_FOREGROUND")));
    paramList.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_NONEXCLUSIVE")));

    m_pInputMgr = OIS::InputManager::createInputSystem(paramList);
    m_pKeyboard = static_cast<OIS::Keyboard*>(m_pInputMgr->createInputObject(OIS::OISKeyboard, true));
    m_pKeyboard->setEventCallback(this);

    m_pMouse = static_cast<OIS::Mouse*>(m_pInputMgr->createInputObject(OIS::OISMouse, true));
    m_pMouse->getMouseState().width = m_pRenderWindow->getWidth();
    m_pMouse->getMouseState().height = m_pRenderWindow->getHeight();
    m_pMouse->setEventCallback(this);

    //prepare resource
    Ogre::String secName, typeName, archiveName;
    Ogre::ConfigFile cf;
    cf.load("resources_d.cfg");

    Ogre::ConfigFile::SectionIterator iter = cf.getSectionIterator();
    while(iter.hasMoreElements())
    {
        secName = iter.peekNextKey();

        Ogre::ConfigFile::SettingsMultiMap *setting = iter.getNext();
        Ogre::ConfigFile::SettingsMultiMap::iterator i;
        for (i=setting->begin();i!=setting->end();++i)
        {
            typeName = i->first;
            archiveName = i->second;

            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(archiveName, typeName, secName);
        } 
    }

    //mipmap
    Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);

    //init resource 
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

    //activate render window
    m_pRenderWindow->setActive(true);

    return true;
}

bool 
COgreApplication::ApplicationInit()
{
    if (false == init())
        return false;

    m_ModuleManager.Initialise();

    return true;
}

void
COgreApplication::ApplicationTerminate()
{
    m_PanelManager.Finalize();

    if(m_pInputMgr)	
        OIS::InputManager::destroyInputSystem(m_pInputMgr);

    if(m_pRoot)	
        delete m_pRoot;
}

void 
COgreApplication::Run()
{
    m_pRoot->getRenderSystem()->_initRenderTargets();
	m_pRoot->clearEventTimes();
	m_pRenderWindow->setActive(true);

    m_Timer.reset();
    unsigned long timeSinceLastFrame = m_Timer.getMicroseconds();
    unsigned long timeAtThisFrame = timeSinceLastFrame;
    double elapsedTime = 0.0;

    while (m_bRun)
    {
        //pump messages
        Ogre::WindowEventUtilities::messagePump();

        //lock frame rate
        timeAtThisFrame = m_Timer.getMicroseconds();
        double deltaTime = (timeAtThisFrame-timeSinceLastFrame)/1000000.0;
        timeSinceLastFrame = timeAtThisFrame;
	
        m_dElapsedTime += deltaTime;
        
        if (m_dElapsedTime<FRAMERATE)
        {
            DWORD dSleepTime = (DWORD)(FRAMERATE - m_dElapsedTime) * 1000;
            Sleep(dSleepTime);
            continue;
        } // End if

        if (false == m_bRun)
            break;

        elapsedTime = m_dElapsedTime;
        m_dElapsedTime = 0.0;

        //update
        updateOneFrame(elapsedTime);

        //render
	    if (false == renderOneFrame(deltaTime)) 
            break;
    }
}

void 
COgreApplication::updateOneFrame(double dElapsedTime)
{
    //input
    m_pKeyboard->capture();
    m_pMouse->capture();

    //module
    m_ModuleManager.UpdateModule((float)dElapsedTime);
}

bool 
COgreApplication::renderOneFrame(double dElapsedTime)
{
    return m_pRoot->renderOneFrame(dElapsedTime);
}

bool 
COgreApplication::keyPressed(const OIS::KeyEvent &keyEventRef)
{
    return m_ModuleManager.GetCurrentModule()->keyPressed(keyEventRef.key);
}

bool 
COgreApplication::keyReleased(const OIS::KeyEvent &keyEventRef)
{
    return m_ModuleManager.GetCurrentModule()->keyReleased(keyEventRef.key);
}

bool
COgreApplication::mouseMoved(const OIS::MouseEvent &evt)
{
    return m_ModuleManager.GetCurrentModule()->mouseMoved(evt);
}

bool
COgreApplication::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
    return m_ModuleManager.GetCurrentModule()->mousePressed(evt, id);
}

bool
COgreApplication::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
    return m_ModuleManager.GetCurrentModule()->mouseReleased(evt, id);
}
	