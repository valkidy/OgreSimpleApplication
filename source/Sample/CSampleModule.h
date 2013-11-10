#pragma once

#include "COgreHeader.h"

/*=============================================================================
| Basic parameters panel widget.
=============================================================================*/
class CSampleModule :
    public CSimpleModuleTemplate,
    public Ogre::RenderQueueListener,
    public Ogre::RenderTargetListener
{
public:
    CSampleModule() {}
    virtual ~CSampleModule() {}

public:
    virtual void createScene();
    virtual void updateScene(float fDeltaTime);
    virtual void destroyScene();

    //Implement RenderTargetListener
    void preRenderTargetUpdate(const Ogre::RenderTargetEvent& evt) { LOG("preRenderTargetUpdate");}
    void postRenderTargetUpdate(const Ogre::RenderTargetEvent& evt) { LOG("postRenderTargetUpdate");}
	
	//Implement RenderQueueListener
    void renderQueueStarted(Ogre::uint8 queueGroupId, const Ogre::String&, bool &skipThisInvocation) {LOG("renderQueueStarted");}

protected:
};
