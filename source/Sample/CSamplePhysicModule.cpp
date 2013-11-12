#include "CSamplePhysicModule.h"
#include "CSamplePhysicUtility.h"
#include "CGLNativeRenderQueueListener.h"

#include "BulletDynamics/ConstraintSolver/btPoint2PointConstraint.h"//picking
#include "BulletDynamics/ConstraintSolver/btGeneric6DofConstraint.h"//picking

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
        dynamicsWorld->addRigidBody(groundRigidBody);

        collisionShapes.push_back(groundShape);
    }

    static void buildBoxShape(btDynamicsWorld* dynamicsWorld, btAlignedObjectArray<btCollisionShape*>& collisionShapes)
    {
        btBoxShape* colShape = new btBoxShape(btVector3(SCALING*1,SCALING*1,SCALING*1));
        collisionShapes.push_back(colShape);
        
	    /// Create Dynamic Objects
	    btTransform startTransform;
	    startTransform.setIdentity();

	    btScalar mass(1.f);	
        btVector3 localInertia(0,0,0);	
		colShape->calculateLocalInertia(mass,localInertia);

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
    m_dynamicsWorld->setGravity(btVector3(0,-10,0));

    // init debug drawer
    m_sceneMgr = Ogre::Root::getSingletonPtr()->getSceneManager("DefaultSceneManager");
    m_camera = m_sceneMgr->getCamera("DefaultCamera");
    m_debugDrawer = new CGLNativeDebugDrawer(m_camera, m_sceneMgr, m_dynamicsWorld);

    // init picker
    m_picker.m_pickConstraint = 0;
    m_picker.m_pickedBody = 0;

    // build some regidBody
    btBuildShapeUtility::buildGroundShape(m_dynamicsWorld, m_collisionShapes);
    btBuildShapeUtility::buildBoxShape(m_dynamicsWorld, m_collisionShapes);
}

void
CBulletPhysicManager::release()
{
    //remove picker
    removePickingConstraint();

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
	    m_dynamicsWorld->stepSimulation(dt);
    }
} 

void 
CBulletPhysicManager::rayCasting(float mouse_x, float mouse_y)
{       
    // for perspective viewport
    btVector3 rayFrom, rayTo;
    makeRayCastingSegment(mouse_x, mouse_y, m_camera, rayFrom, rayTo);

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
        } // End if
    } // End if
    else
        removePickingConstraint();
}

void
CBulletPhysicManager::dragObject(float x, float y)
{
    /*
    if (m_pickConstraint)
	{
		//move the constraint pivot

		if (m_pickConstraint->getConstraintType() == D6_CONSTRAINT_TYPE)
		{
			btGeneric6DofConstraint* pickCon = static_cast<btGeneric6DofConstraint*>(m_pickConstraint);
			if (pickCon)
			{
				//keep it at the same picking distance

				btVector3 rayFrom;
				btVector3 oldPivotInB = pickCon->getFrameOffsetA().getOrigin();

				btVector3 newPivotB;
				{
                    rayFrom = ConvertOgreVectorTobtVector(m_camera->getPosition());
                    btVector3 dir = ConvertOgreVectorTobtVector(m_camera->getDirection());
					dir.normalize();
					// dir *= gOldPickingDist;

					newPivotB = rayFrom + dir;
				}
				pickCon->getFrameOffsetA().setOrigin(newPivotB);
			}

		}         
        else
		{
			btPoint2PointConstraint* pickCon = static_cast<btPoint2PointConstraint*>(m_pickConstraint);
			if (pickCon)
			{
				//keep it at the same picking distance

				btVector3 newRayTo = getRayTo(x,y);
				btVector3 rayFrom;
				btVector3 oldPivotInB = pickCon->getPivotInB();
				btVector3 newPivotB;

				{
					rayFrom = m_cameraPosition;
					btVector3 dir = newRayTo-rayFrom;
					dir.normalize();
					dir *= gOldPickingDist;

					newPivotB = rayFrom + dir;
				}
				pickCon->setPivotB(newPivotB);
			}
           
		}
	}*/

    /*
	float dx, dy;
    dx = btScalar(x) - m_mouseOldX;
    dy = btScalar(y) - m_mouseOldY;
    */

    /*
	///only if ALT key is pressed (Maya style)
	if (m_modifierKeys& BT_ACTIVE_ALT)
	{
		if(m_mouseButtons & 2)
		{
			btVector3 hor = getRayTo(0,0)-getRayTo(1,0);
			btVector3 vert = getRayTo(0,0)-getRayTo(0,1);
			btScalar multiplierX = btScalar(0.001);
			btScalar multiplierY = btScalar(0.001);
			if (m_ortho)
			{
				multiplierX = 1;
				multiplierY = 1;
			}


			m_cameraTargetPosition += hor* dx * multiplierX;
			m_cameraTargetPosition += vert* dy * multiplierY;
		}

		if(m_mouseButtons & (2 << 2) && m_mouseButtons & 1)
		{
		}
		else if(m_mouseButtons & 1) 
		{
			m_azi += dx * btScalar(0.2);
			m_azi = fmodf(m_azi, btScalar(360.f));
			m_ele += dy * btScalar(0.2);
			m_ele = fmodf(m_ele, btScalar(180.f));
		} 
		else if(m_mouseButtons & 4) 
		{
			m_cameraDistance -= dy * btScalar(0.02f);
			if (m_cameraDistance<btScalar(0.1))
				m_cameraDistance = btScalar(0.1);

			
		} 
	}


	m_mouseOldX = x;
    m_mouseOldY = y;
    */
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