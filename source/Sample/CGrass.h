#pragma once
#include "BulletSoftBody/btSoftRigidDynamicsWorld.h"
#include "BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h"

#include "COgreHeader.h"
    using namespace Ogre;
    
namespace Grass 
{
#define GRASS_WIDTH 4
#define GRASS_HEIGHT 4

    struct GrassVertex
	{
        float x, y, z;
        float nx, ny, nz;
        float u, v;
    };

    void createGrassMesh();
    void buildGrass(Ogre::SceneManager* mSceneMgr, Ogre::StaticGeometry* &mField, btDynamicsWorld* dyn)
    {
        const btSoftBodyArray& softBodyArray = ((btSoftRigidDynamicsWorld*)dyn)->getSoftBodyArray();
        int numSoftBodys = softBodyArray.size();


        // create our grass mesh, and create a grass entity from it
		createGrassMesh();
		//Entity* grass = mSceneMgr->createEntity("Grass", "grass");
     
		// create a static geometry field, which we will populate with grass
		mField = mSceneMgr->createStaticGeometry("Field");
		mField->setRegionDimensions(Vector3(140, 140, 140));
        mField->setOrigin(Vector3(0, 0, 0));
		//mField->setOrigin(Vector3(70, 70, 70));

        const int		n=16;
	    const int		sg=4;
	    const btScalar	sz=30;
	    const btScalar	hg=4;
	    const btScalar	in=1/(btScalar)(n-1);
        
		// add grass uniformly throughout the field, with some random variations
        int index = 0;
	    for(int y=0;y<n;++y)
	    {
		    for(int x=0;x<n;++x)
		    {
                Ogre::Entity* grass = mSceneMgr->createEntity("Grass" + Ogre::StringConverter::toString(index), "grass");
                const Ogre::MaterialPtr& mat = Ogre::MaterialManager::getSingleton().getByName("Examples/GrassBlades");
                grass->getSubEntity(0)->setMaterial(mat->clone("GrassCopy" + Ogre::StringConverter::toString(index)));
				//Vector3 pos(x + Math::RangeRandom(-7, 7), 0, z + Math::RangeRandom(-7, 7));
                Vector3 pos(-sz+sz*2*x*in, 1, -sz+sz*2*y*in);

				//Quaternion ori(Degree(Math::RangeRandom(0, 359)), Vector3::UNIT_Y);
                Quaternion ori(Degree(0), Vector3::UNIT_Y);
				Vector3 scale(1, Math::RangeRandom(0.85, 1.15), 1);
                
                if (index < numSoftBodys)
                {
                    softBodyArray[index]->setUserPointer((void*)(grass->getSubEntity(0)));
                }

				mField->addEntity(grass, pos, ori, scale);

                ++index;
			}
		}
        
		mField->build();  // build our static geometry (bake the grass into it)
    }
    
    void createGrassMesh()
	{
		MeshPtr mesh = MeshManager::getSingleton().createManual("grass", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

		// create a submesh with the grass material
		SubMesh* sm = mesh->createSubMesh();
		//sm->setMaterialName("Examples/GrassBlades");
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
	}

}; // namespace