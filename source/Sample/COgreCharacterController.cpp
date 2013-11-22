#include "COgreCharacterController.h"
#include "CCharacterController.h"

#define NUM_ANIMS 13              // number of animations the character has
#define CHAR_HEIGHT 5.0f          // height of character's center of mass above ground
#define TURN_SPEED 500.0f         // character turning in degrees per second
#define RUN_SPEED 17              // character running speed in units per second

void 
COgreCharacterController::setupBody(Ogre::SceneManager* sceneMgr)
{
	// entity
	mBodyEnt = sceneMgr->createEntity("SinbadBody", "Sinbad.mesh");
	
    // node
    mBodyNode = sceneMgr->getRootSceneNode()->createChildSceneNode("SinbadBodyNode");
    mBodyNode->attachObject(mBodyEnt);
    mBodyNode->setPosition(Ogre::Vector3::UNIT_Y * CHAR_HEIGHT);

    mKeyDirection = Ogre::Vector3::ZERO;
	mVerticalVelocity = 0;
}

void 
COgreCharacterController::setupAnimations()
{
    mTopAnimID = mBaseAnimID = ANIM_NONE;

	// this is very important due to the nature of the exported animations
    mBodyEnt->getSkeleton()->setBlendMode(Ogre::ANIMBLEND_CUMULATIVE);

	Ogre::String animNames[] =
	{"IdleBase", "IdleTop", "RunBase", "RunTop", "HandsClosed", "HandsRelaxed", "DrawSwords",
	"SliceVertical", "SliceHorizontal", "Dance", "JumpStart", "JumpLoop", "JumpEnd"};

	// populate our animation list
	for (int i = 0; i < NUM_ANIMS; i++)
	{
		mAnims[i] = mBodyEnt->getAnimationState(animNames[i]);
		mAnims[i]->setLoop(true);
		//mFadingIn[i] = false;
		//mFadingOut[i] = false;
	}

	// start off in the idle state (top and bottom together)
	setBaseAnimation(ANIM_IDLE_BASE);
	setTopAnimation(ANIM_IDLE_TOP);

	// relax the hands since we're not holding anything
	//mAnims[ANIM_HANDS_RELAXED]->setEnabled(true);
}

void 
COgreCharacterController::updateBody(Ogre::Real deltaTime)
{
	// mGoalDirection = Vector3::ZERO;   // we will calculate this
    mDirection = Ogre::Vector3::ZERO;

    if (mKeyDirection != Ogre::Vector3::ZERO) // && mBaseAnimID != ANIM_DANCE)
	{
		// calculate actually goal direction in world based on player's key directions
        mDirection += mKeyDirection.z * mCamera->getOrientation().zAxis();
		mDirection += mKeyDirection.x * mCamera->getOrientation().xAxis();
		mDirection.y = 0;
		mDirection.normalise();

        Ogre::Quaternion toGoal = mBodyNode->getOrientation().zAxis().getRotationTo(mDirection);

		// calculate how much the character has to turn to face goal direction
        Ogre::Real yawToGoal = toGoal.getYaw().valueDegrees();
		// this is how much the character CAN turn this frame
		Ogre::Real yawAtSpeed = yawToGoal / Ogre::Math::Abs(yawToGoal) * deltaTime * TURN_SPEED;
		// reduce "turnability" if we're in midair
		// if (mBaseAnimID == ANIM_JUMP_LOOP) yawAtSpeed *= 0.2f;

		// turn as much as we can, but not more than we need to
        if (yawToGoal < 0) yawToGoal = std::min<Ogre::Real>(0, std::max<Ogre::Real>(yawToGoal, yawAtSpeed)); //yawToGoal = Math::Clamp<Real>(yawToGoal, yawAtSpeed, 0);
		else if (yawToGoal > 0) yawToGoal = std::max<Ogre::Real>(0, std::min<Ogre::Real>(yawToGoal, yawAtSpeed)); //yawToGoal = Math::Clamp<Real>(yawToGoal, 0, yawAtSpeed);
		
///////////////////////////////////////////////////////////////////////////////////////
		// move in current body direction (not the goal direction) instead of rigidbody
        // mBodyNode->translate(0, 0, deltaTime * RUN_SPEED * mAnims[mBaseAnimID]->getWeight(), Ogre::Node::TS_LOCAL);
	
        if (m_character)
        {                         
            Ogre::Vector3 dir(0, 0, RUN_SPEED);
            //Ogre::Vector3 dir();
            //dir = Ogre::Quaternion(Ogre::Radian(Ogre::Degree(yawToGoal)), Ogre::Vector3::UNIT_Y) * dir;
            
            //m_character->setLinearVelocity(btVector3(dir.x, dir.y, dir.z), Ogre::Degree(yawToGoal).valueRadians(), deltaTime);
            m_character->setLinearVelocity(btVector3(mDirection.x * RUN_SPEED, mDirection.y * RUN_SPEED, mDirection.z * RUN_SPEED), 
                Ogre::Degree(yawToGoal).valueRadians(), deltaTime);
            
            mBodyNode->yaw(Ogre::Degree(yawToGoal));
            /*
            const btTransform& transform = m_character->getWorldTransform();
            const btVector3& origin = transform.getOrigin();
            const btQuaternion& rot = transform.getRotation();

            mBodyNode->setPosition(origin.getX(), origin.getY(), origin.getZ());
            mBodyNode->setOrientation(rot.getW(), rot.getX(), rot.getY(), rot.getZ());
            */
        }
        else
        {
            mBodyNode->yaw(Ogre::Degree(yawToGoal));
            mBodyNode->translate(0, 0, deltaTime * RUN_SPEED, Ogre::Node::TS_LOCAL);
        } // End if
///////////////////////////////////////////////////////////////////////////////////////
    }

    if (m_character) //update from Last frame
    {
        const btTransform& transform = m_character->getWorldTransform();
        btScalar angle = transform.getRotation().getAngle();
        const btVector3& origin = transform.getOrigin();

        mBodyNode->setPosition(origin.getX(), origin.getY(), origin.getZ()); 

        m_character->jumping(btVector3(mDirection.x, mDirection.y, mDirection.z), deltaTime);
    }
}

