#include "CCharacterController.h"

#include "BulletDynamics/Dynamics/btDynamicsWorld.h"
#include "BulletCollision/CollisionShapes/btCapsuleShape.h"

// for character controller
#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "BulletDynamics/Character/btKinematicCharacterController.h"

CCharacterController::CCharacterController(btDynamicsWorld* dynamicsWorld, const btTransform& transform,
                                           btScalar characterHeight, btScalar characterWidth)
{   
    btAssert(dynamicsWorld);

	m_ghostObject = new btPairCachingGhostObject();
	m_ghostObject->setWorldTransform(transform);
    
	//btScalar characterHeight = 4;
	//btScalar characterWidth  = 2;
	btConvexShape* capsule = new btCapsuleShape(characterWidth,characterHeight);
	m_ghostObject->setCollisionShape(capsule);
	m_ghostObject->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);
    m_ghostObject->setUserPointer((void*)dynamicsWorld);

    m_ghostPairCallback = new btGhostPairCallback();
    dynamicsWorld->getBroadphase()->getOverlappingPairCache()->setInternalGhostPairCallback(m_ghostPairCallback);

	btScalar stepHeight = btScalar(0.35);
	m_character = new btKinematicCharacterController(m_ghostObject,capsule,stepHeight);

    //only collide with static for now (no interaction with dynamic objects)
	dynamicsWorld->addCollisionObject(m_ghostObject, btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::StaticFilter|btBroadphaseProxy::DefaultFilter); 
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

            dynamicsWorld->removeAction(m_character);
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
CCharacterController::setLinearVelocity(const btVector3& velocity, btScalar angle, btScalar timeInterval)
{        
    //m_character->setWalkDirection(velocity);
    m_character->setVelocityForTimeInterval(velocity, timeInterval);
}

void
CCharacterController::jump()
{    
    m_character->setJumpSpeed(10.0f);
    m_character->setGravity(10.0f);
    //m_character->setFallSpeed(55.0f * 5.0f);
    m_character->setJumpSpeed(50.0f);
    m_character->setGravity(100.0f);
    m_character->setFallSpeed(55.0f);

    m_character->jump();    
}

void
CCharacterController::jumping(const btVector3& direction, btScalar timeInterval)
{
    if (!m_character->onGround())
         m_character->setVelocityForTimeInterval(direction, timeInterval);
}

bool
CCharacterController::onGround()
{
    return m_character->onGround();
}

const btTransform& 
CCharacterController::getWorldTransform()
{
    return m_ghostObject->getWorldTransform();
}