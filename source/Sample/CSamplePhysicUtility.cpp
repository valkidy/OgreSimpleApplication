#include "CSamplePhysicUtility.h"
#include "OgreMath.h"

#include "BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h"

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

// what type of terrain is generated?
enum eTerrainModel {
	eRadial			= 1,	// deterministic
	eFractal		= 2	// random
};


static const int s_gridSize = 16 + 1;// 64 + 1;  // must be (2^N) + 1
static const float s_gridSpacing = 5.0;
static const float s_gridHeightScale = 10.2;

typedef unsigned char byte_t;

byte_t*
getRawHeightfieldData(eTerrainModel model, PHY_ScalarType type, btScalar& minHeight, btScalar& maxHeight);

bool
buildHeightFieldTerrainFromImage(Ogre::Entity* ent, btDynamicsWorld* dynamicsWorld, 
                             btAlignedObjectArray<btCollisionShape*>& collisionShapes)
{
    // parameter    
    int m_upAxis = 1;
    eTerrainModel m_model = eFractal;
    PHY_ScalarType m_type = PHY_FLOAT;
    bool flipQuadEdges = false;
        
    btScalar m_minHeight, m_maxHeight;

	static byte_t* m_rawHeightfieldData = getRawHeightfieldData(m_model, m_type, m_minHeight, m_maxHeight);
	btAssert(m_rawHeightfieldData && "failed to create raw heightfield");
    
	btHeightfieldTerrainShape * heightfieldShape =  
        new btHeightfieldTerrainShape(s_gridSize, s_gridSize,
                                      m_rawHeightfieldData,
                                      s_gridHeightScale,
                                      m_minHeight, m_maxHeight,
                                      m_upAxis, m_type, flipQuadEdges);
	btAssert(heightfieldShape && "null heightfield");

	// scale the shape
	btVector3 localScaling(25.0f, 15.0f, 25.0f); // = getUpVector(m_upAxis, s_gridSpacing, 1.0);
	heightfieldShape->setLocalScaling(localScaling);

    //
    collisionShapes.push_back(heightfieldShape);

    // rigidBody
    btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0,-1,0)));
    btRigidBody::btRigidBodyConstructionInfo cInfo(0, groundMotionState, heightfieldShape, btVector3(0,0,0));
    btRigidBody* groundRigidBody = new btRigidBody(cInfo);        	
	// groundRigidBody->setContactProcessingThreshold(m_defaultContactProcessingThreshold);

    dynamicsWorld->addRigidBody(groundRigidBody);
    
    return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////
static int getByteSize
(
PHY_ScalarType type
)
{
	int size = 0;

	switch (type) {
	case PHY_FLOAT:
		size = sizeof(float);
		break;

	case PHY_UCHAR:
		size = sizeof(unsigned char);
		break;

	case PHY_SHORT:
		size = sizeof(short);
		break;

	default:
		btAssert(!"Bad heightfield data type");
	}

	return size;
}



static float
convertToFloat
(
const byte_t * p,
PHY_ScalarType type
)
{
	btAssert(p);

	switch (type) {
	case PHY_FLOAT:
		{
			float * pf = (float *) p;
			return *pf;
		}

	case PHY_UCHAR:
		{
			unsigned char * pu = (unsigned char *) p;
			return ((*pu) * s_gridHeightScale);
		}

	case PHY_SHORT:
		{
			short * ps = (short *) p;
			return ((*ps) * s_gridHeightScale);
		}

	default:
		btAssert(!"bad type");
	}

	return 0;
}



static float
getGridHeight
(
byte_t * grid,
int i,
int j,
PHY_ScalarType type
)
{
	btAssert(grid);
	btAssert(i >= 0 && i < s_gridSize);
	btAssert(j >= 0 && j < s_gridSize);

	int bpe = getByteSize(type);
	btAssert(bpe > 0 && "bad bytes per element");

	int idx = (j * s_gridSize) + i;
	long offset = ((long) bpe) * idx;

	byte_t * p = grid + offset;

	return convertToFloat(p, type);
}



