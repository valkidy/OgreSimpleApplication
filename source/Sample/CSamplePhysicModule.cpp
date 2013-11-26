#include "CSamplePhysicModule.h"
#include "CSamplePhysicUtility.h"
#include "CCharacterController.h"
#include "CNativeRenderQueueListener.h"

// picking
#include "BulletDynamics/ConstraintSolver/btPoint2PointConstraint.h"
#include "BulletDynamics/ConstraintSolver/btGeneric6DofConstraint.h"

// character controller
#include "BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "BulletDynamics/Character/btKinematicCharacterController.h"

// soft rigid body
#include "BulletSoftBody/btSoftRigidDynamicsWorld.h"
#include "BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h"

#include "Win32Utility.h"

btSoftBodyWorldInfo m_softBodyWorldInfo;

void
CBulletPhysicManager::init()
{
#define SOFT_RIGID_DYNAMICSWORLD 1 
    // Build the broadphase
    //m_broadphase = new btDbvtBroadphase();
    btVector3 worldAabbMin(-1000,-1000,-1000);
	btVector3 worldAabbMax(1000,1000,1000);

	m_broadphase = new btAxisSweep3(worldAabbMin,worldAabbMax,32766);

#ifdef  SOFT_RIGID_DYNAMICSWORLD 
    m_softBodyWorldInfo.m_broadphase = m_broadphase;   
#endif

#ifdef  SOFT_RIGID_DYNAMICSWORLD 
    m_collisionConfiguration = new btSoftBodyRigidBodyCollisionConfiguration();
#else
    // Set up the collision configuration and dispatcher    
    m_collisionConfiguration = new btDefaultCollisionConfiguration();
#endif

    m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);
#ifdef  SOFT_RIGID_DYNAMICSWORLD
    m_softBodyWorldInfo.m_dispatcher = m_dispatcher;
#endif

    // The actual physics solver
    m_solver = new btSequentialImpulseConstraintSolver;
 
#ifdef  SOFT_RIGID_DYNAMICSWORLD 
    btSoftBodySolver* softBodySolver = 0;
    btDiscreteDynamicsWorld* world = new btSoftRigidDynamicsWorld(m_dispatcher,m_broadphase,m_solver,m_collisionConfiguration,softBodySolver);
	m_dynamicsWorld = world;
	//m_dynamicsWorld->setInternalTickCallback(pickingPreTickCallback,this,true);
    m_dynamicsWorld->setGravity(btVector3(0,-10.0f,0));

#else
    // The world.
    m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher,m_broadphase,m_solver,m_collisionConfiguration);        
    m_dynamicsWorld->setGravity(btVector3(0,-10.0f,0));
#endif
    
    
    // init debug drawer
    m_sceneMgr = Ogre::Root::getSingletonPtr()->getSceneManager("DefaultSceneManager");
    m_camera = m_sceneMgr->getCamera("DefaultCamera");
    m_debugDrawer = new CDebugDrawer(m_camera, m_sceneMgr, m_dynamicsWorld);

    // init picker
    m_picker.m_pickConstraint = 0;
    m_picker.m_pickedBody = 0;
    
    // build some regidBody
    //btScalar* data;
    //btUtility::buildHeightFieldTerrainFromImage("terrain257x257.png", m_dynamicsWorld, m_collisionShapes, (void* &)data);
    btUtility::buildBoxShapeArray(m_sceneMgr, m_dynamicsWorld, m_collisionShapes, btVector3(2,2,2));
    btUtility::buildGroundShape(m_sceneMgr, m_dynamicsWorld, m_collisionShapes);
    
    //btTriangleIndexVertexArray* triMesh;
    //btUtility::buildRigidBodyFromOgreEntity(_pEntity, m_dynamicsWorld, m_collisionShapes, (void* &)triMesh);
    //m_triangleMeshes.push_back(triMesh);

