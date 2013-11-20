#include "Win32Utility.h"

#include "OgreMath.h"
#include "BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h"

#include "CSamplePhysicUtility.h"

namespace btUtility
{


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

//////////////////////////////////////////////////////////////////////////////////////////////////
/* ----------------------------------------------------------------------- 
 | the function describe retrieve vertexbuffer from Ogre entity :   
 | (Ref:http://www.ogre3d.org/tikiwiki/tiki-index.php?page=RetrieveVertexData)
 |
 | @prama in : Ogre entity
 | @pamra out : vertex buffer, size of vertexbuffer, vertex indices, sizeof vertex indices
   ----------------------------------------------------------------------- */
void
getVertexBuffer(Ogre::Entity* ent, void* &vertices, size_t& vertexCount, void* &indices, size_t& indexCount)
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

/* ----------------------------------------------------------------------- 
 | the function describe build btRigidBody from Ogre mesh :   
 |
 | @prama in : Ogre entity (use as a single object)
 | @pamra out : return true if build success and add collision shape to dynamicsworld
   ----------------------------------------------------------------------- */
bool
buildRigidBodyFromOgreEntity(Ogre::Entity* ent,
                             btDynamicsWorld* dynamicsWorld, 
                             btAlignedObjectArray<btCollisionShape*>& collisionShapes,
                             void* &data)
{
    void *vertices, *indices;
    size_t vertexCount = 0, indexCount = 0;
    
    getVertexBuffer(ent, vertices, vertexCount, indices, indexCount);
       
    btScalar mass(1.0f);	
    btVector3 localInertia(0,0,0);
    
    data = new btTriangleMesh();
    btTriangleMesh* trimesh = static_cast<btTriangleMesh*>(data);
    btAssert(trimesh);

    Ogre::Vector3* vert = (Ogre::Vector3*)vertices;
    Ogre::ulong* index = (Ogre::ulong*)indices;

	for (size_t i=0 ; i<vertexCount ; i++)
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

////////////////////////////////////////////////////////////////////////////////////////////
/* ----------------------------------------------------------------------- 
 | utility class for get height field vertices data from bullet to construct ogre mesh
 | 
 | @prama in : manual object, material name
   ----------------------------------------------------------------------- */
class btHeightFieldProcessor : public btTriangleCallback
{
public:
    btHeightFieldProcessor(Ogre::ManualObject* _obj, const Ogre::String& strMaterialName) :
        m_obj(_obj) ,m_triangleId(-1)
        { 
            btAssert(m_obj); 
            m_obj->begin(strMaterialName, Ogre::RenderOperation::OT_TRIANGLE_LIST); 
        }
    ~btHeightFieldProcessor() { m_obj->end();}

	virtual void processTriangle(btVector3* triangle, int partId, int triangleIndex)
	{   
        int triangleId = triangleIndex + partId;        
        m_uv = (triangleId != m_triangleId) ? 0 : 1.0f;
                        
        // first triangle : (0,0) (0,1) (1,0)
        // second triangle: (1,0) (0,1) (1,1)
        m_obj->position(triangle[0].getX(), triangle[0].getY(), triangle[0].getZ());
        m_obj->textureCoord(m_uv, 0);
        m_obj->position(triangle[1].getX(), triangle[1].getY(), triangle[1].getZ());
        m_obj->textureCoord(0, 1);
        m_obj->position(triangle[2].getX(), triangle[2].getY(), triangle[2].getZ());
        m_obj->textureCoord(1, m_uv);

        m_triangleId = triangleId;
	}

protected:
    Ogre::ManualObject* m_obj;
    
    int m_triangleId;
    Ogre::Real m_uv;
};

/* ----------------------------------------------------------------------- 
 | build bullet height field shape and generate ogre mesh from grayscale image
 | 
 | @param in : 
 | @param out: raw data of height field terrain
 | ToDo: adjest grid scale, grid height, local scale, max/min height
   ----------------------------------------------------------------------- */
bool
buildHeightFieldTerrainFromImage(const Ogre::String& filename, 
                                 btDynamicsWorld* dynamicsWorld, 
                                 btAlignedObjectArray<btCollisionShape*>& collisionShapes,
                                 void* &data)
{
    Ogre::Image img;
    try
    {
        img.load(filename, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    }   
    catch(Ogre::Exception err)
    {
        LOG(err.what());
        return false;
    }
    
    size_t grid_w = 65, grid_h = 65; // must be (2^N) + 1
    size_t grid_max_w = 129, grid_max_h = 129; // must be (2^N) + 1
    size_t img_w = img.getWidth();
    size_t img_h = img.getHeight();
    
    // validate image size is (2^N) + 1
    if ((img_w-1) & (img_w-2)) img_w = grid_w; 
    if ((img_h-1) & (img_h-2)) img_h = grid_h;
    //if (img_w > grid_max_w) img_w = grid_max_w; 
    //if (img_h > grid_max_h) img_h = grid_max_h; 

    LOG("LoadImage name=%s, width=%d, height=%d, width^2+1=%d, height^2+1=%d",
        filename.c_str(), img.getWidth(), img.getHeight(), img_w, img_h);
    img.resize(img_w, img_h);

    size_t pixelSize = Ogre::PixelUtil::getNumElemBytes(img.getFormat());    
    size_t bufSize = img.getSize() / pixelSize;
    data = new Ogre::Real[ bufSize ];
    Ogre::Real* dest = static_cast<Ogre::Real*>(data);
    memset(dest, 0, bufSize);
        
    /*
     | @ Notice the alignment problem
     | - uchar to float alignment
     | - pixel format in bytes as rawdata type, also affects alignment
     */
    Ogre::uchar* src = img.getData();    
    for (size_t i=0;i<bufSize;++i)
    {        
        dest[i] = ((Ogre::Real)src[i * pixelSize] - 127.0f)/16.0f;
    }
          
    // parameter    
    int upAxis = 1;
    btScalar gridSpacing = 5.0f;
    btScalar gridHeightScale = 0.2f;
    btScalar minHeight = -10.0f;
    btScalar maxHeight = 10.0f;
    btScalar defaultContactProcessingThreshold = BT_LARGE_FLOAT;

	btHeightfieldTerrainShape *heightfieldShape =  
        new btHeightfieldTerrainShape(img_w, img_h,
                                      dest,
                                      gridHeightScale,
                                      minHeight, maxHeight,
                                      upAxis, PHY_FLOAT, false);
	btAssert(heightfieldShape && "null heightfield");

	// shape
	btVector3 localScaling(1.0f, 1.0f, 1.0f);
	heightfieldShape->setLocalScaling(localScaling);    
    collisionShapes.push_back(heightfieldShape);

    // rigidBody
    btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0,0,0)));
    btRigidBody::btRigidBodyConstructionInfo cInfo(0, motionState, heightfieldShape, btVector3(0,0,0));
    btRigidBody* rigidBody = new btRigidBody(cInfo);        	
	rigidBody->setContactProcessingThreshold(defaultContactProcessingThreshold);
    int flags = rigidBody->getCollisionFlags();
    rigidBody->setCollisionFlags(flags | btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT);
    
