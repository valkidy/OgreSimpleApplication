#pragma once
#include "BulletSoftBody/btSoftRigidDynamicsWorld.h"
#include "BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h"

#include "COgreHeader.h"
    
namespace btUtility
{
    /*
    | utiltiy for build bullet grass and ogre entity
    */
    class CGrassSticks
    {
    public:
        bool buildGrassSticks(Ogre::SceneManager* sceneMgr, btDynamicsWorld* dynamicsWorld, btSoftBodyWorldInfo &softBodyWorldInfo);
        bool buildStaticGrassSticks(Ogre::SceneManager* sceneMgr);

    protected:
        void createGrassMesh();
        
        // interface
        static void simulationTickCallback(btDynamicsWorld *dynamicsWorld, btScalar timeStep);
        
        Ogre::StaticGeometry* mField;
        btDynamicsWorld* m_dynamicsWorld;
    }; // End of class   
}; // namespace
