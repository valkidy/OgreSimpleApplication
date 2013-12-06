#include "CSampleBsp.h"

#include "COgreHeader.h"

#include "btBulletDynamicsCommon.h"
#include "BulletCollision/CollisionShapes/btShapeHull.h"

// Bsp
#include "BspLoader.h"

namespace btUtility
{
bool buildBsp(const char* bspfilename, btDynamicsWorld* dyn)
{
    void* memoryBuffer = 0;
	
	FILE* file = NULL; //fopen(bspfilename,"r");
	if (!file)
	{
		//try again other path, 
		//sight... visual studio leaves the current working directory in the projectfiles folder
		//instead of executable folder. who wants this default behaviour?!?
        //bspfilename = "BspDemo.bsp";
        file = fopen(bspfilename,"r");
	}

    if (file)
	{
		BspLoader bspLoader;
		int size=0;
		if (fseek(file, 0, SEEK_END) || (size = ftell(file)) == EOF || fseek(file, 0, SEEK_SET)) {        /* File operations denied? ok, just close and return failure */
			printf("Error: cannot get filesize from %s\n", bspfilename);
		} else
		{
			//how to detect file size?
			memoryBuffer = malloc(size+1);
			fread(memoryBuffer,1,size,file);
			bspLoader.loadBSPFile( memoryBuffer);

			BspToBulletConverter bsp2bullet(dyn);
			float bspScaling = 0.1f;
			bsp2bullet.convertBsp(bspLoader,bspScaling);

		}
		fclose(file);
	}

    return true;
}

bool buildOgreEntity(btAlignedObjectArray<btVector3>& vertices)
{
    static int i = 0;

    //default scene manager
    Ogre::SceneManager* sceneMgr = Ogre::Root::getSingletonPtr()->getSceneManager("DefaultSceneManager");
    //assert(sceneMgr);
    Ogre::ManualObject* obj = sceneMgr->createManualObject("BspEntity" + Ogre::StringConverter::toString(++i));
    
    obj->begin("RustyBarrel", Ogre::RenderOperation::OT_TRIANGLE_LIST);
        
    int dsize = vertices.size();
    for (int j=0;j<dsize;j+=8)
    {    
            obj->position(vertices[j].getX(), vertices[j].getY(), vertices[j].getZ());
            obj->textureCoord(0, 0);
            obj->position(vertices[j+1].getX(), vertices[j+1].getY(), vertices[j+1].getZ());
            obj->textureCoord(0, 1);
            obj->position(vertices[j+2].getX(), vertices[j+2].getY(), vertices[j+2].getZ());
            obj->textureCoord(1, 1);
            obj->position(vertices[j+3].getX(), vertices[j+3].getY(), vertices[j+3].getZ());
            obj->textureCoord(1, 0);
            obj->position(vertices[j+4].getX(), vertices[j+4].getY(), vertices[j+4].getZ());
            obj->textureCoord(1, 0);
            obj->position(vertices[j+5].getX(), vertices[j+5].getY(), vertices[j+5].getZ());
            obj->textureCoord(1, 1);
            obj->position(vertices[j+6].getX(), vertices[j+6].getY(), vertices[j+6].getZ());
            obj->textureCoord(0, 1);
            obj->position(vertices[j+7].getX(), vertices[j+7].getY(), vertices[j+7].getZ());
            obj->textureCoord(0, 0);

            obj->index(0+j);
            obj->index(2+j);
            obj->index(3+j);
            obj->index(0+j);
            obj->index(1+j);
            obj->index(2+j);
            obj->index(1+j);
            obj->index(6+j);
            obj->index(2+j);
            obj->index(1+j);
            obj->index(5+j);
            obj->index(6+j);
            obj->index(4+j);
            obj->index(6+j);
            obj->index(5+j);
            obj->index(4+j);
            obj->index(7+j);
            obj->index(6+j);
            obj->index(0+j);
            obj->index(7+j);
            obj->index(4+j);
            obj->index(0+j);
            obj->index(3+j);
            obj->index(7+j);
            obj->index(0+j);
            obj->index(5+j);
            obj->index(1+j);
            obj->index(0+j);
            obj->index(4+j);
            obj->index(5+j);
            obj->index(2+j);
            obj->index(7+j);
            obj->index(3+j);
            obj->index(2+j);
            obj->index(6+j);
            obj->index(7+j);

            /*
            0,2,3,
            0,1,2,
            1,6,2,
            1,5,6,
            4,6,5,
            4,7,6,
            0,7,4,
            0,3,7,
            0,5,1,
            0,4,5,
            2,7,3,
            2,6,7
            */
        /*
            obj->position(vertices[j].getX(), vertices[j].getY(), vertices[j].getZ());
            obj->textureCoord(0, 0);
            obj->position(vertices[j+1].getX(), vertices[j+1].getY(), vertices[j+1].getZ());
            obj->textureCoord(0, 1);
            obj->position(vertices[j+2].getX(), vertices[j+2].getY(), vertices[j+2].getZ());
            obj->textureCoord(1, 0);
        */

    } // End for

    obj->end();
    
    sceneMgr->getRootSceneNode()->attachObject(obj);

    return true;
}

struct BspVertex
{
    float x, y, z;
    //float nx, ny, nz;
    float u, v;
};

bool buildBspOgreEntity(btConvexShape* shape)
{
    btAssert(shape);

    btShapeHull	shapehull(shape);
    shapehull.buildHull(shape->getMargin());
	const int			ni=shapehull.numIndices();
	const int			nv=shapehull.numVertices();
	const unsigned int*	pi=shapehull.getIndexPointer();
	const btVector3*	pv=shapehull.getVertexPointer();
    
    static int index = 0;
    Ogre::String meshName = "BspEntity" + Ogre::StringConverter::toString(++index);

    Ogre::MeshPtr mesh = Ogre::MeshManager::getSingleton().createManual(meshName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

	// create a submesh with the grass material
    Ogre::SubMesh* sm = mesh->createSubMesh();
	sm->setMaterialName("RustyBarrel");
	sm->useSharedVertices = false;
    sm->vertexData = OGRE_NEW Ogre::VertexData();
	sm->vertexData->vertexStart = 0;
	sm->vertexData->vertexCount = nv;
	sm->indexData->indexCount = ni;

	// specify a vertex format declaration for our mesh: 3 floats for position, 3 floats for normal, 2 floats for UV
	Ogre::VertexDeclaration* decl = sm->vertexData->vertexDeclaration;
    decl->addElement(0, 0, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
    //decl->addElement(0, sizeof(float) * 3, VET_FLOAT3, VES_NORMAL);
    decl->addElement(0, sizeof(float) * 3, Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES, 0);

	// create a vertex buffer
    Ogre::HardwareVertexBufferSharedPtr vb = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer
		(decl->getVertexSize(0), sm->vertexData->vertexCount, Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);

    BspVertex* verts = (BspVertex*)vb->lock(Ogre::HardwareBuffer::HBL_DISCARD);  // start filling in vertex data    
	for (unsigned int i = 0; i < nv; i++)  
	{
        BspVertex& vert = verts[i];

        vert.x = pv[i].getX();
        vert.y = pv[i].getY();
        vert.z = pv[i].getZ();
        
        vert.u = i < 2 ? 0 : 1;
		vert.v = i % 2;
	}
	vb->unlock();  

	sm->vertexData->vertexBufferBinding->setBinding(0, vb);  // bind vertex buffer to our submesh

	// create an index buffer
	sm->indexData->indexBuffer = Ogre::HardwareBufferManager::getSingleton().createIndexBuffer
		(Ogre::HardwareIndexBuffer::IT_16BIT, sm->indexData->indexCount, Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);

	// start filling in index data
	Ogre::uint16* indices = (Ogre::uint16*)sm->indexData->indexBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD);
	for (unsigned int i = 0; i < ni; i++) 
	{		
		*indices++ = pi[i];		
	}
	sm->indexData->indexBuffer->unlock();  // commit index changes

    // update mesh AABB
    Ogre::AxisAlignedBox aabb;
    aabb.setExtents(-100,-100,-100,100,100,100);
    mesh->_setBounds(aabb);

    //default scene manager
    Ogre::SceneManager* sceneMgr = Ogre::Root::getSingletonPtr()->getSceneManager("DefaultSceneManager");
    btAssert(sceneMgr);
    Ogre::Entity* ent = sceneMgr->createEntity("BspEntity" + Ogre::StringConverter::toString(index), meshName);
    btAssert(ent);
    Ogre::SceneNode* node = sceneMgr->getRootSceneNode()->createChildSceneNode("BspSceneNode" + Ogre::StringConverter::toString(index));
    node->attachObject(ent);
    
    return true;
}

void
BspToBulletConverter::addConvexVerticesCollider(btAlignedObjectArray<btVector3>& vertices, bool isEntity, const btVector3& entityTargetLocation)
{
	///perhaps we can do something special with entities (isEntity)
	///like adding a collision Triggering (as example)
	if (isEntity)
        return;

	if (vertices.size() > 0)
	{
		btScalar mass = 0.f;
        btVector3 localInertia(0,0,0);

		btTransform startTransform;		
		startTransform.setIdentity();
		startTransform.setOrigin(btVector3(0,0,-10.f));
        //startTransform.setRotation(btQuaternion(btVector3(0,0,1), 45.0f));

        //this create an internal copy of the vertices		
		btCollisionShape* shape = new btConvexHullShape(&(vertices[0].getX()),vertices.size());        
        //shape->calculateLocalInertia(mass,localInertia);
		//m_demoApp->m_collisionShapes.push_back(shape);
        
    	//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
	    btDefaultMotionState* motionState = new btDefaultMotionState(startTransform);
	    btRigidBody::btRigidBodyConstructionInfo cInfo(mass,motionState,shape,localInertia);
	    btRigidBody* body = new btRigidBody(cInfo);
	    body->setContactProcessingThreshold(BT_LARGE_FLOAT);
        body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT);

	    m_dynamicsWorld->addRigidBody(body);

        buildBspOgreEntity(static_cast<btConvexShape*>(shape));
	}
}

    
    
}; // namespace