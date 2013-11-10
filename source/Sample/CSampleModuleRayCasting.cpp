#include "CSampleModuleRayCasting.h"

#include "btBulletDynamicsCommon.h"
//#include "BulletDynamics/Dynamics/btDynamicsWorld.h"

#include "BulletDynamics/ConstraintSolver/btPoint2PointConstraint.h"//picking
#include "BulletDynamics/ConstraintSolver/btGeneric6DofConstraint.h"//picking

//#include "BulletCollision/CollisionShapes/btCollisionShape.h"
//#include "BulletCollision/CollisionShapes/btBoxShape.h"
//#include "BulletCollision/CollisionShapes/btSphereShape.h"
//#include "BulletCollision/CollisionShapes/btCompoundShape.h"
//#include "BulletCollision/CollisionShapes/btUniformScalingShape.h"
//#include "BulletDynamics/ConstraintSolver/btConstraintSolver.h"
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

    void addGroundShape()    
    {
		//create a few basic rigid bodies
	    btBoxShape* groundShape = new btBoxShape(btVector3(btScalar(50.),btScalar(50.),btScalar(50.)));
        	
	    m_collisionShapes.push_back(groundShape);

	    btTransform groundTransform;
	    groundTransform.setIdentity();
	    groundTransform.setOrigin(btVector3(0,-50,0));

		btScalar mass(0.);

		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.f);

		btVector3 localInertia(0,0,0);
		if (isDynamic)
			groundShape->calculateLocalInertia(mass,localInertia);

		//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,groundShape,localInertia);
		btRigidBody* body = new btRigidBody(rbInfo);

		//add the body to the dynamics world
		m_dynamicsWorld->addRigidBody(body);	
    }

    void simulate(double dt)
    {
    
	    if (m_dynamicsWorld)
	    {
		    m_dynamicsWorld->stepSimulation(dt);
		    
            //optional
		    m_dynamicsWorld->debugDrawWorld();
	    }
    } //

protected:
    void init()
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
    }

    void release()
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
        /*
	    for (int j=0;j<m_collisionShapes.size();j++)
	    {
		    btCollisionShape* shape = m_collisionShapes[j];
		    delete shape;
	    }
	    m_collisionShapes.clear();
        */

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

    btAlignedObjectArray<btCollisionShape*>	m_collisionShapes;
    btTriangleIndexVertexArray* m_indexVertexArrays;
    btBroadphaseInterface*	m_broadphase;
    btCollisionDispatcher*	m_dispatcher;
    btConstraintSolver*	m_solver;
    btDefaultCollisionConfiguration* m_collisionConfiguration;
        
	btDynamicsWorld*		m_dynamicsWorld;
    // for mouse picking
	btTypedConstraint*		m_pickConstraint;
}; // End of CBulletPhysicManager

CSampleModuleRayCasting::CSampleModuleRayCasting() :
    m_pBulletPhysicManager(NULL)
{
}

void 
CSampleModuleRayCasting::createScene()
{
    CSimpleModuleTemplate::createScene();

    LOG("CSampleModuleRayCasting::createScene");   
    
    m_pBulletPhysicManager = new CBulletPhysicManager();

}

void 
CSampleModuleRayCasting::destroyScene()
{
    CSimpleModuleTemplate::destroyScene();

    LOG("CSampleModuleRayCasting::destroyScene");

    if (m_pBulletPhysicManager)
    {
        delete m_pBulletPhysicManager;
        m_pBulletPhysicManager = NULL;
    }
}

void 
CSampleModuleRayCasting::updateScene(float fDeltaTime)
{
    CSimpleModuleTemplate::updateScene(fDeltaTime);
    
    m_pBulletPhysicManager->simulate(fDeltaTime);
}