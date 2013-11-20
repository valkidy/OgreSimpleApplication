#include "CCharacterController.h"

#include "BulletDynamics/Dynamics/btDynamicsWorld.h"
#include "BulletCollision/CollisionShapes/btCapsuleShape.h"

// for character controller
#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "BulletDynamics/Character/btKinematicCharacterController.h"

CCharacterController::CCharacterController(btDynamicsWorld* dynamicsWorld, const btTransform& transform)
{   
    btAssert(dynamicsWorld);

	m_ghostObject = new btPairCachingGhostObject();
	m_ghostObject->setWorldTransform(transform);
    
	btScalar characterHeight = 4;
	btScalar characterWidth  = 2;
	btConvexShape* capsule = new btCapsuleShape(characterWidth,characterHeight);
	m_ghostObject->setCollisionShape(capsule);
	m_ghostObject->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);
    m_ghostObject->setUserPointer((void*)dynamicsWorld);

    m_ghostPairCallback = new btGhostPairCallback();
    dynamicsWorld->getBroadphase()->getOverlappingPairCache()->setInternalGhostPairCallback(m_ghostPairCallback);

	btScalar stepHeight = btScalar(0.35);
	m_character = new btKinematicCharacterController(m_ghostObject,capsule,stepHeight);

    //only collide with static for now (no interaction with dynamic objects)
	dynamicsWorld->addCollisionObject(m_ghostObject,btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::StaticFilter|btBroadphaseProxy::DefaultFilter); 
	dynamicsWorld->addAction(m_character);
}

CCharacterController::~CCharacterController()
{
    if (m_ghostObject)
    {
        btCollisionShape* shape = m_ghostObject->getCollisionShape();
        btAssert(shape);
        delete shape;
        
        btDynamicsWorld* dynamicsWorld = static_cast<btDynamicsWorld*>(m_ghostObject->getUserPointer());
        if (dynamicsWorld)
        {
            // remove callback and ghost object            
            dynamicsWorld->removeCollisionObject(m_ghostObject);
            dynamicsWorld->getBroadphase()->getOverlappingPairCache()->setInternalGhostPairCallback(NULL);
        } 
            
        delete m_ghostObject;
        m_ghostObject = NULL;
    }    
    
    if (m_ghostPairCallback)
    {        
        delete m_ghostPairCallback;
        m_ghostPairCallback = NULL;
    }

    if (m_character)
    {
        delete m_character;
        m_character = NULL;
    }
}

void 
CCharacterController::setLinearVelocity(const btVector3& velocity)
{
    btScalar walkSpeed = 0.05f;    
    m_character->setWalkDirection(velocity*walkSpeed);
}

void
CCharacterController::jump()
{
    m_character->setJumpSpeed(30.0f);
    m_character->jump();
}