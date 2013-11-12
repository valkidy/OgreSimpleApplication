#pragma once

#include "btBulletDynamicsCommon.h"

#define ARRAY_SIZE_X 5
#define ARRAY_SIZE_Y 5
#define ARRAY_SIZE_Z 5

#define SCALING 1.
#define START_POS_X -5
#define START_POS_Y 5
#define START_POS_Z -3

namespace Ogre
{
    class SceneManager;
    class Camera;
};

class CGLNativeDebugDrawer;
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
    void rayCasting(float mouse_x, float mouse_y);
    void dragObject(float mouse_x, float mouse_y);

protected:
    void init();
    void release();
    
    void addPickingConstraint(btRigidBody* body, const btVector3& localPivot);
    void removePickingConstraint();

private:
    // bullet shape manage container
    btAlignedObjectArray<btCollisionShape*>	m_collisionShapes;
    
    // bullet 
    btBroadphaseInterface*	m_broadphase;
    btCollisionDispatcher*	m_dispatcher;
    btConstraintSolver*	m_solver;
    btDefaultCollisionConfiguration* m_collisionConfiguration;        
	btDynamicsWorld*		m_dynamicsWorld;

    // bullet picker pack
    btPick m_picker;

    // debug drawer
    CGLNativeDebugDrawer* m_debugDrawer;
      
    // ogre object
    Ogre::SceneManager* m_sceneMgr;
    Ogre::Camera* m_camera;
}; // End of CBulletPhysicManager
