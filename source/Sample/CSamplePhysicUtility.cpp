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

/* ----------------------------------------------------------------------- 
 | the function describe build btRigidBody from Ogre mesh :   
 |
 | @prama in : Ogre entity (use as a single object)
 | @pamra out : return true if build success and add collision shape to dynamicsworld
   ----------------------------------------------------------------------- */
void
getVertexBuffer(Ogre::Entity* ent, void* &vertices, size_t& vertexCount, void* &indices, size_t& indexCount);
bool
buildRigidBodyFromOgreEntity(Ogre::Entity* ent, btDynamicsWorld* dynamicsWorld, 
                             btAlignedObjectArray<btCollisionShape*>& collisionShapes,
                             btAlignedObjectArray<btTriangleIndexVertexArray*>& triangleMeshes)
{
    void *vertices, *indices;
    size_t vertexCount = 0, indexCount = 0;
    
    getVertexBuffer(ent, vertices, vertexCount, indices, indexCount);
       
    btScalar mass(1.0f);	
    btVector3 localInertia(0,0,0);
    //btCollisionShape* colShape = new btCapsuleShape();    

    btTriangleMesh* trimesh = new btTriangleMesh();
	// btVector3 localScaling(6.f,6.f,6.f);
			
    Ogre::Vector3* vert = (Ogre::Vector3*)vertices;
    Ogre::ulong* index = (Ogre::ulong*)indices;

	for (int i=0 ; i<vertexCount ; i++)
	{
		int index0 = index[i*3];
		int index1 = index[i*3+1];
		int index2 = index[i*3+2];

        btVector3 vertex0(vert[index0].x, vert[index0].y, vert[index0].z);
        btVector3 vertex1(vert[index1].x, vert[index1].y, vert[index1].z);
        btVector3 vertex2(vert[index2].x, vert[index2].y, vert[index2].z);

		trimesh->addTriangle(vertex0,vertex1,vertex2);
	}

    delete [] vertices;
    delete [] indices;
    triangleMeshes.push_back(trimesh);

    btCollisionShape* colShape = new btConvexTriangleMeshShape(trimesh);
    const btVector3& scale = colShape->getLocalScaling();
    colShape->calculateLocalInertia(mass,localInertia);
    collisionShapes.push_back(colShape);
    
    btTransform trans;
    trans.setIdentity();
    btDefaultMotionState* motionState = new btDefaultMotionState(trans);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,motionState,colShape,localInertia);
    btRigidBody* body = new btRigidBody(rbInfo);
    dynamicsWorld->addRigidBody(body);
    
    // link RigidBody and SceneNode
    Ogre::SceneNode* node = ent->getParentSceneNode();
    body->setUserPointer((void*)node);
    
    return true;
}

/* ----------------------------------------------------------------------- 
 | the function describe retrieve vertexbuffer from Ogre entity :   
 | (Ref:http://www.ogre3d.org/tikiwiki/tiki-index.php?page=RetrieveVertexData)
 |
 | @prama in : Ogre entity
 | @pamra out : vertex buffer, size of vertexbuffer, vertex indices, sizeof vertex indices
   ----------------------------------------------------------------------- */
void
getVertexBuffer(Ogre::Entity* ent, void* &vertices, size_t& vertexCount, void* &indices, size_t& indexCount)
//getVertexBuffer(Ogre::Entity* ent, (void*)&vertices, size_t& vertexCount, (void*)&indices, size_t& indexCount)
{
    bool added_shared = false;
    size_t current_offset = 0;
    size_t shared_offset = 0;
    size_t next_offset = 0;
    size_t index_offset = 0;
 
    vertexCount = indexCount = 0;
 
    const Ogre::MeshPtr& mesh = ent->getMesh();

    // Calculate vertexCount, indexCount
    for ( Ogre::ushort i = 0; i < mesh->getNumSubMeshes(); ++i)
    {
        Ogre::SubMesh* subMesh = mesh->getSubMesh(i);        
        if (subMesh->useSharedVertices)
        {
            if( !added_shared )
            {
                vertexCount += mesh->sharedVertexData->vertexCount;
                added_shared = true;
            }

            vertexCount += mesh->sharedVertexData->vertexCount;
        }
        else
            vertexCount += subMesh->vertexData->vertexCount;
        
        indexCount += subMesh->indexData->indexCount;
    }
    
    // Alloc vertices, indices
    vertices = new Ogre::Vector3[ vertexCount ];
    indices = new Ogre::ulong[ indexCount ];
  
    added_shared = false;

    // Assign vertices data, indices data
    for (Ogre::ushort i = 0; i < mesh->getNumSubMeshes(); ++i)
    {
        Ogre::SubMesh* subMesh = mesh->getSubMesh(i);
        Ogre::VertexData* vertexData = subMesh->useSharedVertices ? mesh->sharedVertexData : subMesh->vertexData;

        if ((!subMesh->useSharedVertices) || (subMesh->useSharedVertices && !added_shared))
        {
        if(subMesh->useSharedVertices)
        {
            added_shared = true;
            shared_offset = current_offset;
        }

        // Retrieve vertices        
        const Ogre::VertexElement* posElem = vertexData->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);
        Ogre::HardwareVertexBufferSharedPtr vbuf = vertexData->vertexBufferBinding->getBuffer(posElem->getSource());
        
        Ogre::uchar* vertex = static_cast<Ogre::uchar*>(vbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
        Ogre::Real* data;
        
        for (size_t j = 0 ; j < vertexData->vertexCount ; ++j, vertex += vbuf->getVertexSize())
        {
            posElem->baseVertexPointerToElement(vertex, &data);
            Ogre::Vector3 origin(data[0], data[1], data[2]);
            // ((Ogre::Vector3*)vertices)[ current_offset + j ] = (orient * (origin * scale)) + position;
            ((Ogre::Vector3*)vertices)[ current_offset + j ] = origin;
        } // End for
        
        vbuf->unlock();
        next_offset += vertexData->vertexCount;
        } // End if

        // Retrieve indices
        Ogre::IndexData* indexData = subMesh->indexData;
        size_t trisCount = indexData->indexCount / 3;

        Ogre::HardwareIndexBufferSharedPtr ibuf = indexData->indexBuffer;
        bool use32bitindexes = (ibuf->getType() == Ogre::HardwareIndexBuffer::IT_32BIT);

        void* index = (ibuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));       
        Ogre::ulong offset = (subMesh->useSharedVertices) ? shared_offset : current_offset; 
        for (Ogre::ulong k = 0; k < trisCount * 3; ++k)
        {
            if ( use32bitindexes )
                ((Ogre::ulong*)indices)[ index_offset++ ] = ((Ogre::ulong*)index)[ k + offset ];
            else
                ((Ogre::ulong*)indices)[ index_offset++ ] = ((Ogre::ushort*)index)[ k + offset ];
        }

        ibuf->unlock();
        current_offset = next_offset;        
    } // End for
}