void 
COgreCharacterController::updateAnimation(Ogre::Real deltaTime)
{
    Ogre::Real baseAnimSpeed = 1.0f;
    Ogre::Real topAnimSpeed = 1.0f;

	// increment the current base and top animation times
	if (mBaseAnimID != ANIM_NONE) mAnims[mBaseAnimID]->addTime(deltaTime * baseAnimSpeed);
	if (mTopAnimID != ANIM_NONE) mAnims[mTopAnimID]->addTime(deltaTime * topAnimSpeed);
}

void 
COgreCharacterController::addTime(Ogre::Real deltaTime)
{
    updateBody(deltaTime);
    updateAnimation(deltaTime);
}

void
COgreCharacterController::setBaseAnimation(AnimID id, bool reset)
{	
	if (mBaseAnimID != ANIM_NONE)
	{
		mAnims[ mBaseAnimID ]->setEnabled(false);
		mAnims[ mBaseAnimID ]->setWeight(0);
		mAnims[ mBaseAnimID ]->setTimePosition(0);
	}

    mBaseAnimID = id;

    if (id != ANIM_NONE)
	{
		// if we have a new animation, enable it and fade it in
		mAnims[id]->setEnabled(true);
		mAnims[id]->setWeight(1);
		if (reset) mAnims[id]->setTimePosition(0);
	}
}

void 
COgreCharacterController::setTopAnimation(AnimID id, bool reset)
{
    if (mTopAnimID != ANIM_NONE)
	{
		mAnims[ mTopAnimID ]->setEnabled(false);
		mAnims[ mTopAnimID ]->setWeight(0);
		mAnims[ mTopAnimID ]->setTimePosition(0);
	}

	mTopAnimID = id;

	if (id != ANIM_NONE)
	{
		// if we have a new animation, enable it and fade it in
		mAnims[id]->setEnabled(true);
		mAnims[id]->setWeight(1);
		if (reset) mAnims[id]->setTimePosition(0);
	}
}

void 
COgreCharacterController::injectKeyDown(const OIS::KeyCode& iKeyCode)
{
	// keep track of the player's intended direction
	if (iKeyCode == OIS::KC_T) mKeyDirection.z = -1;
	else if (iKeyCode == OIS::KC_F) mKeyDirection.x = -1;
	else if (iKeyCode == OIS::KC_G) mKeyDirection.z = 1;
	else if (iKeyCode == OIS::KC_H) mKeyDirection.x = 1;

	if (!mKeyDirection.isZeroLength() && mBaseAnimID == ANIM_IDLE_BASE)
	{
		// start running if not already moving and the player wants to move
		setBaseAnimation(ANIM_RUN_BASE, true);
		if (mTopAnimID == ANIM_IDLE_TOP) setTopAnimation(ANIM_RUN_TOP, true);
    }
}

void 
COgreCharacterController::injectKeyUp(const OIS::KeyCode& iKeyCode)
{
    if (iKeyCode == OIS::KC_T && mKeyDirection.z == -1) mKeyDirection.z = 0;
	else if (iKeyCode == OIS::KC_F && mKeyDirection.x == -1) mKeyDirection.x = 0;
	else if (iKeyCode == OIS::KC_G && mKeyDirection.z == 1) mKeyDirection.z = 0;
	else if (iKeyCode == OIS::KC_H && mKeyDirection.x == 1) mKeyDirection.x = 0;

    if (mKeyDirection.isZeroLength() && mBaseAnimID == ANIM_RUN_BASE)
	{
		// stop running if already moving and the player doesn't want to move
		setBaseAnimation(ANIM_IDLE_BASE);
		if (mTopAnimID == ANIM_RUN_TOP) setTopAnimation(ANIM_IDLE_TOP);
	}

    if (iKeyCode == OIS::KC_SPACE)
    {
        if (m_character)
        {
            m_character->jump();
        }
    }
}

void 
COgreCharacterController::createCharacterController(btDynamicsWorld* dynamicsWorld)
{
    const Ogre::Vector3& pos = mBodyNode->getPosition();
    const Ogre::Quaternion& rot = mBodyNode->getOrientation();

    btTransform startTransform;
    startTransform.setOrigin(btVector3(pos.x, pos.y, pos.z));
    startTransform.setRotation(btQuaternion(rot.x, rot.y, rot.z, rot.w));

    if (!m_character)
        m_character = new CCharacterController(dynamicsWorld, startTransform);
}