/////////////////////////////////////////////////////////////////////////////////////////    
    m_dynamicsWorld->getDispatchInfo().m_enableSPU = true;
	m_dynamicsWorld->setGravity(btVector3(0,-10,0));
	//m_softBodyWorldInfo.m_gravity.setValue(0,-10,0);
	m_softBodyWorldInfo.m_sparsesdf.Initialize();
    m_softBodyWorldInfo.air_density = 1.2f;
    m_softBodyWorldInfo.water_density = 0;
    m_softBodyWorldInfo.water_offset = 0;
    m_softBodyWorldInfo.water_normal = btVector3(0, 0, 0);

/*
    m_softBodyWorldInfo.air_density		=	(btScalar)1.2;
*/
    btUtility::buildSticks(m_sceneMgr, m_dynamicsWorld, m_softBodyWorldInfo);

    /////////////////////////////////////////////////////////	
}

void
CBulletPhysicManager::release()
{
#define SOFT_RIGID_DYNAMICSWORLD 1 
#ifdef  SOFT_RIGID_DYNAMICSWORLD 
    m_softBodyWorldInfo.m_sparsesdf.Reset();
#endif

    //remove picker
    removePickingConstraint();

    //remove the tiranglemeshes
    for (int j=0;j<m_triangleMeshes.size();j++)
    {
  	    btTriangleIndexVertexArray* triMesh = m_triangleMeshes[j];
	    delete triMesh;
    }
    m_triangleMeshes.clear();

    //remove the rigidbodies from the dynamics world and delete them
    int i;
    for (i=m_dynamicsWorld->getNumCollisionObjects()-1; i>=0 ;i--)
    {
	    btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[i];
	    btRigidBody* body = btRigidBody::upcast(obj);
	    if (body && body->getMotionState())
	    {
		    delete body->getMotionState();
	    }
	    m_dynamicsWorld->removeCollisionObject( obj );
	    delete obj;
    }

    //delete collision shapes
    for (int j=0;j<m_collisionShapes.size();j++)
    {
	    btCollisionShape* shape = m_collisionShapes[j];
	    delete shape;
    }
    m_collisionShapes.clear();
    
    //delete debug drawer
    delete m_debugDrawer;

    //delete dynamics world
    delete m_dynamicsWorld;

    //delete solver
    delete m_solver;

    //delete broadphase
    delete m_broadphase;

    //delete dispatcher
    delete m_dispatcher;

    delete m_collisionConfiguration;    
}

void
CBulletPhysicManager::simulate(double dt)
{
    if (m_dynamicsWorld)
    {   
        // LOG("dt : 1/60 = %.3f, %.3f", dt, 1/60.0f);    
        int numSimSteps;
		numSimSteps = m_dynamicsWorld->stepSimulation(dt);

#define SOFT_RIGID_DYNAMICSWORLD 1
#ifdef  SOFT_RIGID_DYNAMICSWORLD                    
        m_softBodyWorldInfo.m_sparsesdf.GarbageCollect();
#endif
        
        int numCollisionObjects = m_dynamicsWorld->getNumCollisionObjects();
        for (int i=0;i<numCollisionObjects;++i)
        {
	        btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[i];
	        btRigidBody* body = btRigidBody::upcast(obj);
	        if (!body)
                continue;

            Ogre::SceneNode* node = static_cast<Ogre::SceneNode*>(body->getUserPointer());
            if (node)
            {
                /// Both ogre entity's root and bullet shape is at center
                /// it might needs to substrate half-height
                const btVector3& localScaling = body->getCollisionShape()->getLocalScaling();
                
                const btTransform& trans = body->getWorldTransform();
                node->setPosition(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ());
                node->setOrientation(trans.getRotation().getW(), trans.getRotation().getX(), trans.getRotation().getY(), trans.getRotation().getZ());
            }
        }
    }
} 