    dynamicsWorld->addRigidBody(rigidBody);
    
    // add ogre height field mesh
    Ogre::SceneManager* sceneMgr = Ogre::Root::getSingletonPtr()->getSceneManager("DefaultSceneManager");
    btAssert(sceneMgr);
    
    Ogre::ManualObject* obj = sceneMgr->createManualObject("btHeightFieldEntity");

    btVector3 aabbMin, aabbMax;
    heightfieldShape->getAabb(btTransform(btQuaternion(0,0,0,1),btVector3(0,0,0)), aabbMin, aabbMax);

    btHeightFieldProcessor callback(obj, "DefaultPlane");
    heightfieldShape->processAllTriangles(&callback, aabbMin, aabbMax);
   
    sceneMgr->getRootSceneNode()->attachObject(obj);

    return true;
}

bool
buildGroundShape(Ogre::SceneManager* sceneMgr, btDynamicsWorld* dynamicsWorld, btAlignedObjectArray<btCollisionShape*>& collisionShapes)
{
    btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0,1,0), 1);
    btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0,0,0)));
    btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0,groundMotionState,groundShape,btVector3(0,0,0));
    btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);
    groundRigidBody->setCcdMotionThreshold(1.0f);
    groundRigidBody->setRestitution(0.0f);
    groundRigidBody->setFriction(1.0f);

    dynamicsWorld->addRigidBody(groundRigidBody);

    collisionShapes.push_back(groundShape);

    // build plane if scene manager exist
    if (sceneMgr)
    {
        Ogre::Plane* plane = new Ogre::MovablePlane("Plane");
	    plane->d = 0;
	    plane->normal = Ogre::Vector3::UNIT_Y;
	
	    Ogre::MeshManager::getSingleton().createPlane("PlaneMesh", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, *plane,
		    256, 256, 1, 1, true, 1, 1, 1, Ogre::Vector3::UNIT_Z);

        Ogre::Entity* ent = sceneMgr->createEntity("PlaneEntity", "PlaneMesh");
        assert(ent);
        ent->setMaterialName("DefaultPlane");
	    
        Ogre::SceneNode* node = sceneMgr->getRootSceneNode()->createChildSceneNode("PlaneNode");
	    node->attachObject(ent);
	    node->setPosition(Ogre::Vector3::ZERO);
    } // End if

    return true;
}

/* ----------------------------------------------------------------------- 
 | build bullet box shape
 | 
 | : default create 125 (5x5x5) dynamic object
   ----------------------------------------------------------------------- */
bool
buildBoxShapeArray(Ogre::SceneManager* sceneMgr, btDynamicsWorld* dynamicsWorld, btAlignedObjectArray<btCollisionShape*>& collisionShapes,
                   const btVector3& array_size, btScalar scale)
{
    btTransform startTransform;
    startTransform.setIdentity();

    btScalar mass(1.f);	
    btVector3 localInertia(0,0,0);	

    btBoxShape* colShape = new btBoxShape(btVector3(scale, scale, scale));
    btAssert(colShape);
    colShape->calculateLocalInertia(mass,localInertia);
    collisionShapes.push_back(colShape);

    float start_x = - array_size.getX()/2;
    float start_y = array_size.getY();
    float start_z = - array_size.getZ()/2;

    for (int k=0;k<array_size.getY();k++)
    {
	    for (int i=0;i<array_size.getX();i++)
	    {
            for(int j = 0;j<array_size.getZ();j++)
		    {
			    startTransform.setOrigin(scale * btVector3(
								    btScalar(2.0*i + start_x),
								    btScalar(20+2.0*k + start_y),
								    btScalar(2.0*j + start_z)));
                
			    //using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
			    btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
			    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,colShape,localInertia);
			    btRigidBody* body = new btRigidBody(rbInfo);
				
			    dynamicsWorld->addRigidBody(body);
		    }
	    }		
    }

    return true;
}

}; // namespace
