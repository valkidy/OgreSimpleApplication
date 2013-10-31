#pragma once
          
#include "COgreHeader.h"

class COgreApplication : 
    public OIS::KeyListener,
    public OIS::MouseListener
{
public:
    COgreApplication();
    virtual ~COgreApplication();
    
    bool ApplicationInit();
    void ApplicationTerminate();
    void Run();
    void End() {m_bRun = false;}

    static COgreApplication* sharedApplication();
    
protected:
    bool init();
    void updateOneFrame(double dElapsedTime);
    bool renderOneFrame(double dElapsedTime);

    bool keyPressed(const OIS::KeyEvent &keyEventRef); 
	bool keyReleased(const OIS::KeyEvent &keyEventRef);
 
	bool mouseMoved(const OIS::MouseEvent &evt);
	bool mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id);
	bool mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id);

    Ogre::Root* m_pRoot;
    Ogre::RenderWindow* m_pRenderWindow;
    
    OIS::InputManager* m_pInputMgr;
    OIS::Keyboard* m_pKeyboard;
    OIS::Mouse* m_pMouse;
    
    bool m_bRun;
    Ogre::Timer m_Timer;
    double m_dElapsedTime;

    static COgreApplication* sm_pSharedApplication;

    CPanelManager m_PanelManager;
    COgreModuleManager m_ModuleManager;
};

/* =====================================================================================
 | win32 function
 ===================================================================================== */
HWND CreateWin32Window(int _width, int _height, HINSTANCE hInstance);
static LRESULT CALLBACK WindowProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