void 
CBulletPhysicManager::rayCasting(float x, float y)
{       
    // for perspective viewport
    btVector3 rayFrom, rayTo;
    btUtility::makeRayCastingSegment(x, y, m_camera, rayFrom, rayTo);

    btCollisionWorld::ClosestRayResultCallback rayCallback(rayFrom,rayTo); 
    if (m_dynamicsWorld)
        m_dynamicsWorld->rayTest(rayFrom, rayTo, rayCallback);
        
    if (rayCallback.hasHit())
    {
        btRigidBody* body = (btRigidBody*)btRigidBody::upcast(rayCallback.m_collisionObject);
	    if (body)
		{
            btVector3 pickPos = rayCallback.m_hitPointWorld;				
            btVector3 localPivot = body->getCenterOfMassTransform().inverse() * pickPos;

            addPickingConstraint(body, localPivot);

            m_picker.m_pickPos = rayTo;
            m_picker.m_rayTo = pickPos;
            m_picker.m_pickDist = (pickPos - rayFrom).length();
        } // End if
    } // End if  
}

void 
CBulletPhysicManager::addPickingConstraint(btRigidBody* body, const btVector3& localPivot)
{   
	if ((!m_picker.m_pickConstraint) && (!(body->isStaticObject() || body->isKinematicObject())))
	{			         
        body->setActivationState(DISABLE_DEACTIVATION);

		btTransform transform;
		transform.setIdentity();
		transform.setOrigin(localPivot);

		btGeneric6DofConstraint* dof6 = new btGeneric6DofConstraint(*body, transform, false);
		dof6->setLinearLowerLimit(btVector3(0,0,0));
		dof6->setLinearUpperLimit(btVector3(0,0,0));
		dof6->setAngularLowerLimit(btVector3(0,0,0));
		dof6->setAngularUpperLimit(btVector3(0,0,0));

		m_dynamicsWorld->addConstraint(dof6, true);
		
        m_picker.m_pickConstraint = dof6;
        m_picker.m_pickedBody = body;
	} 
}

void
CBulletPhysicManager::removePickingConstraint()
{    
    if (m_picker.m_pickConstraint && m_dynamicsWorld)
	{
		m_dynamicsWorld->removeConstraint(m_picker.m_pickConstraint);
		
        delete m_picker.m_pickConstraint;
		m_picker.m_pickConstraint = 0;

        m_picker.m_pickedBody->forceActivationState(ACTIVE_TAG);
		m_picker.m_pickedBody->setDeactivationTime( 0.0f );
		m_picker.m_pickedBody = 0;
	} // End if
}

void 
CBulletPhysicManager::dragPickingConstraint(float x, float y)
{
    if (m_picker.m_pickConstraint)
	{
		if (m_picker.m_pickConstraint->getConstraintType() == D6_CONSTRAINT_TYPE)
		{
			btGeneric6DofConstraint* pickDof6 = static_cast<btGeneric6DofConstraint*>(m_picker.m_pickConstraint);
			if (pickDof6)
			{
	            // for perspective viewport
                btVector3 rayFrom, rayTo;
                btUtility::makeRayCastingSegment(x, y, m_camera, rayFrom, rayTo);

				btVector3 oldPivotInB = pickDof6->getFrameOffsetA().getOrigin();

				btVector3 dir = rayTo - rayFrom;
                dir.normalize();
                dir *= m_picker.m_pickDist;

				btVector3 newPivotB = rayFrom + dir;
				pickDof6->getFrameOffsetA().setOrigin(newPivotB);

                btCollisionShape* shape = m_picker.m_pickedBody->getCollisionShape();
                if (shape)
                {
                    /*
                    btVector3 scale(1, 1, 1); 
                    const btTransform& transform = m_picker.m_pickedBody->getWorldTransform();
                    btScalar scaleRatio = m_picker.m_pickDist / (transform.getOrigin() - rayFrom).length();
                    
                    shape->setLocalScaling(scale*scaleRatio);
                    */
                } // End if                                
			}
		}
    } // End if
}

void CBulletPhysicManager::enableDebug(bool enable)
{
    m_debugDrawer->setEnable(enable);
}
