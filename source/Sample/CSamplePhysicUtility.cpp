#include "CSamplePhysicUtility.h"
#include "OgreMath.h"

btVector3
ConvertOgreVectorTobtVector(const Ogre::Vector3& v) { return btVector3(v.x, v.y, v.z);}

/* ----------------------------------------------------------------------- 
 | the function describe line segment :  
 | - rayFrom : camera position
 | - rayTo : a point from mouse point on near plane raycasting to far plane
 |
 | only for perspective, not ortho viewport 
 | current method reference from DemoApplication::getRayTo
   ----------------------------------------------------------------------- */
bool
makeRayCastingSegment(float mouse_x, float mouse_y, Ogre::Camera* cam, btVector3& rayFrom, btVector3& rayTo)
{
    float nearPlane = 1.0f; 
    // float nearPlane = cam->getNearClipDistance();
    float farPlane = cam->getFarClipDistance();
    // float fovy = cam->getFOVy().valueDegrees();    
    float top = 1.f;
	float bottom = -1.f;	
	float tanFov = (top-bottom) * 0.5f / nearPlane;
	float fov = btScalar(2.0) * btAtan(tanFov);
    float aspect = cam->getAspectRatio();
    float screenWidth = (float)(cam->getViewport()->getActualWidth());
    float screenHeight = (float)(cam->getViewport()->getActualHeight());

    if (Ogre::Math::isNaN(aspect))
    {
        OGRE_EXCEPT(Ogre::Exception::ERR_NOT_IMPLEMENTED, 
				"Calculate aspect is NAN.",
				"::makeRayCastingResult");
        return false;
    } // End if
    
	rayFrom = ConvertOgreVectorTobtVector(cam->getPosition());
    btVector3 rayForward = ConvertOgreVectorTobtVector(cam->getDirection().normalisedCopy());
	rayForward *= farPlane;

    btVector3 vertical = ConvertOgreVectorTobtVector(cam->getUp());

	btVector3 hor;
	hor = rayForward.cross(vertical);
	hor.normalize();
	vertical = hor.cross(rayForward);
	vertical.normalize();

	float tanfov = tanf(0.5f*fov);
	hor *= 2.0f * farPlane * tanfov;
	vertical *= 2.0f * farPlane * tanfov;
	
	hor *= aspect;

	btVector3 rayToCenter = rayFrom + rayForward;
	btVector3 dHor = hor * 1.0f/screenWidth;
	btVector3 dVert = vertical * 1.0f/screenHeight;

	rayTo = rayToCenter - 0.5f * hor + 0.5f * vertical;
	rayTo += btScalar(mouse_x) * dHor;
	rayTo -= btScalar(mouse_y) * dVert;

    return true;
} //End for makeRayCastingResult

