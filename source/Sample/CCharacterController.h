#ifndef __CCHARACTER_CONTROLLER_H__
#define __CCHARACTER_CONTROLLER_H__

#include "LinearMath/btTransform.h"

/* 
 | reference from CharacterDemo bullet source rev 2715
 |
 | // class DynamicCharacterController : public btCharacterControllerInterface
 | @ Since the dynamic character controller(above) is not fully supported, instead of
 |   implement kinetic character controller using btKinematicCharacterController
 |
 | @ Useful wiki reference:
 |   -(motion state) http://www.bulletphysics.org/mediawiki-1.5.8/index.php/MotionStates
 |   -(collision contact) http://www.bulletphysics.org/mediawiki-1.5.8/index.php/Collision_Callbacks_and_Triggers
 */
class btDynamicsWorld;
class btKinematicCharacterController;
class btPairCachingGhostObject;
class btGhostPairCallback;
class CCharacterController
{
public:
    CCharacterController(btDynamicsWorld* dynamicsWorld, const btTransform& transform);
    virtual ~CCharacterController();

    void setLinearVelocity(const btVector3& velocity, btScalar angle, btScalar timeInterval);    
    void jump();
    void jumping(const btVector3& direction, btScalar timeInterval);    

    const btTransform& getWorldTransform();

protected:
    btGhostPairCallback* m_ghostPairCallback;
    btKinematicCharacterController* m_character;
    btPairCachingGhostObject* m_ghostObject;
};


#endif