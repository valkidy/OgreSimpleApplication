#include "CSampleModule.h"

void 
CSampleModule::createScene()
{
    CSimpleModuleTemplate::createScene();

    LOG("CSampleModule::createScene");

    //default camera
    m_pSceneMgr->setAmbientLight(Ogre::ColourValue(0.3f, 0.3f, 0.3f));
    m_pSceneMgr->setSkyBox(true, "DefaultSkyBox");

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
}

void 
CSampleModule::destroyScene()
{
    CSimpleModuleTemplate::destroyScene();

    LOG("CSampleModule::destroyScene");
}

void 
CSampleModule::updateScene(float fDeltaTime)
{
    CSimpleModuleTemplate::updateScene(fDeltaTime);
}