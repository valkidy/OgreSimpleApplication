#pragma once
#include "BulletSoftBody/btSoftRigidDynamicsWorld.h"
#include "BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h"

#include "COgreHeader.h"
    using namespace Ogre;
    
namespace btUtility
{
    /*
    | utiltiy for build bullet
    */
    class CGrassSticks
        : public Ogre::RenderQueueListener
    {
    public:
        bool buildGrassSticks(Ogre::SceneManager* sceneMgr, btDynamicsWorld* dynamicsWorld, btSoftBodyWorldInfo &softBodyWorldInfo);
    
    protected:
        void createGrassMesh();

        // interface
        virtual void renderQueueStarted(Ogre::uint8 queueGroupId, const Ogre::String& invocation, bool& skipThisInvocation) {}
	    virtual void renderQueueEnded(Ogre::uint8 queueGroupId, const Ogre::String& invocation, bool& repeatThisInvocation);
        
        // interface
        static void simulationTickCallback(btDynamicsWorld *dynamicsWorld, btScalar timeStep);
        
        Ogre::StaticGeometry** mField;
        btDynamicsWorld* m_dynamicsWorld;
    }; // End of class   
}; // namespace