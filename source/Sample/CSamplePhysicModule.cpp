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

#include "Win32Utility.h"

///create 125 (5x5x5) dynamic object
#define ARRAY_SIZE_X 5
#define ARRAY_SIZE_Y 5
#define ARRAY_SIZE_Z 5

//maximum number of objects (and allow user to shoot additional boxes)
#define MAX_PROXIES (ARRAY_SIZE_X*ARRAY_SIZE_Y*ARRAY_SIZE_Z + 1024)

///scaling of the objects (0.1 = 20 centimeter boxes )
#define SCALING 1.
#define START_POS_X -5
#define START_POS_Y  5
#define START_POS_Z -3

class btBuildShapeUtility
{
public:
    static void buildGroundShape(btDynamicsWorld* dynamicsWorld, btAlignedObjectArray<btCollisionShape*>& collisionShapes)
    {
        btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0,1,0),1);
        btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0,-1,0)));
        btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0,groundMotionState,groundShape,btVector3(0,0,0));
        btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);
        groundRigidBody->setRestitution(0.0f);
        groundRigidBody->setFriction(1.0f);

        dynamicsWorld->addRigidBody(groundRigidBody);

        collisionShapes.push_back(groundShape);
    }

    static void buildBoxShape(btDynamicsWorld* dynamicsWorld, btAlignedObjectArray<btCollisionShape*>& collisionShapes)
    {        
	    /// Create Dynamic Objects
	    btTransform startTransform;
	    startTransform.setIdentity();

	    btScalar mass(1.f);	
        btVector3 localInertia(0,0,0);	
		
	    float start_x = START_POS_X - ARRAY_SIZE_X/2;
	    float start_y = START_POS_Y;
	    float start_z = START_POS_Z - ARRAY_SIZE_Z/2;

	    for (int k=0;k<ARRAY_SIZE_Y;k++)
	    {
		    for (int i=0;i<ARRAY_SIZE_X;i++)
		    {
			    for(int j = 0;j<ARRAY_SIZE_Z;j++)
			    {
				    startTransform.setOrigin(SCALING*btVector3(
									    btScalar(2.0*i + start_x),
									    btScalar(20+2.0*k + start_y),
									    btScalar(2.0*j + start_z)));

    		        btBoxShape* colShape = new btBoxShape(btVector3(SCALING*1,SCALING*1,SCALING*1));
                    collisionShapes.push_back(colShape);
                    colShape->calculateLocalInertia(mass,localInertia);

				    //using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
				    btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
				    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,colShape,localInertia);
				    btRigidBody* body = new btRigidBody(rbInfo);
    				
				    dynamicsWorld->addRigidBody(body);
			    }
		    }		
        }
    }
};


void
CBulletPhysicManager::init()
{
    // Build the broadphase
    m_broadphase = new btDbvtBroadphase();
 
    // Set up the collision configuration and dispatcher
    m_collisionConfiguration = new btDefaultCollisionConfiguration();
    m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);
 
    // The actual physics solver
    m_solver = new btSequentialImpulseConstraintSolver;
 
    // The world.
    m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher,m_broadphase,m_solver,m_collisionConfiguration);        
    m_dynamicsWorld->setGravity(btVector3(0,-10.0f,0));

    // init debug drawer
    m_sceneMgr = Ogre::Root::getSingletonPtr()->getSceneManager("DefaultSceneManager");
    m_camera = m_sceneMgr->getCamera("DefaultCamera");
    m_debugDrawer = new CDebugDrawer(m_camera, m_sceneMgr, m_dynamicsWorld);

    // init picker
    m_picker.m_pickConstraint = 0;
    m_picker.m_pickedBody = 0;
    
    // create entity
    Ogre::Entity* _pEntity = m_sceneMgr->createEntity("OgreHead", "fish.mesh");
    assert(_pEntity);
    
    Ogre::SceneNode* _pNode = m_sceneMgr->getRootSceneNode()->createChildSceneNode("OgreHeadNode");
    assert(_pNode);
    _pNode->attachObject(_pEntity);    
    _pNode->setPosition(0, 0, 0);
    _pNode->setVisible(true);

    // build some regidBody
    btScalar* data;
    buildHeightFieldTerrainFromImage("terrain.png", m_dynamicsWorld, m_collisionShapes, (void* &)data);
    //btBuildShapeUtility::buildGroundShape(m_dynamicsWorld, m_collisionShapes);
    //btBuildShapeUtility::buildBoxShape(m_dynamicsWorld, m_collisionShapes);

    btTriangleIndexVertexArray* triMesh;
    buildRigidBodyFromOgreEntity(_pEntity, m_dynamicsWorld, m_collisionShapes, (void* &)triMesh);
    m_triangleMeshes.push_back(triMesh);

    btTransform startTransform;
	startTransform.setIdentity();
	startTransform.setOrigin (btVector3(0.0, 4.0, 0.0));
	
    // add character
    //m_character = new CCharacterController(m_dynamicsWorld, startTransform);    
}

void
CBulletPhysicManager::release()
{
    //delete character
    if (m_character)
    {
        delete m_character;
        m_character = NULL;
    }

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
	    m_dynamicsWorld->stepSimulation(dt, 4);
        //m_dynamicsWorld->stepSimulation(1/60.0f, 4);
        

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
    makeRayCastingSegment(x, y, m_camera, rayFrom, rayTo);

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
                makeRayCastingSegment(x, y, m_camera, rayFrom, rayTo);

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