#pragma once

#pragma warning(disable: 4010)
#pragma warning(disable: 4251)
#pragma warning(disable: 4996)
#pragma warning(disable: 4193)
#pragma warning(disable: 4275)

// ogre
#include <Ogre.h>

// bullet
#include "btBulletDynamicsCommon.h"

/*
 | utiltiy for convert Ogre::Vector3 to btVector3
*/
btVector3
ConvertOgreVectorTobtVector(const Ogre::Vector3& v);

/*
 | utiltiy for rayCasting binding Ogre3d and bullet
*/
bool
makeRayCastingSegment(float mouse_x, float mouse_y, Ogre::Camera* cam, btVector3& rayFrom, btVector3& rayTo);

/*
 | utiltiy for build triangle shape from ogre mesh 
 */
bool
buildRigidBodyFromOgreEntity(Ogre::Entity* ent,
                             btDynamicsWorld* dynamicsWorld, 
                             btAlignedObjectArray<btCollisionShape*>& collisionShapes,
                             void* &data);

/*
 | utiltiy for build bullet height field terrain from gray scale image
 */
bool
buildHeightFieldTerrainFromImage(const Ogre::String& filename, 
                                 btDynamicsWorld* dynamicsWorld, 
                                 btAlignedObjectArray<btCollisionShape*>& collisionShapes,
                                 void* &data);
                             