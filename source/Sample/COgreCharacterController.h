#ifndef __COGRECHARACTER_CONTROLLER_H__
#define __COGRECHARACTER_CONTROLLER_H__

#include "COgreHeader.h"

/* 
 | Reference from SinbadCharacterController
 | a Entity involved with :
 | - transform
 | - animation(fsm)
 | - mesh
 | - controller(keyboard/mouse....etc)
 | - camera follower
 | 
 | a Entity with RigidBody involved with :
 | - transform
 | - collision
 */
class btDynamicsWorld;
class CCharacterController;
class COgreCharacterController
{
protected:
    enum AnimID
	{
		ANIM_IDLE_BASE,
		ANIM_IDLE_TOP,
		ANIM_RUN_BASE,
		ANIM_RUN_TOP,
		ANIM_HANDS_CLOSED,
		ANIM_HANDS_RELAXED,
		ANIM_DRAW_SWORDS,
		ANIM_SLICE_VERTICAL,
		ANIM_SLICE_HORIZONTAL,
		ANIM_DANCE,
		ANIM_JUMP_START,
		ANIM_JUMP_LOOP,
		ANIM_JUMP_END,
		ANIM_NONE
	};

public:
    COgreCharacterController(Ogre::Camera* cam) : 
        mCamera(cam),
        m_character(NULL) { setupBody(cam->getSceneManager()); setupAnimations(); }
    virtual ~COgreCharacterController() { destroyCharacterController(); }
    
    void addTime(Ogre::Real deltaTime);
    void injectKeyDown(const OIS::KeyCode& iKeyCode);
    void injectKeyUp(const OIS::KeyCode& iKeyCode);

    // character controller
    void createCharacterController(btDynamicsWorld* dynamicsWorld);

protected:
    void destroyCharacterController();
    void setupBody(Ogre::SceneManager* sceneMgr);
    void setupAnimations();
    void updateBody(Ogre::Real deltaTime);
    void updateAnimation(Ogre::Real deltaTime);
    void setBaseAnimation(AnimID id, bool reset = false);
    void setTopAnimation(AnimID id, bool reset = false);

private:
    Ogre::Camera* mCamera;
    Ogre::SceneNode* mBodyNode;
    Ogre::Entity* mBodyEnt;
    Ogre::AnimationState* mAnims[13];

    Ogre::Vector3 mKeyDirection;
    Ogre::Vector3 mDirection;
    Ogre::Vector3 mVerticalVelocity;

    AnimID mBaseAnimID;                   // current base (full- or lower-body) animation
	AnimID mTopAnimID;                    // current top (upper-body) animation

    // physic
    CCharacterController* m_character;
};


#endif // __COGRECHARACTER_CONTROLLER_H__