static void
convertFromFloat
(
byte_t * p,
float value,
PHY_ScalarType type
)
{
	btAssert(p && "null");

	switch (type) {
	case PHY_FLOAT:
		{
			float * pf = (float *) p;
			*pf = value;
		}
		break;

	case PHY_UCHAR:
		{
			unsigned char * pu = (unsigned char *) p;
			*pu = (unsigned char) (value / s_gridHeightScale);
		}
		break;

	case PHY_SHORT:
		{
			short * ps = (short *) p;
			*ps = (short) (value / s_gridHeightScale);
		}
		break;

	default:
		btAssert(!"bad type");
	}
}



// creates a radially-varying heightfield
static void
setRadial
(
byte_t * grid,
int bytesPerElement,
PHY_ScalarType type,
float phase = 0.0
)
{
	btAssert(grid);
	btAssert(bytesPerElement > 0);

	// min/max
	float period = 0.5 / s_gridSpacing;
	float floor = 0.0;
	float min_r = 3.0 * sqrt(s_gridSpacing);
	float magnitude = 50.0 * sqrt(s_gridSpacing);

	// pick a base_phase such that phase = 0 results in max height
	//   (this way, if you create a heightfield with phase = 0,
	//    you can rely on the min/max heights that result)
	float base_phase = (0.5 * SIMD_PI) - (period * min_r);
	phase += base_phase;

	// center of grid
	float cx = 0.5 * s_gridSize * s_gridSpacing;
	float cy = cx;		// assume square grid
	byte_t * p = grid;
	for (int i = 0; i < s_gridSize; ++i) {
		float x = i * s_gridSpacing;
		for (int j = 0; j < s_gridSize; ++j) {
			float y = j * s_gridSpacing;

			float dx = x - cx;
			float dy = y - cy;

			float r = sqrt((dx * dx) + (dy * dy));

			float z = period;
			if (r < min_r) {
				r = min_r;
			}
			z = (1.0 / r) * sin(period * r + phase);
			if (z > period) {
				z = period;
			} else if (z < -period) {
				z = -period;
			}
			z = floor + magnitude * z;

			convertFromFloat(p, z, type);
			p += bytesPerElement;
		}
	}
}



static float
randomHeight
(
int step
)
{
	return (0.33 * s_gridSpacing * s_gridSize * step * (rand() - (0.5 * RAND_MAX))) / (1.0 * RAND_MAX * s_gridSize);
}



static void
dumpGrid
(
const byte_t * grid,
int bytesPerElement,
PHY_ScalarType type,
int max
)
{
	//std::cerr << "Grid:\n";

	char buffer[32];

	for (int j = 0; j < max; ++j) {
		for (int i = 0; i < max; ++i) {
			long offset = j * s_gridSize + i;
			float z = convertToFloat(grid + offset * bytesPerElement, type);
			sprintf(buffer, "%6.2f", z);
			//std::cerr << "  " << buffer;
		}
		//std::cerr << "\n";
	}
}



static void
updateHeight
(
byte_t * p,
float new_val,
PHY_ScalarType type
)
{
	float old_val = convertToFloat(p, type);
	if (!old_val) {
		convertFromFloat(p, new_val, type);
	}
}



