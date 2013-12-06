#pragma once

#include "BspConverter.h"

class btDynamicsWorld;
class btConvexShape;
namespace btUtility
{
bool buildBsp(const char* bspfilename, btDynamicsWorld* dyn);
bool buildOgreEntity(btAlignedObjectArray<btVector3>& vertices);
bool buildBspOgreEntity(btConvexShape* shape);

class BspToBulletConverter : public BspConverter
{
public:
    BspToBulletConverter(btDynamicsWorld* dyn) :
        m_dynamicsWorld(dyn) {}
    virtual ~BspToBulletConverter() {}

    virtual void	addConvexVerticesCollider(btAlignedObjectArray<btVector3>& vertices, bool isEntity, const btVector3& entityTargetLocation);

protected:
    btDynamicsWorld* m_dynamicsWorld;
};

}; /// namespace