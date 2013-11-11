#include "CSamplePhysicModule.h"
#include "CGLNativeRenderQueueListener.h"

#include "BulletDynamics/ConstraintSolver/btPoint2PointConstraint.h"//picking
#include "BulletDynamics/ConstraintSolver/btGeneric6DofConstraint.h"//picking
    
///create 125 (5x5x5) dynamic object
#define ARRAY_SIZE_X 5
#define ARRAY_SIZE_Y 5
#define ARRAY_SIZE_Z 5

//maximum number of objects (and allow user to shoot additional boxes)
#define MAX_PROXIES (ARRAY_SIZE_X*ARRAY_SIZE_Y*ARRAY_SIZE_Z + 1024)

///scaling of the objects (0.1 = 20 centimeter boxes )
#define SCALING 1.
#define START_POS_X -5
#define START_POS_Y 5
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
    Ogre::SceneManager* _sceneMgr = Ogre::Root::getSingletonPtr()->getSceneManager("DefaultSceneManager");
    Ogre::Camera* _camera = _sceneMgr->getCamera("DefaultCamera");
    m_debugDrawer = new CGLNativeDebugDrawer(_camera, _sceneMgr, m_dynamicsWorld);

    btBuildShapeUtility::buildGroundShape(m_dynamicsWorld, m_collisionShapes);
    btBuildShapeUtility::buildBoxShape(m_dynamicsWorld, m_collisionShapes);
}

void
CBulletPhysicManager::release()
{
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
