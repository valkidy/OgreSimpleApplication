#pragma once

#include "btBulletDynamicsCommon.h"

#define ARRAY_SIZE_X 5
#define ARRAY_SIZE_Y 5
#define ARRAY_SIZE_Z 5

#define SCALING 1.
#define START_POS_X -5
#define START_POS_Y 5
#define START_POS_Z -3

class CGLNativeDebugDrawer;
class CBulletPhysicManager 
{
public:
    CBulletPhysicManager() :
        m_broadphase(NULL),
        m_dispatcher(NULL),
        m_solver(NULL),
        m_collisionConfiguration(NULL),
        m_dynamicsWorld(NULL),
        m_pickConstraint(NULL) { init();}
    ~CBulletPhysicManager()  {release();}

    void simulate(double dt);
    
protected:
    void init();
    void release();
    
private:
    btAlignedObjectArray<btCollisionShape*>	m_collisionShapes;
    
    btBroadphaseInterface*	m_broadphase;
    btCollisionDispatcher*	m_dispatcher;
    btConstraintSolver*	m_solver;
    btDefaultCollisionConfiguration* m_collisionConfiguration;
        
	btDynamicsWorld*		m_dynamicsWorld;
    // for mouse picking
	btTypedConstraint*		m_pickConstraint;
   
    CGLNativeDebugDrawer* m_debugDrawer;
}; // End of CBulletPhysicManager