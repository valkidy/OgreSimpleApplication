#pragma once

#include "btBulletDynamicsCommon.h"

namespace Ogre
{
    class SceneManager;
    class Camera;
};

class btKinematicCharacterController;
class btPairCachingGhostObject;

class CDebugDrawer;
class CCharacterController;
class CBulletPhysicManager 
{
    struct btPick
    {
        btTypedConstraint*	m_pickConstraint;
        btRigidBody* m_pickedBody;
        
        btVector3 m_rayTo;
        btVector3 m_pickPos;
        float m_pickDist;
    };

public:
    CBulletPhysicManager() :
        m_broadphase(NULL),
        m_dispatcher(NULL),
        m_solver(NULL),
        m_collisionConfiguration(NULL),
        m_dynamicsWorld(NULL),
        m_sceneMgr(NULL),
        m_camera(NULL) { init();}
    ~CBulletPhysicManager()  {release();}

    void simulate(double dt);
    void rayCasting(float x, float y);
    void addPickingConstraint(btRigidBody* body, const btVector3& localPivot);
    void removePickingConstraint();
    void dragPickingConstraint(float x, float y);

protected:
    void init();
    void release();

private:
    // bullet shape manage container
    btAlignedObjectArray<btCollisionShape*>	m_collisionShapes;
    btAlignedObjectArray<btTriangleIndexVertexArray*> m_triangleMeshes;
    
    // bullet 
    btBroadphaseInterface*	m_broadphase;
    btCollisionDispatcher*	m_dispatcher;
    btConstraintSolver*	m_solver;
    btDefaultCollisionConfiguration* m_collisionConfiguration;        
	btDynamicsWorld*		m_dynamicsWorld;

    // bullet picker pack
    btPick m_picker;

    // debug drawer
    CDebugDrawer* m_debugDrawer;
      
    // ogre object
    Ogre::SceneManager* m_sceneMgr;
    Ogre::Camera* m_camera;

public:
    btDynamicsWorld* getWorld(){return m_dynamicsWorld;}
    void enableDebug(bool enable);
}; // End of CBulletPhysicManager

