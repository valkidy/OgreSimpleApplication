#include "CSampleGrassSticks.h"

#include "BulletSoftBody/btSoftBodyHelpers.h"
#include "BulletSoftBody/btSoftRigidDynamicsWorld.h"

#include "CSamplePhysicUtility.h"

using namespace Ogre;
namespace btUtility
{
/*
 | simple data struct for create grass mesh
 */
    #define GRASS_MESH_NAME "grass"
    #define GRASS_WIDTH 4
    #define GRASS_HEIGHT 4

    struct GrassVertex
    {
        float x, y, z;
        float nx, ny, nz;
        float u, v;
    };
/* *******************************************************************************
 | implement of CGrassSticks
 ******************************************************************************* */
void 
CGrassSticks::createGrassMesh()
{    
	MeshPtr mesh = MeshManager::getSingleton().createManual(GRASS_MESH_NAME, ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

	// create a submesh with the grass material
	SubMesh* sm = mesh->createSubMesh();
	sm->setMaterialName("Examples/GrassBlades");
	sm->useSharedVertices = false;
	sm->vertexData = OGRE_NEW VertexData();
	sm->vertexData->vertexStart = 0;
	sm->vertexData->vertexCount = 12;
	sm->indexData->indexCount = 18;

	// specify a vertex format declaration for our mesh: 3 floats for position, 3 floats for normal, 2 floats for UV
	VertexDeclaration* decl = sm->vertexData->vertexDeclaration;
    decl->addElement(0, 0, VET_FLOAT3, VES_POSITION);
    decl->addElement(0, sizeof(float) * 3, VET_FLOAT3, VES_NORMAL);
    decl->addElement(0, sizeof(float) * 6, VET_FLOAT2, VES_TEXTURE_COORDINATES, 0);

	// create a vertex buffer
	HardwareVertexBufferSharedPtr vb = HardwareBufferManager::getSingleton().createVertexBuffer
		(decl->getVertexSize(0), sm->vertexData->vertexCount, HardwareBuffer::HBU_STATIC_WRITE_ONLY);

	GrassVertex* verts = (GrassVertex*)vb->lock(HardwareBuffer::HBL_DISCARD);  // start filling in vertex data

	for (unsigned int i = 0; i < 3; i++)  // each grass mesh consists of 3 planes
	{
		// planes intersect along the Y axis with 60 degrees between them
		Real x = Math::Cos(Degree(i * 60)) * GRASS_WIDTH / 2;
		Real z = Math::Sin(Degree(i * 60)) * GRASS_WIDTH / 2;

		for (unsigned int j = 0; j < 4; j++)  // each plane has 4 vertices
		{
			GrassVertex& vert = verts[i * 4 + j];

			vert.x = j < 2 ? -x : x;
			vert.y = j % 2 ? 0 : GRASS_HEIGHT;
			vert.z = j < 2 ? -z : z;

			// all normals point straight up
			vert.nx = 0;
			vert.ny = 1;
			vert.nz = 0;

			vert.u = j < 2 ? 0 : 1;
			vert.v = j % 2;
		}
	}

	vb->unlock();  // commit vertex changes

	sm->vertexData->vertexBufferBinding->setBinding(0, vb);  // bind vertex buffer to our submesh

	// create an index buffer
	sm->indexData->indexBuffer = HardwareBufferManager::getSingleton().createIndexBuffer
		(HardwareIndexBuffer::IT_16BIT, sm->indexData->indexCount, HardwareBuffer::HBU_STATIC_WRITE_ONLY);

	// start filling in index data
	Ogre::uint16* indices = (Ogre::uint16*)sm->indexData->indexBuffer->lock(HardwareBuffer::HBL_DISCARD);

	for (unsigned int i = 0; i < 3; i++)  // each grass mesh consists of 3 planes
	{
		unsigned int off = i * 4;  // each plane consists of 2 triangles

		*indices++ = 0 + off;
		*indices++ = 3 + off;
		*indices++ = 1 + off;

		*indices++ = 0 + off;
		*indices++ = 2 + off;
		*indices++ = 3 + off;
	}

	sm->indexData->indexBuffer->unlock();  // commit index changes

    // update mesh AABB
    Ogre::AxisAlignedBox aabb;
    aabb.setExtents(-1,-1,-1,1,1,1);
    mesh->_setBounds(aabb);

    // Ogre::MeshSerializer serial;
    // serial.exportMesh(mesh.getPointer(), "grass.mesh");
}


bool 
CGrassSticks::buildGrassSticks(Ogre::SceneManager* sceneMgr, btDynamicsWorld* dynamicsWorld, btSoftBodyWorldInfo &softBodyWorldInfo)
{    
    // create our grass mesh, and create a grass entity from it
    if (!sceneMgr->hasEntity(GRASS_MESH_NAME))
    {
	    createGrassMesh();
    } // End if

    const int		n=16;
	const int		sg=4;    
	const btScalar	sz=16;
	const btScalar	hg=4;
	const btScalar	in=1/(btScalar)(n-1);
    int             index = 0;

	for(int y=0;y<n;++y)
	{
		for(int x=0;x<n;++x)
		{
			const btVector3	org(-sz+sz*2*x*in,
				1,
				-sz+sz*2*y*in);
			btSoftBody*		psb=btSoftBodyHelpers::CreateRope(softBodyWorldInfo, org,
				org+btVector3(hg*0.001f,hg,0),
				sg,
				1);
			psb->m_cfg.kDP		=	0.005f;
			psb->m_cfg.kCHR		=	0.1f;
			for(int i=0;i<3;++i)
			{
				psb->generateBendingConstraints(2+i);
			}
			psb->setMass(1,0);
			psb->setTotalMass(0.01f);
                       
            static_cast<btSoftRigidDynamicsWorld*>(dynamicsWorld)->addSoftBody(psb);
            
            const Ogre::String& strIndex = Ogre::StringConverter::toString(index++);
            Ogre::Entity* grass = sceneMgr->createEntity("Grass" + strIndex, GRASS_MESH_NAME);
            Ogre::SceneNode* node = sceneMgr->getRootSceneNode()->createChildSceneNode("node_grass_" + strIndex
                ,Ogre::Vector3(org.getX(), org.getY(), org.getZ())
                ,Ogre::Quaternion(Ogre::Degree(0), Vector3::UNIT_Y));
            node->attachObject(grass);
            node->scale(1.0f, Ogre::Math::RangeRandom(0.85f, 1.15f), 1.0f);
            node->setVisible(true);
                                    
            psb->setUserPointer((void*)(grass->getSubEntity(0)));
		} // End for
	} // End for

    dynamicsWorld->setInternalTickCallback(&CGrassSticks::simulationTickCallback);
    
    return true;
}

void
CGrassSticks::simulationTickCallback(btDynamicsWorld *dynamicsWorld, btScalar timeStep)
{
    //if (mField)
    {       
        const btSoftBodyArray& softBodyArray = static_cast<btSoftRigidDynamicsWorld*>(dynamicsWorld)->getSoftBodyArray();
        int numSoftBodys = softBodyArray.size();

        btVector3 rot0(0,0,0);
        for (int i=0;i<numSoftBodys;++i)
        { 
            btSoftBody* psb = softBodyArray[i];
            int linksize = psb->m_links.size();
            
            rot0.setZero();
            for(int k=0;k<linksize;++k)
            {
	            const btSoftBody::Link&	l=psb->m_links[k];
	            rot0 += l.m_n[1]->m_x-l.m_n[0]->m_x;				            
            }
            
            const btVector3& pos0 = psb->m_nodes[0].m_x;
            Ogre::Vector4 offset(-rot0.getX(), -rot0.getY()/2.0f, -rot0.getZ(), 0);				
            Ogre::Vector4 pos(pos0.getX(), pos0.getY(), pos0.getZ(), 0);
            Ogre::Renderable* rend = static_cast<Ogre::Renderable*>(psb->getUserPointer());
            if (rend)
            {                
                rend->setCustomParameter(999, offset);
            }
        }    
    } // End if
}

/**************************************************************************************
 | build grass cluster using static geometry
 **************************************************************************************/
bool 
CGrassSticks::buildStaticGrassSticks(Ogre::SceneManager* sceneMgr)
{            
    // create our grass mesh, and create a grass entity from it
    if (!sceneMgr->hasEntity(GRASS_MESH_NAME))
    {
	    createGrassMesh();
    } // End if

    mField = sceneMgr->createStaticGeometry("GrassField");
    mField->setRegionDimensions(Ogre::Vector3(1000, 1000, 1000));
    mField->setOrigin(Ogre::Vector3(0,0,0));
    
    Ogre::Entity* ent = sceneMgr->createEntity("Grass", GRASS_MESH_NAME);

    const int		n=16;
	const int		sg=4;    
	const btScalar	sz=16;
	const btScalar	hg=4;
	const btScalar	in=1/(btScalar)(n-1);
    
	for(int y=0;y<n;++y)
	{
		for(int x=0;x<n;++x)
		{
			const btVector3	org(-sz+sz*2*x*in,
				1,
				-sz+sz*2*y*in);

            Ogre::Vector3 pos(org.getX(), org.getY(), org.getZ());
            Ogre::Quaternion ori(Ogre::Degree(0), Vector3::UNIT_Y);
            Ogre::Vector3 scale(1, Ogre::Math::RangeRandom(0.85f, 1.15f), 1);
                            
	        mField->addEntity(ent, pos, ori, scale);                    
        } // End for
    } // End for

    mField->build();
                
    return true;
}

}; // namespace