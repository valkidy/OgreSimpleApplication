#pragma once

class COgreModule
{
public:
    COgreModule() {}
    virtual ~COgreModule() {}
    
    //scene
    virtual void createScene() = 0;
    virtual void updateScene(float fDeltaTime) = 0;
    virtual void destroyScene() = 0;    

    //control
    virtual bool keyPressed(const OIS::KeyCode& iKeyCode) = 0;
	virtual bool keyReleased(const OIS::KeyCode& iKeyCode) = 0;
	virtual bool mouseMoved(const OIS::MouseEvent &evt) = 0;
	virtual bool mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id) = 0;
	virtual bool mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id) = 0;
};