// creates a random, fractal heightfield
static void
setFractal
(
byte_t * grid,
int bytesPerElement,
PHY_ScalarType type,
int step
)
{
	btAssert(grid);
	btAssert(bytesPerElement > 0);
	btAssert(step > 0);
	btAssert(step < s_gridSize);

	int newStep = step / 2;
//	std::cerr << "Computing grid with step = " << step << ": before\n";
//	dumpGrid(grid, bytesPerElement, type, step + 1);

	// special case: starting (must set four corners)
	if (s_gridSize - 1 == step) {
		// pick a non-zero (possibly negative) base elevation for testing
		float base = randomHeight(step / 2);

		convertFromFloat(grid, base, type);
		convertFromFloat(grid + step * bytesPerElement, base, type);
		convertFromFloat(grid + step * s_gridSize * bytesPerElement, base, type);
		convertFromFloat(grid + (step * s_gridSize + step) * bytesPerElement, base, type);
	}

	// determine elevation of each corner
	float c00 = convertToFloat(grid, type);
	float c01 = convertToFloat(grid + step * bytesPerElement, type);
	float c10 = convertToFloat(grid + (step * s_gridSize) * bytesPerElement, type);
	float c11 = convertToFloat(grid + (step * s_gridSize + step) * bytesPerElement, type);

	// set top middle
	updateHeight(grid + newStep * bytesPerElement, 0.5 * (c00 + c01) + randomHeight(step), type);

	// set left middle
	updateHeight(grid + (newStep * s_gridSize) * bytesPerElement, 0.5 * (c00 + c10) + randomHeight(step), type);

	// set right middle
	updateHeight(grid + (newStep * s_gridSize + step) * bytesPerElement, 0.5 * (c01 + c11) + randomHeight(step), type);

	// set bottom middle
	updateHeight(grid + (step * s_gridSize + newStep) * bytesPerElement, 0.5 * (c10 + c11) + randomHeight(step), type);

	// set middle
	updateHeight(grid + (newStep * s_gridSize + newStep) * bytesPerElement, 0.25 * (c00 + c01 + c10 + c11) + randomHeight(step), type);

//	std::cerr << "Computing grid with step = " << step << ": after\n";
//	dumpGrid(grid, bytesPerElement, type, step + 1);

	// terminate?
	if (newStep < 2) {
		return;
	}

	// recurse
	setFractal(grid, bytesPerElement, type, newStep);
	setFractal(grid + newStep * bytesPerElement, bytesPerElement, type, newStep);
	setFractal(grid + (newStep * s_gridSize) * bytesPerElement, bytesPerElement, type, newStep);
	setFractal(grid + ((newStep * s_gridSize) + newStep) * bytesPerElement, bytesPerElement, type, newStep);
}

byte_t*
getRawHeightfieldData(eTerrainModel model, PHY_ScalarType type, btScalar& minHeight, btScalar& maxHeight)
{
//	std::cerr << "\nRegenerating terrain\n";
//	std::cerr << "  model = " << model << "\n";
//	std::cerr << "  type = " << type << "\n";

	long nElements = ((long) s_gridSize) * s_gridSize;
//	std::cerr << "  nElements = " << nElements << "\n";

	int bytesPerElement = getByteSize(type);
//	std::cerr << "  bytesPerElement = " << bytesPerElement << "\n";
	btAssert(bytesPerElement > 0 && "bad bytes per element");

	long nBytes = nElements * bytesPerElement;
//	std::cerr << "  nBytes = " << nBytes << "\n";
	byte_t * raw = new byte_t[nBytes];
	btAssert(raw && "out of memory");

	// populate based on model
	switch (model) {
	//case eRadial:
	//	setRadial(raw, bytesPerElement, type);
	//	break;

	case eFractal:
		for (int i = 0; i < nBytes; i++)
		{
			raw[i] = 0;
		}
		setFractal(raw, bytesPerElement, type, s_gridSize - 1);
		break;

	default:
		btAssert(!"bad model type");
	}


	// find min/max
	for (int i = 0; i < s_gridSize; ++i) {
		for (int j = 0; j < s_gridSize; ++j) {
			float z = getGridHeight(raw, i, j, type);
//			std::cerr << "i=" << i << ", j=" << j << ": z=" << z << "\n";

			// update min/max
			if (!i && !j) {
				minHeight = z;
				maxHeight = z;
			} else {
				if (z < minHeight) {
					minHeight = z;
				}
				if (z > maxHeight) {
					maxHeight = z;
				}
			}
		}
	}

	if (maxHeight < -minHeight) {
		maxHeight = -minHeight;
	}
	if (minHeight > -maxHeight) {
		minHeight = -maxHeight;
	}

//	std::cerr << "  minHeight = " << minHeight << "\n";
//	std::cerr << "  maxHeight = " << maxHeight << "\n";

	return raw;
}